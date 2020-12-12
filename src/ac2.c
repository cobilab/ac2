#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include "mem.h"
#include "defs.h"
#include "msg.h"
#include "buffer.h"
#include "alphabet.h"
#include "levels.h"
#include "common.h"
#include "pmodels.h"
#include "tolerant.h"
#include "context.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"
#include "mix.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - C O M P R E S S O R - - - - - - - - - - - - - -

int Compress(Parameters *P, CModel **cModels, uint8_t id, uint32_t refNModels, INF *I, float lr, uint32_t hs){
  FILE        *Reader  = Fopen(P->tar[id], "r");
  char        *name    = concatenate(P->tar[id], ".co");
  FILE        *Writter = Fopen(name, "w");
  uint32_t    j, n, k, x, cModel, totModels, idxPos;
  uint64_t    i, size = 0;
  uint8_t     *readerBuffer, sym, *pos;
  double      se = 0;
  PModel      **pModel, *MX;
  FloatPModel *PT;
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  CMWeight    *WM;

  if(P->verbose)
    fprintf(stderr, "Analyzing data and creating models ...\n");

  #ifdef ESTIMATE
  FILE *IAE = NULL;
  char *IAEName = NULL;
  if(P->estim == 1){
    IAEName = concatenate(P->tar[id], ".iae");
    IAE = Fopen(IAEName, "w");
    }
  #endif

  _bytes_output = 0;
  size = NBytesInFile(Reader);

  // BUILD ALPHABET
  ALPHABET *AL = CreateAlphabet(P->low);
  LoadAlphabet(AL, Reader);
  if(P->verbose)
    PrintAlphabet(AL);

  // ADAPT ALPHABET FOR NON FREQUENT SYMBOLS
  AdaptAlphabetNonFrequent(AL, Reader);

  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P->nModels;
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].edits != 0){
      totModels++;
      }

  fprintf(stderr, "Using %u probabilistic models\n", totModels);

  pModel        = (PModel  **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n]   = CreatePModel(AL->cardinality);
  MX            = CreatePModel(AL->cardinality);
  PT            = CreateFloatPModel(AL->cardinality);
  WM            = CreateWeightModel(totModels);
  readerBuffer  = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));

  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == TARGET){
      cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den, TARGET,
      P->model[n].edits, P->model[n].eDen, AL->cardinality, P->model[n].hashSize,
      P->model[n].gamma, P->model[n].eGamma);
      }

  if(P->verbose){
    fprintf(stderr, "Done!\n");
    fprintf(stderr, "Compressing target sequence %d [%"PRIu64" symbols] ...\n",
    id + 1, size);
    }

  startoutputtingbits();
  start_encode();

  WriteNBits(WATERMARK,           WATERMARK_BITS, Writter);
  WriteNBits(P->checksum,          CHECKSUM_BITS, Writter);
  WriteNBits(size,                     SIZE_BITS, Writter);

  // PRE HEADER : NON FREQUENT SYMBOLS
  WriteNBits(P->low,                    LOW_BITS, Writter);
  WriteNBits(AL->nLow,                N_LOW_BITS, Writter);
  for(x = 0 ; x < AL->nLow ; ++x){
    WriteNBits(AL->lowAlpha[x],     LOW_SYM_BITS, Writter);
    }

  // REMAP ALPHABET
  // ResetAlphabet(AL);
  // PrintAlphabet(AL);

  WriteNBits(AL->cardinality,                   CARDINALITY_BITS, Writter);
  for(x = 0 ; x < AL->cardinality ; ++x)
    WriteNBits(AL->toChars[x],                          SYM_BITS, Writter);
  WriteNBits(P->nModels,                           N_MODELS_BITS, Writter);
  for(n = 0 ; n < P->nModels ; ++n){
    WriteNBits(cModels[n]->ctx,                         CTX_BITS, Writter);
    WriteNBits(cModels[n]->alphaDen,              ALPHA_DEN_BITS, Writter);
    WriteNBits((int)(cModels[n]->gamma * 65534),      GAMMA_BITS, Writter);
    WriteNBits(cModels[n]->edits,                     EDITS_BITS, Writter);
    WriteNBits(cModels[n]->hashSize,                   HASH_BITS, Writter);
    if(cModels[n]->edits != 0){
      WriteNBits((int)(cModels[n]->eGamma * 65534), E_GAMMA_BITS, Writter);
      WriteNBits(cModels[n]->TM->den,                 E_DEN_BITS, Writter);
      }
    WriteNBits(P->model[n].type,                       TYPE_BITS, Writter);
    }

  union {
    float    from;
    uint32_t to;
  } pun = { .from = lr };

  WriteNBits(pun.to,                                        BITS_LR, Writter);
  WriteNBits(hs,                                            BITS_HS, Writter);



  I[id].header = _bytes_output;

  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P->nModels ; ++n){
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P->model[n].edits != 0){
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
  double totalacbits = 0;
  double totalac2bits = 0;
  
  double expacbits = 0;
  double expac2bits = 0;

  
  // pre train
  
  for(i = 0; i < 50; ++i) {  
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

  i = 0;
  while((k = fread(readerBuffer, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){

      CalcProgress(size, ++i);

      symBuf->buf[symBuf->idx] = sym = AL->revMap[ readerBuffer[idxPos] ];
      memset((void *)PT->freqs, 0, AL->cardinality * sizeof(double));

      n = 0;
      pos = &symBuf->buf[symBuf->idx-1];
      for(cModel = 0 ; cModel < P->nModels ; ++cModel){
        CModel *CM = cModels[cModel];
        GetPModelIdx(pos, CM);
        ComputePModel(CM, pModel[n], CM->pModelIdx, CM->alphaDen, freqs[n], &sums[n]);
        ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
        if(CM->edits != 0){
          ++n;
          CM->TM->seq->buf[CM->TM->seq->idx] = sym;
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

      mix_update_state(mxs, probs, sym, lr);

      const double a = 0.999;
      const double na = 1 - a;

      double acbits = -log2(acmix[sym]);
      double ac2bits = -log2(yn[sym]);
      
      totalacbits += acbits;
      totalac2bits += ac2bits;
      
      expacbits = (na * acbits) + (a * expacbits);
      expac2bits = (na * ac2bits) + (a * expac2bits);

      AESym(sym, (int *)(MX->freqs), (int) MX->sum, Writter);
      #ifdef ESTIMATE
      if(P->estim != 0)
        fprintf(IAE, "%.3g\n", PModelSymbolNats(MX, sym) / M_LN2);
      #endif

      CalcDecayment(WM, pModel, sym);

      for(n = 0 ; n < P->nModels ; ++n)
        if(cModels[n]->ref == TARGET)
          UpdateCModelCounter(cModels[n], sym, cModels[n]->pModelIdx);

      RenormalizeWeights(WM);

      n = 0;
      for(cModel = 0 ; cModel < P->nModels ; ++cModel){
        if(cModels[cModel]->edits != 0)
          UpdateTolerantModel(cModels[cModel]->TM, pModel[++n], sym);
        ++n;
        }

      UpdateCBuffer(symBuf);
      }

  finish_encode(Writter);
  doneoutputtingbits(Writter);
  fclose(Writter);

  se = PrintSE(AL);

  #ifdef ESTIMATE
  if(P->estim == 1){
    fclose(IAE);
    Free(IAEName);
    }
  #endif

  RemovePModel(MX);
  Free(name);
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == REFERENCE) {
      ResetCModelIdx(cModels[n]);
      RemoveCModel(cModels[n]);
    }
    else
      RemoveCModel(cModels[n]);
  for(n = 0 ; n < totModels ; ++n){
    RemovePModel(pModel[n]);
    }
  Free(pModel);

  RemoveFPModel(PT);
  Free(readerBuffer);
  RemoveCBuffer(symBuf);
  int card = AL->cardinality;
  RemoveAlphabet(AL);
  RemoveWeightModel(WM);
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
  printf("%ld, %f, %f, %f\n", i, totalacbits - totalac2bits, totalacbits, totalac2bits);

  I[id].bytes = _bytes_output;
  I[id].size  = i;
  I[id].se    = se;
  return card;
}


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - R E F E R E N C E - - - - - - - - - - - - -

CModel **LoadReference(Parameters *P){
  FILE      *Reader = Fopen(P->ref, "r");
  uint32_t  n, k, idxPos;
  uint64_t  nSymbols = 0;
  uint8_t   *readerBuffer, sym;
  CBUF      *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  CModel    **cModels;

  #ifdef PROGRESS
  uint64_t  i = 0;
  #endif

  if(P->verbose == 1)
    fprintf(stdout, "Building reference model ...\n");

  // BUILD ALPHABET
  ALPHABET *AL = CreateAlphabet(P->low);
  LoadAlphabet(AL, Reader);
  PrintAlphabet(AL);

  readerBuffer  = (uint8_t *) Calloc(BUFFER_SIZE + 1, sizeof(uint8_t));
  cModels       = (CModel **) Malloc(P->nModels * sizeof(CModel *));
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == REFERENCE)
      cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den, REFERENCE,
      P->model[n].edits, P->model[n].eDen, AL->cardinality, P->model[n].hashSize,
      P->model[n].gamma, P->model[n].eGamma);

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

      #ifdef PROGRESS
      CalcProgress(nSymbols, ++i);
      #endif
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
    fprintf(stdout, "Done!                          \n");  // SPACES ARE VALID
  else
    fprintf(stdout, "                               \n");  // SPACES ARE VALID

  return cModels;
  }

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char        **p = *&argv, **xargv, *xpl = NULL;
  CModel      **refModels;
  int32_t     xargc = 0, cardinality = 1;
  uint32_t    n, k, refNModels;
  uint64_t    totalBytes, headerBytes, totalSize;
  clock_t     stop = 0, start = clock();
  double      se_average;

  Parameters  *P;
  INF         *I;

  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if((P->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenu();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgsState(0, p, argc, "-s")){
    PrintLevels();
    return EXIT_SUCCESS;
    }

  P->verbose = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v" );
  P->force   = ArgsState  (DEFAULT_FORCE,   p, argc, "-f" );
  P->estim   = ArgsState  (0,               p, argc, "-e" );
  P->level   = ArgsNum    (0,   p, argc, "-l", MIN_LEVEL, MAX_LEVEL);
  P->low     = ArgsNum    (10,  p, argc, "-t", MIN_THRESHOLD, MAX_THRESHOLD);

  uint32_t hs = ArgsNum    (32, p, argc, "-hs", 0, UINT32_MAX);
  float lr    = ArgsDouble (0.03, p, argc, "-lr");
  printf("lr: %g, hs: %d\n", lr, hs);

  P->nModels = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-rm") == 0 || strcmp(argv[n], "-tm") == 0)
      P->nModels += 1;

  if(P->nModels == 0 && P->level == 0)
    P->level = DEFAULT_LEVEL;

  if(P->level != 0){
    xpl = GetLevels(P->level);
    xargc = StrToArgv(xpl, &xargv);
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-rm") == 0 || strcmp(xargv[n], "-tm") == 0)
        P->nModels += 1;
    }

  if(P->nModels == 0){
    fprintf(stderr, "Error: at least you need to use a context model!\n");
    return 1;
    }

  P->model = (ModelPar *) Calloc(P->nModels, sizeof(ModelPar));

  k = 0;
  refNModels = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-rm") == 0){
      P->model[k++] = ArgsUniqModel(argv[n+1], 1);
      ++refNModels;
      }
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-rm") == 0){
        P->model[k++] = ArgsUniqModel(xargv[n+1], 1);
        ++refNModels;
        }
    }

  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-tm") == 0)
      P->model[k++] = ArgsUniqModel(argv[n+1], 0);
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-tm") == 0)
        P->model[k++] = ArgsUniqModel(xargv[n+1], 0);
    }

  P->ref      = ArgsString (NULL, p, argc, "-r");
  P->nTar     = ReadFNames (P, argv[argc-1]);
  P->checksum = 0;
  if(P->verbose)
    PrintArgs(P);

  if(refNModels == 0)
    refModels = (CModel **) Malloc(P->nModels * sizeof(CModel *));
  else{
    if(P->ref == NULL){
      fprintf(stderr, "Error: using reference model(s) in nonexistent "
      "reference sequence!\n");
      exit(1);
      }
    refModels = LoadReference(P);
    if(P->verbose)
      fprintf(stderr, "Checksum: %"PRIu64"\n", P->checksum);
    }

  I = (INF *) Calloc(P->nTar, sizeof(INF));

  se_average  = 0;
  totalSize   = 0;
  totalBytes  = 0;
  headerBytes = 0;
  cardinality = 1;
  for(n = 0 ; n < P->nTar ; ++n){
    cardinality  = Compress(P, refModels, n, refNModels, I, lr, hs);
    totalSize   += I[n].size;
    totalBytes  += I[n].bytes;
    headerBytes += I[n].header;
    se_average  += I[n].se;
    }
  se_average /= P->nTar;

  if(P->nTar > 1)
    for(n = 0 ; n < P->nTar ; ++n){
      fprintf(stdout, "File %d compressed bytes: %"PRIu64" (", n+1, (uint64_t)
      I[n].bytes);
      PrintHRBytes(I[n].bytes);
      fprintf(stdout, ") , Normalized Dissimilarity Rate: %.6g\n",
      (8.0*I[n].bytes)/(log2(cardinality)*I[n].size));

      fprintf(stdout, "Shannon entropy: %.6g\n", I[n].se);
      }

  fprintf(stdout, "Total bytes: %"PRIu64" (", totalBytes);
  PrintHRBytes(totalBytes);
  fprintf(stdout, "), %.5g bps, %.5g bps w/ no header\n",
  ((8.0*totalBytes)/totalSize), ((8.0*(totalBytes-headerBytes))/totalSize));

  fprintf(stdout, "Normalized Dissimilarity Rate: %.6g\n", (8.0*totalBytes)/
  (log2(cardinality)*totalSize));

  fprintf(stdout, "Average Shannon entropy: %.6g\n", se_average);
  if(P->level != 0){
    Free(xargv[0]);
    Free(xargv);
  }
  Free(I);
  Free(refModels);
  Free(P->model);
  Free(P->tar);
  Free(P);
  stop = clock();
  fprintf(stdout, "Spent %g sec.\n", ((double)(stop-start))/CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
