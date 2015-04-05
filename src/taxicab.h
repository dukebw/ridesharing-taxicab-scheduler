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
  // NOTE(brendan): allocate and free schedules; shortestPaths live
  // statically in taxi_searching
  List<int> *schedule;
  // NOTE(brendan): pointer to shortest path that the taxi is on
  List<DirectedEdge *> *shortestPath;
};

// NOTE(brendan): contains data for insertion points in the query-insertion
// alg.
struct InsertionPoint {
  int start;
  int end;
  int index;
  float weight;
};

// NOTE(brendan): struct for storing the current min taxi query to be updated
struct TaxiQuery {
  Taxi *taxi;
  float weight;
  int pickupIndex;
  int dropoffIndex;
};

// NOTE(brendan): struct containing all the state for the application
struct TaxiState {
  bool graphInitialized;
  int screenWidth;
  int screenHeight;
  int maxPassengerCount;
  int timeSinceLastQuery;
  int queryInterval;
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
