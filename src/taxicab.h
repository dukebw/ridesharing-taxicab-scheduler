#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include <SDL2/SDL.h>

// NOTE(brendan): struct containing all the 
struct TaxiState {
  EdgeWeightedDigraph roadNetwork;
  SDL_Renderer *renderer;
  bool graphInitialized;
};

// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState);

#endif /* TAXICAB_H */
