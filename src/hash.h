#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "defs.h"

typedef uint64_t HCC;         // Size of context counters for hash tables
typedef uint8_t ENTMAX;     // Entry size (nKeys for each hIndex)

#define MAX_HASH_COUNTER  3
#define HASH_COUNTER_BITS 2

#define HASH_SIZE 16777259

typedef struct{
  uint64_t   key;            // The key stored in this entry 
  HCC        counters;      // The context counters
  }
ENTRY;

typedef struct{
  uint32_t   size;           // Size of the hash table
  ENTMAX     *entrySize;     // Number of keys in this entry
  ENTRY      **entries;      // The heads of the hash table lists
  uint32_t   nSym;           // Number of symbols
  }
HASH;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t     ZHASH               (uint64_t);
HASH         *CreateHashTable    (uint32_t);
void         InsertKey           (HASH *, uint32_t, uint64_t, uint32_t);
HCC          GetHCCounters      (HASH *, uint64_t);
void         UpdateHashCounter   (HASH *, uint32_t, uint64_t);
void         RemoveHashTable     (HASH *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
