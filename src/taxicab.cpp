/* ========================================================================
   File: taxicab.cpp
   Date: Mar. 26/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "taxicab.h"
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

// TODO(brendan): testing; remove
#define DIMENSION 8

internal float speed();

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): does updating and rendering for applications
void updateAndRender(TaxiState *taxiState) {
  if (taxiState->graphInitialized) {
  }
  else {
    makeEdgeWeightedDigraph(&taxiState->roadNetwork, DIMENSION*DIMENSION);
    taxiState->graphInitialized = true;
    
    for (int row = 0; row < DIMENSION; ++row) {
      for (int col = 0; col < DIMENSION; ++col) {
        if (row > 0) {
          addEdge(&taxiState->roadNetwork, row*DIMENSION + col, 
                  (row - 1)*DIMENSION + col, speed());
        }
        if (col > 0) {
          addEdge(&taxiState->roadNetwork, row*DIMENSION + col, 
                  row*DIMENSION + (col - 1), speed());
        }
        if (row < (DIMENSION - 1)) {
          addEdge(&taxiState->roadNetwork, row*DIMENSION + col, 
                  (row + 1)*DIMENSION + col, speed());
        }
        if (col < (DIMENSION - 1)) {
          addEdge(&taxiState->roadNetwork, row*DIMENSION + col, 
                  row*DIMENSION + (col + 1), speed());
        }
      }
    }
  }
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------
internal float speed() {
  local_persist int minSpeed = 40;
  local_persist int maxSpeed = 100;
  return (float)rand()/(RAND_MAX + 1) * (maxSpeed - minSpeed) + minSpeed;
}