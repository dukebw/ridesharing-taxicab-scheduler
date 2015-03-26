/* ========================================================================
   File: edge_weighted_digraph.cpp
   Date: Mar. 26/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   This file implements an edge-weighted digraph using adjacency lists.
   ======================================================================== */

#include "helper.h"
#include "edge_weighted_digraph.h"
#include <stdlib.h>

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------
internal void
destroyAdjacencyLists(EdgeWeightedDigraph *digraph);

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------
// NOTE(brendan): initialize EdgeWeighted Digraph with V vertices
void makeEdgeWeightedDigraph(EdgeWeightedDigraph *digraph, int vertices) {
  if (digraph) {
    destroyAdjacencyLists(digraph);
    digraph->vertices = vertices;
    digraph->adj = 
      (List<DirectedEdge> **)malloc(vertices*sizeof(List<DirectedEdge> *));
    // NOTE(brendan): every malloc introduces a failure point
    if (digraph->adj) {
      for (int vertexIndex = 0;
          vertexIndex < vertices;
          ++vertexIndex) {
        digraph->adj[vertexIndex] = 0;
      }
    }
    else {
      // TODO(brendan): logging
      printf("Could not allocate adjacency lists\n");
    }
  }
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

// NOTE(brendan): frees adjacency lists of digraph
// Client expected to set adj to 0
internal void
destroyAdjacencyLists(EdgeWeightedDigraph *digraph) {
  if (digraph) {
    for (int vertexIndex = 0; 
         vertexIndex < digraph->vertices; 
         ++vertexIndex) {
      List<DirectedEdge>::emptyList(&digraph->adj[vertexIndex]);
    }
    free(digraph->adj);
  }
}
