#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include "mem.h"
#include "msg.h"
#include "defs.h"
#include "buffer.h"
#include "alphabet.h"
#include "common.h"
#include "pmodels.h"
#include "tolerant.h"
#include "context.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"
#include "mix.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - D E C O M P R E S S O R - - - - - - - - - - - -


uint64_t garbage;
void Decompress(Parameters *P, CModel **cModels, uint8_t id){
  FILE        *Reader  = Fopen(P->tar[id], "r");
  char        *name    = ReplaceSubStr(P->tar[id], ".co", ".de");
  FILE        *Writter = Fopen(name, "w");
  uint64_t    nSymbols = 0;
  uint32_t    n, k, x, cModel, totModels, j;
  int32_t     idxOut = 0;
  uint8_t     *outBuffer, sym = 0, *pos;
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  PModel      **pModel, *MX;
  FloatPModel *PT;
  CMWeight    *WM;
  uint64_t    i = 0;

  if(P->verbose)
    fprintf(stderr, "Decompressing %"PRIu64" symbols of target %d ...\n",
    P[id].size, id + 1);

  startinputtingbits();
  start_decode(Reader);

  P[id].watermark        = ReadNBits(       WATERMARK_BITS, Reader);
  garbage                = ReadNBits(        CHECKSUM_BITS, Reader);
  P[id].size             = ReadNBits(            SIZE_BITS, Reader);
  P[id].low              = ReadNBits(             LOW_BITS, Reader);
  ALPHABET *AL = CreateAlphabet(P[id].low);
  AL->length   = P[id].size;
  AL->nLow               = ReadNBits(           N_LOW_BITS, Reader);
  for(x = 0 ; x < AL->nLow ; ++x)
    AL->lowAlpha[x]      = ReadNBits(         LOW_SYM_BITS, Reader);
  AL->cardinality        = ReadNBits(     CARDINALITY_BITS, Reader);
  for(x = 0 ; x < 256 ; ++x)
    AL->revMap[x] = INVALID_SYMBOL;
  for(x = 0 ; x < AL->cardinality ; ++x){
    AL->toChars[x]       = ReadNBits(             SYM_BITS, Reader);
    AL->revMap[(uint8_t) AL->toChars[x]] = x;
    }
  P[id].nModels          = ReadNBits(        N_MODELS_BITS, Reader);
  for(k = 0 ; k < P[id].nModels ; ++k){
    P[id].model[k].ctx   = ReadNBits(             CTX_BITS, Reader);
    P[id].model[k].den   = ReadNBits(       ALPHA_DEN_BITS, Reader);
    P[id].model[k].gamma = ReadNBits(           GAMMA_BITS, Reader) / 65534.0;
    P[id].model[k].edits = ReadNBits(           EDITS_BITS, Reader);
    if(P[id].model[k].edits != 0){
      P[id].model[k].eGamma = ReadNBits(      E_GAMMA_BITS, Reader) / 65534.0;
      P[id].model[k].eDen   = ReadNBits(        E_DEN_BITS, Reader);
      }
    P[id].model[k].type  = ReadNBits(            TYPE_BITS, Reader);
    }

  uint32_t ulr              = ReadNBits(BITS_LR,         Reader);
  uint32_t hs               = ReadNBits(BITS_HS,          Reader);

  union {
    float    from;
    uint32_t to;
  } pun = { .to = ulr };
  float lr = pun.from;

  printf("lr: %g, hs: %d\n", lr, hs);
  if(P->verbose)
    PrintAlphabet(AL);

  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P[id].nModels;
  for(n = 0 ; n < P[id].nModels ; ++n)
    if(P[id].model[n].edits != 0)
      ++totModels; // TOLERANT

  nSymbols      = P[id].size;
  pModel        = (PModel  **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n]   = CreatePModel(AL->cardinality);
  MX            = CreatePModel(AL->cardinality);
  PT            = CreateFloatPModel(AL->cardinality);
  outBuffer     = (uint8_t  *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  WM            = CreateWeightModel(totModels);

  for(n = 0 ; n < P[id].nModels ; ++n){
    if(P[id].model[n].type == TARGET)
      cModels[n] = CreateCModel(P[id].model[n].ctx , P[id].model[n].den,
      TARGET, P[id].model[n].edits, P[id].model[n].eDen, AL->cardinality,
      P[id].model[n].gamma, P[id].model[n].eGamma);
    }

  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P[id].nModels ; ++n){
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P[id].model[n].edits != 0){
      WM->gamma[pIdx++] = cModels[n]->eGamma;
      }
    }

  int nmodels = totModels + 1;
  int alphabet_size = AL->cardinality;
  printf("nmodels: %d, nsymbols: %d\n", nmodels, alphabet_size);
  float **probs = calloc(nmodels, sizeof(float *));
  for(n = 0; n < nmodels; ++n) {
    probs[n] = calloc(alphabet_size, sizeof(float));
  }

  long **freqs = calloc(totModels, sizeof(long *));
  for (n = 0; n < totModels; ++n) {
    freqs[n] = calloc(alphabet_size, sizeof(long));
  }
  long *sums = calloc(totModels, sizeof(long));

  mix_state_t *mxs = mix_init(nmodels, alphabet_size, hs);
  
  double expacbits = 0;
  double expac2bits = 0;

  // pre train
  for(i = 0; i < 1000; ++i) {  
    for(j = 0 ; j < alphabet_size ; ++j) {
      for(n = 0; n < nmodels; ++n) {
	probs[n][j] = 1.0;
      }
      mix(mxs, probs);
      mix_update_state(mxs, probs, j, 0.4);
      for(n = 0; n < nmodels; ++n) {
	for(k = 0 ; k < alphabet_size ; ++k) {
	  probs[n][k] = 0;
	}
      }
    }
  }

  // pre train freqs not available in decompressor?
  /*
  uint64_t tcount = 0;
  for(j = 0 ; j < ALPHABET_MAX_SIZE ; ++j) {
    tcount += AL->counts[j];
  }
  float fr[alphabet_size];
  i = 0;
  for(j = 0 ; j < ALPHABET_MAX_SIZE ; ++j) {
    if(AL->counts[j] > 0) {
      fr[i] = (float)AL->counts[j] / tcount;
      i++;
    }
  }
  for(n = 0; n < nmodels; ++n) {
    for(k = 0 ; k < alphabet_size ; ++k) {
      probs[n][k] = 0;
    }
  }

  float tdata[mxs->nsymbols];
  for(j = 0 ; j < mxs->nsymbols; ++j) {
    tdata[j] = fr[j];
  }
  
  for(i = 0; i < 1000; ++i) {
    mix(mxs, probs);
    ann_train(mxs->ann, tdata, 1);
  }
  */
  i = 0;
  while(nSymbols--){
    CalcProgress(P[id].size, ++i);

    memset((void *)PT->freqs, 0, AL->cardinality * sizeof(double));

    n = 0;
    pos = &symBuf->buf[symBuf->idx-1];
    for(cModel = 0 ; cModel < P[id].nModels ; ++cModel){
      CModel *CM = cModels[cModel];
      GetPModelIdx(pos, CM);
      ComputePModel(CM, pModel[n], CM->pModelIdx, CM->alphaDen, freqs[n], &sums[n]);
      ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
      if(CM->edits != 0){
        ++n;
        CM->TM->idx = GetPModelIdxCorr(CM->TM->seq->buf+
        CM->TM->seq->idx-1, CM, CM->TM->idx);
        ComputePModel(CM, pModel[n], CM->TM->idx, CM->TM->den, freqs[n], &sums[n]);
        ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
        }
      ++n;
      }

    for(n = 0 ; n < totModels ; ++n) {
      for(j = 0 ; j < alphabet_size ; ++j) {
        probs[n][j] = (float)freqs[n][j]/sums[n];
      }
    }

    for(j = 0 ; j < alphabet_size ; ++j) {
      probs[totModels][j] = PT->freqs[j] * 2;
    }

    const float* y = mix(mxs, probs);

    float yn[alphabet_size];
    float acmix[alphabet_size];
    float sum = 0;
    for(n = 0 ; n < alphabet_size ; ++n) {
      sum += y[n];
    }

    for(n = 0 ; n < alphabet_size ; ++n) {
      yn[n] = y[n] / sum;
      acmix[n] = PT->freqs[n];
    }
    
    if(expacbits > expac2bits) {
      for(n = 0 ; n < alphabet_size ; ++n) {
	PT->freqs[n] = y[n];
      }
    }

    ComputeMXProbs(PT, MX, AL->cardinality);

    symBuf->buf[symBuf->idx] = sym = ArithDecodeSymbol(AL->cardinality, (int *)
    MX->freqs, (int) MX->sum, Reader);
    outBuffer[idxOut] = AL->toChars[sym];

    mix_update_state(mxs, probs, sym, lr);

    
    const double a = 0.999;
    const double na = 1 - a;

    double acbits = -log2(acmix[sym]);
    double ac2bits = -log2(yn[sym]);
    
    expacbits = (na * acbits) + (a * expacbits);
    expac2bits = (na * ac2bits) + (a * expac2bits);
    
    for(n = 0 ; n < P[id].nModels ; ++n)
      if(cModels[n]->edits != 0){
        cModels[n]->TM->seq->buf[cModels[n]->TM->seq->idx] = sym;
        }

    CalcDecayment(WM, pModel, sym);

    for(n = 0 ; n < P[id].nModels ; ++n)
      if(P[id].model[n].type == TARGET)
        UpdateCModelCounter(cModels[n], sym, cModels[n]->pModelIdx);

    RenormalizeWeights(WM);

    n = 0;
    for(cModel = 0 ; cModel < P[id].nModels ; ++cModel){
      if(cModels[cModel]->edits != 0){      // CORRECT CMODEL CONTEXTS
        UpdateTolerantModel(cModels[cModel]->TM, pModel[++n], sym);
        }
      ++n;
      }

    if(++idxOut == BUFFER_SIZE){
      fwrite(outBuffer, 1, idxOut, Writter);
      idxOut = 0;
      }

    UpdateCBuffer(symBuf);
    }

  if(idxOut != 0)
    fwrite(outBuffer, 1, idxOut, Writter);

  finish_decode();
  doneinputtingbits();

  fclose(Writter);
  RemovePModel(MX);
  Free(name);
  for(n = 0 ; n < P[id].nModels ; ++n){
    if(P[id].model[n].type == REFERENCE) {
      ResetCModelIdx(cModels[n]);
      RemoveCModel(cModels[n]);
    }
    else
      RemoveCModel(cModels[n]);
    }

  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);

  RemoveWeightModel(WM);
  RemoveFPModel(PT);
  Free(outBuffer);
  RemoveCBuffer(symBuf);
  RemoveAlphabet(AL);
  fclose(Reader);

  if(P->verbose == 1)
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID

  mix_free(mxs);
  free(sums);
  for (n = 0; n < totModels; ++n) {
    free(freqs[n]);
  }
  free(freqs);

  for(n = 0; n < nmodels; ++n) {
    free(probs[n]);
  }
  free(probs);

}


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -

CModel **LoadReference(Parameters *P){
  FILE      *Reader = Fopen(P->ref, "r");
  uint32_t  n, k, idxPos;
  uint64_t  nSymbols = 0, i = 0;
  uint8_t   *readerBuffer, sym;
  CBUF      *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  CModel    **cModels;

  if(P->verbose == 1)
    fprintf(stderr, "Building reference model ...\n");

  // BUILD ALPHABET
  ALPHABET *AL = CreateAlphabet(P->low);
  LoadAlphabet(AL, Reader);
  PrintAlphabet(AL);

  readerBuffer  = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));
  cModels       = (CModel **) Malloc(P->nModels * sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == REFERENCE)
      cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den, REFERENCE,
      P->model[n].edits, P->model[n].eDen, AL->cardinality, P->model[n].gamma,
      P->model[n].eGamma);

  nSymbols = NBytesInFile(Reader);

  P->checksum = 0;
  while((k = fread(readerBuffer, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){


      symBuf->buf[symBuf->idx] = sym = AL->revMap[ readerBuffer[idxPos] ];
      P->checksum = (P->checksum + (uint8_t) sym);

      for(n = 0 ; n < P->nModels ; ++n)
        if(P->model[n].type == REFERENCE){
          GetPModelIdx(symBuf->buf+symBuf->idx-1, cModels[n]);
          UpdateCModelCounter(cModels[n], sym, cModels[n]->pModelIdx);
          }

      UpdateCBuffer(symBuf);

      CalcProgress(nSymbols, ++i);
      }

  P->checksum %= CHECKSUMGF;
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == REFERENCE)
      ResetCModelIdx(cModels[n]);
  Free(readerBuffer);
  RemoveCBuffer(symBuf);
  RemoveAlphabet(AL);
  fclose(Reader);

  if(P->verbose == 1)
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID
  else
    fprintf(stderr, "                               \n");  // SPACES ARE VALID

  return cModels;
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char        **p = *&argv;
  CModel      **refModels;
  uint32_t    n, k, *checksum, refNModels = 0, cardinality;
  Parameters  *P;
  FILE        *Reader = NULL;
  uint8_t     help, verbose, force, nTar = 1;
  clock_t     stop = 0, start = clock();

  if((help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenuD();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  verbose  = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  force    = ArgsState  (DEFAULT_FORCE,   p, argc, "-f");

  for(n = 0 ; n != strlen(argv[argc-1]) ; ++n)
    if(argv[argc-1][n] == ':')
      ++nTar;

  P        = (Parameters *) Malloc(nTar * sizeof(Parameters));
  checksum = (uint32_t   *) Calloc(nTar , sizeof(uint32_t));

  P[0].force   = force;
  P[0].verbose = verbose;
  P[0].nTar    = ReadFNames (P, argv[argc-1]);
  P[0].ref     = ArgsString (NULL, p, argc, "-r");
  for(n = 0 ; n < nTar ; ++n){
    Reader = Fopen(P[0].tar[n], "r");
    startinputtingbits();
    start_decode(Reader);

    refNModels = 0;
    P[n].watermark = ReadNBits(          WATERMARK_BITS, Reader);
    if(P[n].watermark != WATERMARK){
      fprintf(stderr, "Error: Invalid compressed file to decompress!\n");
      return 1;
      }
    checksum[n]    = ReadNBits(           CHECKSUM_BITS, Reader);
    P[n].size      = ReadNBits(               SIZE_BITS, Reader);
    ///////////////////////////////////////////////////////////
    P[n].low       = ReadNBits(                LOW_BITS, Reader);
    uint32_t nLow  = ReadNBits(              N_LOW_BITS, Reader);
    uint32_t x;
    for(x = 0 ; x < nLow ; ++x)
      garbage      = ReadNBits(            LOW_SYM_BITS, Reader);
    ///////////////////////////////////////////////////////////
    cardinality    = ReadNBits(        CARDINALITY_BITS, Reader);
    for(k = 0 ; k < cardinality ; ++k)
      garbage      = ReadNBits(                SYM_BITS,  Reader);
    P[n].nModels   = ReadNBits(           N_MODELS_BITS, Reader);
    P[n].model     = (ModelPar *) Calloc(P[n].nModels, sizeof(ModelPar));
    for(k = 0 ; k < P[n].nModels ; ++k){
      P[n].model[k].ctx   = ReadNBits(         CTX_BITS, Reader);
      P[n].model[k].den   = ReadNBits(   ALPHA_DEN_BITS, Reader);
      P[n].model[k].gamma = ReadNBits(       GAMMA_BITS, Reader) / 65534.0;
      P[n].model[k].edits = ReadNBits(       EDITS_BITS, Reader);
      if(P[n].model[k].edits != 0){
        P[n].model[k].eGamma = ReadNBits(  E_GAMMA_BITS, Reader) / 65534.0;
        P[n].model[k].eDen   = ReadNBits(    E_DEN_BITS, Reader);
        }
      P[n].model[k].type  = ReadNBits(        TYPE_BITS, Reader);
      if(P[n].model[k].type == 1)
        ++refNModels;
      }

    finish_decode();
    doneinputtingbits();
    fclose(Reader);
    }

  if(P->verbose)
    PrintArgs(P);

  if(refNModels > 0 && P[0].ref == NULL){
    fprintf(stderr, "Error: using reference model(s) in nonexistent "
    "reference sequence!\n");
    exit(1);
    }

  if(refNModels != 0)
    refModels = LoadReference(P);
  else
    refModels = (CModel **) Malloc(P->nModels * sizeof(CModel *));

  if(P->verbose && refNModels != 0)
    fprintf(stderr, "Checksum: %"PRIu64"\n", P->checksum);

  for(n = 0 ; n < nTar ; ++n){
    if(refNModels != 0){
      if(CmpCheckSum(checksum[n], P[0].checksum) == 0)
        Decompress(P, refModels, n);
      }
    else
      Decompress(P, refModels, n);
    }

  Free(checksum);
  Free(refModels);
  for(n = 0 ; n < nTar ; ++n){
    Free(P[n].model);
  }
  Free(P->tar);
  Free(P);

  stop = clock();
  fprintf(stderr, "Spent %g sec.\n", ((double)(stop-start))/CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
