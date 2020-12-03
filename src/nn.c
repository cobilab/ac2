#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "nn.h"

ann_t* ann_init(uint64_t xs, uint64_t hs, uint64_t ys) {
  ann_t *ann = malloc(sizeof(ann_t));
  ann->xs = ceilf((xs + 1.0f) / 8.0f) * 8.0f;
  ann->hs = ceilf((float)hs / 8.0f) * 8.0f;
  ann->ys = ceilf((float)ys / 8.0f) * 8.0f;

  printf("xs:%ld hs:%ld ys:%ld\n", xs, hs, ys);
  printf("axs:%ld ahs:%ld ays:%ld\n", ann->xs, ann->hs, ann->ys);

  ann->x = calloc(ann->xs, sizeof(float));
  ann->h = calloc(ann->hs, sizeof(float));
  ann->y = calloc(ann->ys, sizeof(float));

  int i;
  for(i = xs; i < ann->xs; ++i) {
    ann->x[i] = 1.0f;
  }
  
  
  ann->wxh = calloc(ann->xs * ann->hs, sizeof(float));
  ann->why = calloc((hs+1) * ann->ys, sizeof(float));

  float r;
  float *w1 = ann->wxh;
  float *w2 = ann->why;
  for(i = 0; i < ann->xs * ann->hs; ++i) {
    r = (((float)rand() / RAND_MAX) * 2.0) - 1.0;
    *w1++ = r * sqrtf(6.0 / (ann->xs + ann->hs));
  }

  for(i = 0; i < (ann->hs + 1) * ann->ys; ++i) {
    r = (((float)rand() / RAND_MAX) * 2.0) - 1.0;
    *w2++ = r * sqrtf(6.0 / (ann->ys + ann->hs + 1));
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

  if(xs%8!=0 || hs%8!=0 || ys%8!=0) __builtin_unreachable();
  
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

  float maxy = ann->y[0] = ann->y[0] + *w2++;
  for(i = 1; i < ys; ++i) {
    ann->y[i] = ann->y[i] + *w2++;
    maxy = fmaxf(ann->y[i], maxy);
  }

  float total = 0;
  for(i = 0; i < ys; ++i) {
    ann->y[i] = fasterexp(ann->y[i] - maxy);
    total += ann->y[i];
  }

  for(i = 0; i < ys; ++i) {
    ann->y[i] /= total;
  }
}

void ann_train(ann_t *ann, float *t, float learning_rate) {
  int i, j;
  const uint32_t xs = ann->xs;
  const uint32_t hs = ann->hs;
  const uint32_t ys = ann->ys;

  if(xs%8!=0 || hs%8!=0 || ys%8!=0) __builtin_unreachable();
  
  float d1[ys];
  for(i = 0; i < ys; ++i) {
    d1[i] = t[i] - ann->y[i];
  }

  float *w2 = ann->why;
  float d2[hs];
  for(i = 0; i < hs; ++i) {
    d2[i] = 0;
    const float hi = ann->h[i];
    for(j = 0; j < ys; ++j) {
      d2[i] += *w2 * d1[j];
      *w2++ += learning_rate * hi * d1[j];
    }
    ann->h[i] = 0;
    d2[i] *= hi * (1 - hi);
  }

  for(i = 0; i < ys; ++i) {
    *w2++ += learning_rate * d1[i];
    ann->y[i] = 0;
  }

  float *w1 = ann->wxh;
  for(i = 0; i < xs; ++i) {
    const float xi = ann->x[i];
    for(j = 0; j < hs; ++j) {
      *w1++ += learning_rate * xi * d2[j];
    }
  }
}
