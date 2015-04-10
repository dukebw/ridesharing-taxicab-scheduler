/* ========================================================================
   File: edge_weighted_digraph.cpp
   Date: Mar. 26/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   This file implements an edge-weighted digraph using adjacency lists.
   ======================================================================== */

#include "edge_weighted_digraph.h"
#include <stdlib.h>
#include <stdio.h>

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal void
destroyAdjacencyLists(EdgeWeightedDigraph *digraph);

internal void
printEdge(DirectedEdge *edge);

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// TODO(brendan): remove?; debugging purposes
void printGraph(EdgeWeightedDigraph *digraph) 
{
  if (digraph) {
    for (int32 vertexIndex = 0;
         vertexIndex < digraph->vertices;
         ++vertexIndex) {
      List<DirectedEdge *>::traverseList(printEdge, digraph->adj[vertexIndex]);
      printf("\n");
    }
  }
}

// NOTE(brendan): add the vertex (from, to) to the digraph
void addEdge(EdgeWeightedDigraph *digraph, int32 from, int32 to, real32 weight) 
{
  DirectedEdge *edge = (DirectedEdge *)malloc(sizeof(DirectedEdge));
  if(edge) {
    edge->from = from;
    edge->to = to;
    edge->weight = weight;
    if (digraph) {
      digraph->adj[from] = 
        List<DirectedEdge *>::addToList(edge, digraph->adj[from]);
      ++digraph->edges;
    }
    else {
      printf("bad digraph -- passed 0\n");
    }
  } 
  else {
    printf("bad edge allocation -- passed 0\n");
  }
}

// NOTE(brendan): initialize EdgeWeighted Digraph with V vertices
// INPUT: number of vertices we want. OUTPUT: digraph of size V in first param
void makeEdgeWeightedDigraph(EdgeWeightedDigraph *digraph, int32 vertices) 
{
  // TODO(brendan): assert inputs not null
  destroyAdjacencyLists(digraph);
  digraph->vertices = vertices;
  digraph->adj = 
    (List<DirectedEdge *> **)malloc(vertices*sizeof(List<DirectedEdge *> *));
  // NOTE(brendan): every malloc introduces a failure point
  if (digraph->adj) {
    for (int32 vertexIndex = 0;
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


// NOTE(brendan): INPUT: edge. OUTPUT: none. frees edge. wrapped to pass
// to traverseList
void freeDirectedEdge(DirectedEdge *edge) 
{ 
  free(edge); 
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

// NOTE(brendan): frees adjacency lists of digraph
// Client expected to set adj to 0
internal void
destroyAdjacencyLists(EdgeWeightedDigraph *digraph) 
{
  if (digraph) {
    for (int32 vertexIndex = 0; 
         vertexIndex < digraph->vertices; 
         ++vertexIndex) {
      List<DirectedEdge *>::traverseList(freeDirectedEdge, 
                                         digraph->adj[vertexIndex]);
      List<DirectedEdge *>::emptyList(&digraph->adj[vertexIndex]);
      digraph->adj[vertexIndex] = 0;
    }
    free(digraph->adj);
    digraph->adj = 0;
  }
}

// NOTE(brendan): prints an edge's to, from and weight
internal void
printEdge(DirectedEdge *edge) 
{
  if (edge) {
    printf("%d -> %d (%.2f) ", edge->from, edge->to, edge->weight);
  }
}
