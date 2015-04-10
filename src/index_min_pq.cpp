/* ========================================================================
   File: index_min_pq.cpp
   Date: Apr. 1/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "index_min_pq.h"
#include <stdlib.h>
#include <stdio.h>

// -------------------------------------------------------------------------
// Forward declarations
// -------------------------------------------------------------------------

internal void pqSwim(IndexMinPQ *minPQ, int32 key);
internal void pqSink(IndexMinPQ *minPQ, int32 key);
internal void pqExch(IndexMinPQ *minPQ, int32 keyOne, int32 keyTwo);

// -------------------------------------------------------------------------
// Access functions
// -------------------------------------------------------------------------

// NOTE(brendan): INPUT: maximum size of priority queue.
// OUTPUT: pointer to priority queue of max size NMAX, output in minPQ
void
makeIndexMinPQ(IndexMinPQ *minPQ, int32 NMAX)
{
  // TODO(brendan): assert minPQ is not 0
  minPQ->NMAX = NMAX;
  minPQ->N = 0;

  free(minPQ->pq);
  free(minPQ->qp);
  free(minPQ->weights);
  minPQ->pq = (int32 *)malloc((NMAX + 1)*sizeof(int32));
  minPQ->qp = (int32 *)malloc((NMAX + 1)*sizeof(int32));
  minPQ->weights = (real32 *)malloc((NMAX + 1)*sizeof(real32));
  if (minPQ->pq && minPQ->qp && minPQ->weights) {
    for (int32 index = 0; index < (NMAX + 1); ++index) {
      minPQ->qp[index] = -1;
    }
  }
  else {
    // TODO(brendan): logging
    printf("Failed to allocate something in makeIndexMinPQ\n");
  }
}

// NOTE(brendan): INPUT: IndexMinPQ, a index (int32), and a weight (real32)
// OUTPUT: none. UPDATE: minpq is updated; index is inserted with weight
void pqInsert(IndexMinPQ *minPQ, int32 index, real32 weight)
{
  // TODO(brendan): assert 0 <= index < NMAX and minPQ does not contain index
  // also assert minPQ is not null
  ++minPQ->N;
  minPQ->pq[minPQ->N] = index;
  minPQ->qp[index] = minPQ->N;
  minPQ->weights[index] = weight; 
  pqSwim(minPQ, minPQ->N);
}

// NOTE(brendan): INPUT: IndexMinPQ. OUTPUT: index associated with the deleted min. 
// UPDATE: minpq; min-weighted index is deleted
int32 pqDelMin(IndexMinPQ *minPQ)
{
  // TODO(brendan): assert minPQ != 0 and minPQ->N > 0
  int32 min = minPQ->pq[1];
  pqExch(minPQ, 1, minPQ->N--);
  pqSink(minPQ, 1);
  // NOTE(brendan): delete
  minPQ->qp[min] = -1;
  return min;
}

// NOTE(brendan): INPUT: min pq. OUTPUT: true if minPQ is empty; false 
// otherwise
bool32 pqEmpty(IndexMinPQ *minPQ)
{
  // TODO(brendan): assert minPQ != 0
  return (minPQ->N == 0);
}

// NOTE(brendan): INPUT: min-pq, index v. OUTPUT: true iff v is in minPQ
bool32 pqContains(IndexMinPQ *minPQ, int32 index)
{
  // TODO(brendan): assert minPQ != 0
  return (minPQ->qp[index] != -1);
}

// NOTE(brendan): INPUT: min-pq, index, new weighting for index.
// OUTPUT: none. UPDATE: min-pq; index is updated with new (lower) weighting
void pqDecreaseWeight(IndexMinPQ *minPQ, int32 index, real32 weight)
{
  // TODO(brendan): assert minPQ != 0 and 0 <= index < minPQ->NMAX
  // and minPQ contains index, and the new weight < previous weight
  minPQ->weights[index] = weight;
  pqSwim(minPQ, minPQ->qp[index]);
}

#if 0
// NOTE(brendan): INPUT: minPQ. OUTPUT: none. UPDATE: minPQ -- frees and sets
// to null the 3 arrays in minPQ
void destroyMinPQMembers(IndexMinPQ *minPQ)
{
  free(minPQ->pq);
  free(minPQ->qp);
  free(minPQ->weights);
  minPQ->pq = 0;
  minPQ->qp = 0;
  minPQ->weights = 0;
}
#endif

// -------------------------------------------------------------------------
// Local functions
// -------------------------------------------------------------------------

// NOTE(brendan): INPUT: IndexMinPQ, a key (int32) to swim. OUTPUT: none.
// UPDATE: minpq; the given key is swam upwards until minPQ is heap-sorted
internal void
pqSwim(IndexMinPQ *minPQ, int32 key)
{
  // TODO(brendan): assert minPQ is not 0
  while ((key > 1) && (minPQ->weights[key/2] > minPQ->weights[key])) {
    pqExch(minPQ, key, key/2);
    key /= 2;
  }
}

// NOTE(brendan): INPUT: IndexMinPQ minpq, a key to sink. OUTPUT: none.
// UPDATE: minPQ; key is swam down until minPQ is heap sorted
internal void
pqSink(IndexMinPQ *minPQ, int32 key)
{
  // TODO(brendan): assert minPQ != 0
  while (2*key <= minPQ->N) {
    int32 j = 2*key;
    if ((j < minPQ->N) && (minPQ->weights[j] > minPQ->weights[j + 1])) {
      ++j;
    }
    if (minPQ->weights[key] <= minPQ->weights[j]) {
      break;
    }
    pqExch(minPQ, key, j);
    key = j;
  }
}

// NOTE(brendan): INPUT: IndexMinPQ, two ints (keys) to exchange
// OUTPUT: none. UPDATE: minpq; keyOne and keyTwo elements are exchanged
internal void
pqExch(IndexMinPQ *minPQ, int32 keyOne, int32 keyTwo)
{
  // TODO(brendan): assert minPQ is not 0
  int32 swap = minPQ->pq[keyOne];
  minPQ->pq[keyOne] = minPQ->pq[keyTwo];
  minPQ->pq[keyTwo] = swap;
  minPQ->qp[minPQ->pq[keyOne]] = keyOne;
  minPQ->qp[minPQ->pq[keyTwo]] = keyTwo;
}
