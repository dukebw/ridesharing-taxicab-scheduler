/* ========================================================================
   File: taxicab.cpp
   Date: Mar. 26/15
   Revision: 1
   Creator: Brendan Duke
   Notice: (C) Copyright 2015 by BRD Inc. All Rights Reserved.
   ======================================================================== */

#include "taxicab.h"
#include "dijkstra_sp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// TODO(brendan): testing; remove
#define DIMENSION 8
#define SPEED_FACTOR 0.0006f

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal float speed();

inline void
placeImage(SDL_Renderer *renderer, SDL_Texture *image, int x, int y,
           int width, int height);

internal void
initTaxiCab(TaxiState *taxiState, Taxi *taxi, List<int> *schedule);

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): does updating and rendering for applications
void updateAndRender(TaxiState *taxiState, int dt) 
{
  // NOTE(brendan): Clear screen
  // TODO(brendan): need to set draw colour here?
  SDL_SetRenderDrawColor(taxiState->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(taxiState->renderer);
  SDL_RenderCopy(taxiState->renderer, 
                 taxiState->textures[BACKGROUND_TEXTURE], 0, 0);
  placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 50, 50, 
             50, 50);
  placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 400, 150, 
             50, 50);
  placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 100, 200, 
             50, 50);

  // NOTE(brendan): updating
  // TODO(brendan): do actual updating with time step
  if (taxiState->graphInitialized) {
    for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
      Taxi *currentTaxi = &taxiState->taxis[taxiIndex];
      DirectedEdge *currentEdge = currentTaxi->shortestPath->edgeList->item;
      Point destination = taxiState->intersectionCoords[currentEdge->to];
      float deltaY = destination.y - currentTaxi->position.y;
      float deltaX = destination.x - currentTaxi->position.x;
      float distanceToDest = sqrt(deltaX*deltaX + deltaY*deltaY);

      currentTaxi->position.x += SPEED_FACTOR*currentTaxi->velocity.x*dt;
      currentTaxi->position.y += SPEED_FACTOR*currentTaxi->velocity.y*dt;
      placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 
                 (int)currentTaxi->position.x, 
                 (int)currentTaxi->position.y, 20, 20);
    }
  }
  else {
    taxiState->graphInitialized = true;
    makeEdgeWeightedDigraph(&taxiState->roadNetwork, INTERSECTIONS);

    // NOTE(brendan): place vertices on screen
    float pitch = 480.0f/DIMENSION;
    float stride = 640.0f/DIMENSION;
    for (int y = 0; y < DIMENSION; ++y) {
      for (int x = 0; x < DIMENSION; ++x) {
        taxiState->intersectionCoords[x + y*DIMENSION] = {x*stride + stride/2, 
                                                          y*pitch + pitch/2};
      }
    }
    
    // NOTE(brendan): initialize edges in graph
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

    // TODO(brendan): remove; debugging
    printf("edges: %d\n", taxiState->roadNetwork.edges);
    printGraph(&taxiState->roadNetwork);

    makeAllShortestPaths(&taxiState->roadNetwork);
    for (int vertexFrom = 0; 
         vertexFrom < taxiState->roadNetwork.vertices;
         ++vertexFrom) {
      for (int vertexTo = 0; 
           vertexTo < taxiState->roadNetwork.vertices;
           ++vertexTo) {
        ShortestPath *shortestPath = getShortestPath(vertexFrom, vertexTo);
        if (shortestPath->edgeList) {
          printf("(%.2f) %d -> ", shortestPath->totalWeight, 
                                  shortestPath->edgeList->item->from);
        }
        for (List<DirectedEdge *> *pathPtr = shortestPath->edgeList;
             (pathPtr != 0);
             pathPtr = pathPtr->next) {
          printf((pathPtr->next != 0) ? "%d -> " : "%d\n", pathPtr->item->to);
        }
      }
    }


    List<int> *schedules[NUMBER_OF_TAXIS] = {};
    schedules[0] = List<int>::addToList(0, 0);
    schedules[0] = List<int>::addToList(23, schedules[0]);
    schedules[0] = List<int>::addToList(63, schedules[0]);
    schedules[0] = List<int>::addToList(0, schedules[0]);

    // NOTE(brendan): init taxis
    for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
      initTaxiCab(taxiState, &taxiState->taxis[taxiIndex], 
                  schedules[taxiIndex]);
      printf("position: %.2f %.2f velocity: %.2f %.2f\n", 
             taxiState->taxis[taxiIndex].position.x, 
             taxiState->taxis[taxiIndex].position.y, 
             taxiState->taxis[taxiIndex].velocity.x, 
             taxiState->taxis[taxiIndex].velocity.y); 
    }
  }
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

internal float speed() 
{
  local_persist int minSpeed = 40;
  local_persist int maxSpeed = 100;
  return (float)((double)rand()/(unsigned)(RAND_MAX)*
      (maxSpeed - minSpeed) + minSpeed);
}


// -------------------------------------------------------------------------
// SDL functions
// ------------------------------------------------------------------------
// NOTE(brendan): places the image at (x, y)
inline void
placeImage(SDL_Renderer *renderer, SDL_Texture *image, int x, int y, 
           int width, int height) 
{
	SDL_Rect destRect;
  destRect.x = x;
  destRect.y = y;
  destRect.w = width;
  destRect.h = height;
	SDL_RenderCopy(renderer, image, NULL, &destRect ); 
}

// INPUT: taxi-state, taxi, schedule. OUTPUT: none. UPDATE: the taxi is
// updated; its shortestPath is set based on its schedule, and its
// position and velocity are set based on its shortestPath
internal void
initTaxiCab(TaxiState *taxiState, Taxi *taxi, List<int> *schedule)
{
  // TODO(brendan): assert taxi != 0, 
  taxi->numberOfPassengers = 0;
  taxi->schedule = schedule;
  if (schedule) {
    taxi->shortestPath = getShortestPath(schedule->item, schedule->next->item);
    taxi->position.x = taxiState->intersectionCoords[schedule->item].x;
    taxi->position.y = taxiState->intersectionCoords[schedule->item].y;
    if (schedule->next) {
      DirectedEdge *currentEdge = taxi->shortestPath->edgeList->item;
      float speed = currentEdge->weight;
      float deltaX = taxiState->intersectionCoords[currentEdge->to].x -
                     taxiState->intersectionCoords[currentEdge->from].x;
      float deltaY = taxiState->intersectionCoords[currentEdge->to].y -
                     taxiState->intersectionCoords[currentEdge->from].y;
      float distance = sqrt(deltaX*deltaX + deltaY*deltaY);
      taxi->velocity.x = speed*deltaX/distance;
      taxi->velocity.y = speed*deltaY/distance;
    }
    else {
      taxi->velocity.x = 0.0f;
      taxi->velocity.y = 0.0f;
    }
  }
  else {
    // TODO(brendan): init position?
    *taxi = {};
  }
}
