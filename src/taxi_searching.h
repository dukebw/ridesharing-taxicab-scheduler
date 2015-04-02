#ifndef TAXI_SEARCHING_H
#define TAXI_SEARCHING_H

#include "linked_list.h"
#include "dijkstra_sp.h"

#define INTERSECTIONS 64

// NOTE(brendan): INPUT: source and destination vertices.
// OUTPUT: pointer to shortest path from that source to the destination vertex
ShortestPath *getShortestPath(int source, int dest);

// NOTE(brendan): INPUT: edge weighted digraph. OUTPUT: none. UPDATE:none.
// initializes the spAllPairs global in this module to have all the shortest
// paths between all pairs in digraph
void makeAllShortestPaths(EdgeWeightedDigraph *digraph);

#endif /* TAXI_SEARCHING_H */
