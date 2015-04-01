#ifndef INDEXMINPQ_H
#define INDEXMINPQ_H

// NOTE(brendan): source: Algorithims by Sedgewick and Wayne
struct IndexMinPQ {
    int NMAX;        // maximum number of elements on PQ
    int N;           // number of elements on PQ
    int *pq;        // binary heap using 1-based indexing; returns index
    int *qp;        // inverse of pq - qp[pq[i]] = pq[qp[i]] = i; gives key
    float *weights;      // weights[i] = priority of i
};

// NOTE(brendan): INPUT: maximum size of priority queue.
// OUTPUT: pointer to priority queue of max size NMAX, returned in minPQ
void makeIndexMinPQ(IndexMinPQ *minPQ, int NMAX);

// NOTE(brendan): INPUT: IndexMinPQ, an index (int), and a weight (float)
// OUTPUT: none. UPDATE: minpq is updated; vertex is inserted with weight
void pqInsert(IndexMinPQ *minPQ, int index, float weight);

// NOTE(brendan): INPUT: IndexMinPQ. OUTPUT: none. UPDATE: minpq; min-weighted
// vertex is deleted
int pqDelMin(IndexMinPQ *minPQ);

// NOTE(brendan): INPUT: min pq. OUTPUT: true if minPQ is empty; false 
// otherwise
bool pqEmpty(IndexMinPQ *minPQ);

// NOTE(brendan): INPUT: min-pq, index. OUTPUT: true iff v is in minPQ
bool pqContains(IndexMinPQ *minPQ, int index);

// NOTE(brendan): INPUT: min-pq, index, new weighting for index.
// OUTPUT: none. UPDATE: min-pq; index is updated with new (lower) weighting
void pqDecreaseWeight(IndexMinPQ *minPQ, int vertex, float weight);

#if 0
// NOTE(brendan): INPUT: minPQ. OUTPUT: none. UPDATE: minPQ -- frees and sets
// to null the 3 arrays in minPQ
void destroyMinPQMembers(IndexMinPQ *minPQ);
#endif

#endif /* INDEXMINPQ_H */
