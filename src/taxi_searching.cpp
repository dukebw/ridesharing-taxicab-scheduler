/* ========================================================================
   File: taxi_searching.cpp
   Date: Apr. 1/15
   Revision: 1
   Creator: Kuir Aguer, Brendan Duke, Jaeden Guo
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "helper.h"
#include "taxi_searching.h"
#include "edge_weighted_digraph.h"
#include "dijkstra_sp.h"

#define INTERSECTIONS 64

global_variable List<int> *spAllPairs[INTERSECTIONS][INTERSECTIONS];

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): INPUT: source and destination vertices.
// OUTPUT: pointer to shortest path from that source to the destination vertex
List<int> *getShortestPath(int source, int dest)
{
  return spAllPairs[source][dest];
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

// NOTE(brendan): INPUT: edge weighted digraph. OUTPUT: none. UPDATE:none.
// initializes the spAllPairs global in this module to have all the shortest
// paths between all pairs in digraph
internal void
makeAllShortestPaths(EdgeWeightedDigraph *digraph)
{
#if 0
  for (int vertexFrom = 0; vertexFrom < digraph->vertices; ++vertexFrom) {
    makeSPTree();
    for (int vertexTo = 0; vertexTo < digraph->vertices; ++vertexTo) {
      spAllPairs[vertexFrom][vertexTo] = pathTo(spTree[vertexFrom], vertexTo);
    }
  }
#endif
}
