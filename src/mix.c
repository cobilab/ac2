#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "common.h"
#include "mix.h"

mix_state_t* mix_init(uint32_t nmodels, uint32_t nsymbols, uint32_t hs) {
  mix_state_t *mxs = malloc(sizeof(mix_state_t));

  mxs->nmodels  = nmodels;
  mxs->nsymbols = nsymbols;
  int sequence_derived = 4; // last 8, last 16, last 64 symbols
  int xs = (nmodels * nsymbols) + (sequence_derived * nsymbols);
  xs++; //bias neuron
  printf("xs: %d\n", xs);
  mxs->ann = ann_init(xs, hs, nsymbols);
  mxs->ann->x[xs - 1]  = 1.0;

  // past symbols
  mxs->symlogs1 = 8; // empirically determined
  mxs->symlogs2 = 16; // empirically determined
  mxs->symlogs3 = 64; // empirically determined
  mxs->symlog = calloc(mxs->symlogs3, sizeof(uint8_t));

  mxs->avg = calloc(nsymbols, sizeof(float));
  mxs->mean = 1.0 / nsymbols;
  return mxs;
}

float const* mix(mix_state_t* mxs, float **probs) {
  float *ret = mxs->ann->y;
  int i, j, k = 0;
  const float mean = mxs->mean;
  const uint32_t nmodels = mxs->nmodels;
  const uint32_t nsymbols = mxs->nsymbols;

  for(i = 0; i < nmodels; i++) {
    for(j = 0; j < nsymbols; j++) {
      mxs->ann->x[k++] = (probs[i][j] - mean) * 5;
    }
  }

  int sf1[nsymbols];
  int sf2[nsymbols];
  int sf3[nsymbols];
  for(i = 0; i < nsymbols; ++i) {
    sf1[i] = 0;
    sf2[i] = 0;
    sf3[i] = 0;
  }

  for(i = 0; i < mxs->symlogs1; ++i) {
    sf1[mxs->symlog[i]]++;
  }

  for(i = 0; i < mxs->symlogs2; ++i) {
    sf2[mxs->symlog[i]]++;
  }

  for(i = 0; i < mxs->symlogs3; ++i) {
    sf3[mxs->symlog[i]]++;
  }


  for(i = 0; i < nsymbols; ++i) {
    mxs->ann->x[k++] = (((float)sf1[i] / mxs->symlogs1)) * 5;
    mxs->ann->x[k++] = (((float)sf2[i] / mxs->symlogs2)) * 5;
    mxs->ann->x[k++] = (((float)sf3[i] / mxs->symlogs3)) * 5;
  }

  for(i = 0; i < nsymbols; ++i) {
    mxs->ann->x[k++] = mxs->avg[i];
  }

  ann_apply(mxs->ann);

  return ret;
}

void calc_aggregates(mix_state_t* mxs, float **probs, uint8_t sym) {
  const float alpha = 0.8;
  const float nalpha = 1 - alpha;
  const uint32_t nsymbols = mxs->nsymbols;
  // last N symbols (symbol log)
  int i;
  for(i = 0; i < nsymbols; ++i) {
    if(i == sym) {
      mxs->avg[i] = alpha + (nalpha *  mxs->avg[i]);
    } else {
      mxs->avg[i] = (nalpha *  mxs->avg[i]);
    }
  }

  for(i = mxs->symlogs3 - 1; i > 0 ; --i) {
    mxs->symlog[i] = mxs->symlog[i-1];
  }
  mxs->symlog[0] = sym;
}

void mix_update_state(mix_state_t* mxs, float **probs, uint8_t sym, float learning_rate) {
  // Train NN
  float tdata[mxs->nsymbols];
  int i;
  for(i = 0 ; i < mxs->nsymbols; ++i) {
    tdata[i] = 0.0;
  }
  tdata[sym] = 1.0;

  ann_train(mxs->ann, tdata, learning_rate);
  calc_aggregates(mxs, probs, sym);
}

void mix_free(mix_state_t* mxs) {
  ann_free(mxs->ann);
  free(mxs->avg);
  free(mxs->symlog);
  free(mxs);
}
