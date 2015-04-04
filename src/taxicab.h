#ifndef TAXICAB_H
#define TAXICAB_H

#include "edge_weighted_digraph.h"
#include "taxi_searching.h"
#include "helper.h"
#include <SDL2/SDL.h>

#define NUMBER_OF_IMAGES 2
#define NUMBER_OF_TAXIS 3

enum {BACKGROUND_TEXTURE, TAXI_TEXTURE};

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
  List<DirectedEdge *> *shortestPath;
};

// NOTE(brendan): struct containing all the state for the application
struct TaxiState {
  bool graphInitialized;
  int screenWidth;
  int screenHeight;
  SDL_Renderer *renderer;
  SDL_Texture *textures[NUMBER_OF_IMAGES];
  EdgeWeightedDigraph roadNetwork;
  Point intersectionCoords[INTERSECTIONS];
  Taxi taxis[NUMBER_OF_TAXIS];
};

// TODO(brendan): put in update_and_render to avoid cycles in uses diagram
// NOTE(brendan): does updating and rendering
void updateAndRender(TaxiState *taxiState, int dt);

#endif /* TAXICAB_H */
