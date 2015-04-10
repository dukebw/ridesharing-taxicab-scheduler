#ifndef EDGE_WEIGHTED_DIGRAPH_H
#define EDGE_WEIGHTED_DIGRAPH_H

#include "linked_list.h"
#include "helper.h"

// NOTE(brendan): edge data structure
struct DirectedEdge {
  int32 from;
  int32 to;
  real32 weight;
};

// NOTE(brendan): type for our edge-weighted directed graph
struct EdgeWeightedDigraph {
  int32 vertices;
  int32 edges;
  List<DirectedEdge *> **adj;
};

// TODO(brendan): remove?; debugging purposes
void printGraph(EdgeWeightedDigraph *digraph);

// NOTE(brendan): initialize EdgeWeighted Digraph with V vertices
void makeEdgeWeightedDigraph(EdgeWeightedDigraph *digraph, int32 vertices);

// NOTE(brendan): add the vertex (from, to) to the digraph
void addEdge(EdgeWeightedDigraph *digraph, int32 from, int32 to, real32 weight);

// NOTE(brendan): INPUT: edge. OUTPUT: none. frees edge. wrapped to pass
// to traverseList
void freeDirectedEdge(DirectedEdge *edge);

#endif /* EDGE_WEIGHTED_DIGRAPH_H */
