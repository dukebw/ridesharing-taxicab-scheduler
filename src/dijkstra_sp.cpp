/* ========================================================================
   File: dijkstra_sp.cpp
   Date: Apr. 1/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "dijkstra_sp.h"
#include <math.h>
#include "helper.h"

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal void relax(void *spTreePtr, void *minPQPtr, DirectedEdge *edge);

// -------------------------------------------------------------------------
// Access functions
// -------------------------------------------------------------------------

// NOTE(brendan): INPUT: edge-weighted digraph, source vertex
// OUTPUT: shortest-path tree from that source vertex to all other vertices
// in the graph, returned in first parameter passed
void makeDijkstraSPTree(DijkstraSPTree *spTree, EdgeWeightedDigraph *digraph, 
                        int source)
{
  // TODO(brendan): assert spTree not null
  free(spTree->distTo);
  free(spTree->edgeTo);
  // NOTE(brendan): N = # of vertices in digraph
  int N = digraph->vertices;
  spTree->distTo = (float *)malloc(N*sizeof(float));
  spTree->edgeTo = (DirectedEdge **)malloc(N*sizeof(DirectedEdge *));
  if (spTree->distTo && spTree->edgeTo) {
    for (int distToIndex = 0; distToIndex < N; ++distToIndex) {
      spTree->distTo[distToIndex] = INFINITY;
    }
    spTree->distTo[source] = 0.0f;
    spTree->edgeTo[source] = 0;

    IndexMinPQ minpq = {};
    // NOTE(brendan): relax vertices in order of distance from source
    makeIndexMinPQ(&minpq, digraph->vertices);
    pqInsert(&minpq, source, spTree->distTo[source]);
    while (!pqEmpty(&minpq)) {
      int v = pqDelMin(&minpq);
      List<DirectedEdge *>::traverseList(relax, spTree, &minpq,
                                         digraph->adj[v]);
    }
  }
  else {
    // TODO(brendan): logging
    printf("Couldn't allocate distTo or edgeTo in makeDijkstraSPTree\n");
  }
}

// NOTE(brendan): INPUT: shortest-path tree, source vertex and dest
// vertex. OUTPUT: shortest path list of directed edges from that
// source vertex to the destination vertex, output in shortestPath param.
void
pathTo(DijkstraSPTree *spTree, ShortestPath *shortestPath, int dest)
{
  // TODO(brendan): assert input not null
  List<DirectedEdge *>::traverseList(freeDirectedEdge, shortestPath->edgeList);
  List<DirectedEdge *>::emptyList(&shortestPath->edgeList);
  shortestPath->totalWeight = spTree->distTo[dest];
  // NOTE(brendan): check if there IS a path to dest from the root of spTree
  if (spTree->distTo[dest] < INFINITY) {
    for (DirectedEdge *nextEdge = spTree->edgeTo[dest];
        nextEdge != 0;
        nextEdge = spTree->edgeTo[nextEdge->from]) {
      shortestPath->edgeList = 
        List<DirectedEdge *>::addToList(nextEdge, shortestPath->edgeList);
    }
  }
}

// -------------------------------------------------------------------------
// Local functions
// -------------------------------------------------------------------------

// NOTE(brendan): INPUT: shortest-path tree, edge. OUTPUT: none.
// UPDATE: spTree; decreases the distance to w if going from v->w is cheaper
// takes a void * so that it can be used with traverseList
internal void
relax(void *spTreePtr, void *minPQPtr, DirectedEdge *edge)
{
  DijkstraSPTree *spTree = (DijkstraSPTree *)spTreePtr;
  IndexMinPQ *minpq = (IndexMinPQ *)minPQPtr;
  // TODO(brendan): assert inputs not null
  int v = edge->from, w = edge->to;
  if (spTree->distTo[w] > spTree->distTo[v] + edge->weight) {
    spTree->distTo[w] = spTree->distTo[v] + edge->weight;
    spTree->edgeTo[w] = edge;
    if (pqContains(minpq, w)) {
      pqDecreaseWeight(minpq, w, spTree->distTo[w]);
    }
    else {
      pqInsert(minpq, w, spTree->distTo[w]);
    }
  }
}
