#ifndef EDGE_WEIGHTED_DIGRAPH_H
#define EDGE_WEIGHTED_DIGRAPH_H

#include "linked_list.h"

// NOTE(brendan): edge data structure
struct DirectedEdge {
  int from;
  int to;
  float weight;
};

// NOTE(brendan): type for our edge-weighted directed graph
struct EdgeWeightedDigraph {
  int vertices;
  int edges;
  List<DirectedEdge *> **adj;
};

// TODO(brendan): remove?; debugging purposes
void printGraph(EdgeWeightedDigraph *digraph);

// NOTE(brendan): initialize EdgeWeighted Digraph with V vertices
void makeEdgeWeightedDigraph(EdgeWeightedDigraph *digraph, int vertices);

// NOTE(brendan): add the vertex (from, to) to the digraph
void addEdge(EdgeWeightedDigraph *digraph, int from, int to, float weight);

#endif /* EDGE_WEIGHTED_DIGRAPH_H */
