#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "nn.h"
#include "common.h"

static inline float Q_rsqrt( float number )
{
  const float x2 = number * 0.5F;
  const float threehalfs = 1.5F;

  union {
    float f;
    unsigned long i;
  } conv  = { .f = number };
  conv.i  = 0x5f3759df - ( conv.i >> 1 );
  conv.f  *= ( threehalfs - ( x2 * conv.f * conv.f ) );
  return conv.f;
}


ann_t* ann_init(uint64_t xs, uint64_t hs, uint64_t ys) {
  ann_t *ann = malloc(sizeof(ann_t));
  ann->xs = xs;
  ann->hs = hs;
  ann->ys = ys;

  ann->x = calloc(xs, sizeof(float));
  ann->h = calloc(hs, sizeof(float));
  ann->y = calloc(ys, sizeof(float));

  ann->m = calloc(xs * (hs+1) * ys, sizeof(float));
  ann->v = calloc(xs * (hs+1) * ys, sizeof(float));

  ann->wxh = calloc(xs * hs, sizeof(float));
  ann->why = calloc((hs+1) * ys, sizeof(float));

  float r;
  float *w1 = ann->wxh;
  float *w2 = ann->why;
  int i;
  for(i = 0; i < xs * hs; ++i) {
    r = (((float)rand() / RAND_MAX) * 2.0) - 1.0;
    *w1++ = r * sqrtf(6.0 / (xs + hs));
  }

  for(i = 0; i < (hs + 1) * ys; ++i) {
    r = (((float)rand() / RAND_MAX) * 2.0) - 1.0;
    *w2++ = r * sqrtf(6.0 / (ys + hs + 1));
  }

  return ann;
}

void ann_free(ann_t *ann) {
  free(ann->wxh);
  free(ann->why);
  free(ann->x);
  free(ann->h);
  free(ann->y);
  free(ann);
}

void ann_apply(ann_t *ann) {
  int i, j;
  const uint32_t xs = ann->xs;
  const uint32_t hs = ann->hs;
  const uint32_t ys = ann->ys;

  float *w1 = ann->wxh;
  for(i = 0; i < xs; ++i) {
    const float xi = ann->x[i];
    for(j = 0; j < hs; ++j) {
      ann->h[j] += *w1++ * xi;
    }
  }

  float *w2 = ann->why;
  for(i = 0; i < hs; ++i) {
    const float hi = sig(ann->h[i]);
    for(j = 0; j < ys; ++j) {
      ann->y[j] += *w2++ * hi;
    }
    ann->h[i] = hi;
  }

  for(i = 0; i < ys; ++i) {
    ann->y[i] = sig(ann->y[i] + *w2++);
  }
}


const float b1 = 0.99;
const float nb1 = 1 - b1;
const float b2 = 0.9999;
const float nb2 = 1 - b2;
const float eps = 1E-8;

void ann_train(ann_t *ann, float *t, float learning_rate) {

  float *m = ann->m;
  float *v = ann->v;
  int i, j;
  const uint32_t xs = ann->xs;
  const uint32_t hs = ann->hs;
  const uint32_t ys = ann->ys;

  float d1[ys];
  float vd1[ys];
  float md1[ys];
  for(i = 0; i < ys; ++i) {
    d1[i] = ann->y[i] * (1 - ann->y[i]) * (t[i] - ann->y[i]);
    vd1[i] = nb1 * d1[i];
    md1[i] = nb2 * d1[i] * d1[i];
  }

  float *w2 = ann->why;
  float d2[hs];
  float vd2[hs];
  float md2[hs];
  for(i = 0; i < hs; ++i) {
    for(j = 0; j < ys; ++j) {
      *m = (b1 * *m) + (ann->h[i] * vd1[j]);
      m++;
    }
    m -= ys;
    const float hi2 = ann->h[i] * ann->h[i];
    for(j = 0; j < ys; ++j) {
      *v = (b2 * *v) + (hi2 * md1[j]);
      v++;
    }
    v -= ys;
    d2[i] = 0;
    for(j = 0; j < ys; ++j) {
      d2[i] += *w2 * d1[j];
      *w2++ += learning_rate * *m * Q_rsqrt(*v + eps);
      m++;
      v++;
    }
    d2[i] *= ann->h[i] * (1 - ann->h[i]);
    vd2[i] = nb1 * d2[i];
    md2[i] = nb2 * d2[i] * d2[i];
    ann->h[i] = 0;
  }

  

  for(i = 0; i < ys; ++i) {
    *m = (b1 * *m) + vd1[i];
    m++;
    ann->y[i] = 0;
  }
  m -= ys;
  for(i = 0; i < ys; ++i) {
    *v = (b2 * *v) + md1[i];
    v++;
  }
  v -= ys;
  for(i = 0; i < ys; ++i) {
    *w2++ += learning_rate * *m * Q_rsqrt(*v + eps);
    m++;
    v++;
  }

  float *w1 = ann->wxh;
  for(i = 0; i < xs; ++i) {
    for(j = 0; j < hs; ++j) {
      *m = (b1 * *m) + (ann->x[i] * vd2[j]);
      m++;
    }
    m -= hs;
    const float xi2 = ann->x[i] * ann->x[i];
    for(j = 0; j < hs; ++j) {
      *v = (b2 * *v) + (xi2 * md2[j]);
      v++;
    }
    v -= hs;
    for(j = 0; j < hs; ++j) {
      *w1++ += learning_rate * *m * Q_rsqrt(*v + eps);
      m++;
      v++;
    }
  }
}

