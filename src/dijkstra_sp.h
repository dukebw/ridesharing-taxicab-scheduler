#ifndef DIJKSTRA_SP_H
#define DIJKSTRA_SP_H

#include "linked_list.h"
#include "edge_weighted_digraph.h"
#include "index_min_pq.h"

#define EDGE_TO_ROOT -1

struct DijkstraSPTree {
  real32 *distTo;
  DirectedEdge **edgeTo;
};

struct ShortestPath {
  real32 totalWeight;
  // NOTE(brendan): ShortestPath is a list of pointers; does not
  // make copies (i.e. side-effects) the pointers point to memory allocated
  // in the DijkstraSPTree array in the taxi_searching module
  List<DirectedEdge *> *edgeList;
};

// NOTE(brendan): INPUT: edge-weighted digraph, source vertex
// OUTPUT: shortest-path tree from that source vertex to all other vertices
// in the graph, returned in first parameter passed
void makeDijkstraSPTree(DijkstraSPTree *spTree, EdgeWeightedDigraph *digraph, 
                        int32 source);

// NOTE(brendan): INPUT: edge weighted digraph, source vertex and dest
// vertex. OUTPUT: shortest path list of directed edges from that
// source vertex to the destination vertex.
void
pathTo(DijkstraSPTree *spTree, ShortestPath *shortestPath, int32 dest);

#endif /* DIJKSTRA_SP_H */
