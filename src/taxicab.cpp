/* ========================================================================
   File: taxicab.cpp
   Date: Mar. 26/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "taxicab.h"

// TODO(brendan): testing; remove
#define NUMBER_VERTICES 64

// NOTE(brendan): does updating and rendering for applications
void updateAndRender(TaxiState *taxiState) {
  if (taxiState->graphInitialized) {
  }
  else {
    makeEdgeWeightedDigraph(&taxiState->roadNetwork, NUMBER_VERTICES);
    taxiState->graphInitialized = true;
    // TODO(brendan): debugging; remove
    for (int i = 0; i < taxiState->roadNetwork.vertices; ++i) {
      if (taxiState->roadNetwork.adj[i] == 0) {
        printf("%d\n", i);
      }
    }
  }
}
