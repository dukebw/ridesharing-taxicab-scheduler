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

global_variable ShortestPath spAllPairs[INTERSECTIONS][INTERSECTIONS];

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): INPUT: source and destination vertices.
// OUTPUT: pointer to shortest path from that source to the destination vertex
ShortestPath *getShortestPath(int source, int dest)
{
  return &spAllPairs[source][dest];
}

// NOTE(brendan): INPUT: edge weighted digraph. OUTPUT: none. UPDATE:none.
// initializes the spAllPairs global in this module to have all the shortest
// paths between all pairs in digraph
void makeAllShortestPaths(EdgeWeightedDigraph *digraph)
{
  // TODO(brendan): assert digraph != 0
  local_persist DijkstraSPTree spTreesArray[INTERSECTIONS];
  for (int vertexFrom = 0; vertexFrom < digraph->vertices; ++vertexFrom) {
    makeDijkstraSPTree(&spTreesArray[vertexFrom], digraph, vertexFrom);
    for (int vertexTo = 0; vertexTo < digraph->vertices; ++vertexTo) {
       pathTo(&spTreesArray[vertexFrom], &spAllPairs[vertexFrom][vertexTo],
              vertexTo);
    }
  }
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

