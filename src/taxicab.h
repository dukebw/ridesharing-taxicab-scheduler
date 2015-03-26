#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"

// NOTE(brendan): struct containing all the 
struct TaxiState {
  EdgeWeightedDigraph roadNetwork;
  bool graphInitialized;
};

// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState);

#endif /* TAXICAB_H */
