#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include <SDL2/SDL.h>

#define NUMBER_OF_IMAGES 2

enum {BACKGROUND_TEXTURE, TAXI_TEXTURE};

// NOTE(brendan): struct containing all the 
struct TaxiState {
  EdgeWeightedDigraph roadNetwork;
  SDL_Renderer *renderer;
  SDL_Texture *textures[NUMBER_OF_IMAGES];
  bool graphInitialized;
};

// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState);

#endif /* TAXICAB_H */
