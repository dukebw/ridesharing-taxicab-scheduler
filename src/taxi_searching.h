#ifndef TAXI_SEARCHING_H
#define TAXI_SEARCHING_H

#include "linked_list.h"
#include "dijkstra_sp.h"

// NOTE(brendan): # of unique wayNodes (vertices in graph)
#define MAX_VERTICES 2048

// NOTE(brendan): INPUT: source and destination vertices.
// OUTPUT: pointer to shortest path from that source to the destination vertex
ShortestPath *getShortestPath(EdgeWeightedDigraph *digraph, int32 source,
                              int32 dest);

// NOTE(brendan): INPUT: edge weighted digraph. OUTPUT: none. UPDATE:none.
// initializes the spAllPairs global in this module to have all the shortest
// paths between all pairs in digraph
void makeAllShortestPaths(EdgeWeightedDigraph *digraph);

#endif /* TAXI_SEARCHING_H */
