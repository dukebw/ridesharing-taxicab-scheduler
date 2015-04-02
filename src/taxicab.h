#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include "helper.h"
#include <SDL2/SDL.h>

#define NUMBER_OF_IMAGES 2

enum {BACKGROUND_TEXTURE, TAXI_TEXTURE};

// NOTE(brendan): struct containing all the state for the application
struct TaxiState {
  bool graphInitialized;
  EdgeWeightedDigraph roadNetwork;
  SDL_Renderer *renderer;
  SDL_Texture *textures[NUMBER_OF_IMAGES];
};

// NOTE(brendan): contains data that each taxi object needs to draw it
// and calculate its schedule
struct Taxi {
  int numberOfPassengers;
  // NOTE(brendan): rendering
  Point position;
  Vector velocity;
  // NOTE(brendan): for schedule-updating
  List<int> *schedule;
  // NOTE(brendan): pointer to shortest path that the taxi is on
  List<int> *shortestPath;
};

// TODO(brendan): put in update_and_render to avoid cycles in uses diagram
// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState);

#endif /* TAXICAB_H */
