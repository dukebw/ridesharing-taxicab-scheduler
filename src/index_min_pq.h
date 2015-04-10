#ifndef INDEXMINPQ_H
#define INDEXMINPQ_H

#include "helper.h"

// NOTE(brendan): source: Algorithims by Sedgewick and Wayne
struct IndexMinPQ {
    int32 NMAX;        // maximum number of elements on PQ
    int32 N;           // number of elements on PQ
    int32 *pq;        // binary heap using 1-based indexing; returns index
    int32 *qp;        // inverse of pq - qp[pq[i]] = pq[qp[i]] = i; gives key
    real32 *weights;      // weights[i] = priority of i
};

// NOTE(brendan): INPUT: maximum size of priority queue.
// OUTPUT: pointer to priority queue of max size NMAX, returned in minPQ
void makeIndexMinPQ(IndexMinPQ *minPQ, int32 NMAX);

// NOTE(brendan): INPUT: IndexMinPQ, an index (int32), and a weight (real32)
// OUTPUT: none. UPDATE: minpq is updated; vertex is inserted with weight
void pqInsert(IndexMinPQ *minPQ, int32 index, real32 weight);

// NOTE(brendan): INPUT: IndexMinPQ. OUTPUT: none. UPDATE: minpq; min-weighted
// vertex is deleted
int32 pqDelMin(IndexMinPQ *minPQ);

// NOTE(brendan): INPUT: min pq. OUTPUT: true if minPQ is empty; false 
// otherwise
bool32 pqEmpty(IndexMinPQ *minPQ);

// NOTE(brendan): INPUT: min-pq, index. OUTPUT: true iff v is in minPQ
bool32 pqContains(IndexMinPQ *minPQ, int32 index);

// NOTE(brendan): INPUT: min-pq, index, new weighting for index.
// OUTPUT: none. UPDATE: min-pq; index is updated with new (lower) weighting
void pqDecreaseWeight(IndexMinPQ *minPQ, int32 vertex, real32 weight);

#if 0
// NOTE(brendan): INPUT: minPQ. OUTPUT: none. UPDATE: minPQ -- frees and sets
// to null the 3 arrays in minPQ
void destroyMinPQMembers(IndexMinPQ *minPQ);
#endif

#endif /* INDEXMINPQ_H */
