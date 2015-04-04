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
#define SPEED_FACTOR 0.0012f

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal float speed();

inline void
placeImage(SDL_Renderer *renderer, SDL_Texture *image, int x, int y,
           int width, int height);

internal void
setTaxiVelocity(Taxi *taxi, TaxiState *taxiState);

internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int numberOfPassengers,
            Point position, List<int> *schedule);

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

  // NOTE(brendan): updating
  if (taxiState->graphInitialized) {
    for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
      Taxi *currentTaxi = &taxiState->taxis[taxiIndex];
      if (currentTaxi->shortestPath) {
        DirectedEdge *currentEdge = currentTaxi->shortestPath->item;
        Point destination = taxiState->intersectionCoords[currentEdge->to];
        float deltaY = destination.y - currentTaxi->position.y;
        float deltaX = destination.x - currentTaxi->position.x;

        // NOTE(brendan): taxi reaches the destination on this physics update
        float vy = currentTaxi->velocity.y;
        float vx = currentTaxi->velocity.x;
        float timeToDestX = (vx != 0.0f) ? deltaX/vx : 0.0f;
        float timeToDestY = (vy != 0.0f) ? deltaY/vy : 0.0f;
        if ((timeToDestX < dt) && (timeToDestY < dt)) {
          // NOTE(brendan): set taxi's position to its destination
          currentTaxi->position.x = destination.x;
          currentTaxi->position.y = destination.y;
          currentTaxi->shortestPath = currentTaxi->shortestPath->next;
          setTaxiVelocity(currentTaxi, taxiState);
        }

        currentTaxi->position.x += currentTaxi->velocity.x*dt;
        currentTaxi->position.y += currentTaxi->velocity.y*dt;
      }
      // NOTE(brendan): get the next dropoff or pickup spot in the schedule
      // and discard last one
      else if (currentTaxi->schedule) {
        initTaxiCab(currentTaxi, taxiState, 0, currentTaxi->position,
                    List<int>::removeHead(currentTaxi->schedule));
      }
      placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 
                 (int)currentTaxi->position.x, 
                 (int)currentTaxi->position.y, 20, 20);
    }
  }
  else {
    taxiState->graphInitialized = true;
    makeEdgeWeightedDigraph(&taxiState->roadNetwork, INTERSECTIONS);

    // NOTE(brendan): place vertices on screen
    float pitch = (float)taxiState->screenHeight/DIMENSION;
    float stride = (float)taxiState->screenWidth/DIMENSION;
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
    makeAllShortestPaths(&taxiState->roadNetwork);

    // TODO(brendan): testing; fix up
    // NOTE(brendan): creating testing schedules
    List<int> *schedules[NUMBER_OF_TAXIS] = {};
    schedules[0] = List<int>::addToList(0, 0);
    schedules[0] = List<int>::addToList(23, schedules[0]);
    schedules[0] = List<int>::addToList(63, schedules[0]);
    schedules[0] = List<int>::addToList(0, schedules[0]);

    // NOTE(brendan): init taxis
    for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
      initTaxiCab(&taxiState->taxis[taxiIndex], taxiState, 0, 
                  {stride/2.0f, pitch/2.0f}, schedules[taxiIndex]);
    }
  }

  // NOTE(brendan): paint on the canvas
  SDL_RenderPresent(taxiState->renderer);
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

// NOTE(brendan): INPUT: taxi, taxistate. OUTPUT: none. UPDATE: taxi; taxi's
// velocity is set based on the next edge in its shortestPath. If its 
// shortestPath has been traversed, its shortestPath is set to 0
// and its velocity is set to 0
internal void
setTaxiVelocity(Taxi *taxi, TaxiState *taxiState)
{
  // TODO(brendan): assert inputs != 0
  if (taxi->shortestPath) {
    DirectedEdge *currentEdge = taxi->shortestPath->item;
    float speed = currentEdge->weight;
    float deltaX = taxiState->intersectionCoords[currentEdge->to].x -
                   taxi->position.x;
    float deltaY = taxiState->intersectionCoords[currentEdge->to].y -
                   taxi->position.y;
    float distance = sqrt(deltaX*deltaX + deltaY*deltaY);
    if (distance) {
      taxi->velocity.x = SPEED_FACTOR*speed*deltaX/distance;
      taxi->velocity.y = SPEED_FACTOR*speed*deltaY/distance;
    }
    else {
      taxi->velocity.x = 0.0f;
      taxi->velocity.y = 0.0f;
    }
  }
  else {
    taxi->velocity.x = 0.0f;
    taxi->velocity.y = 0.0f;
  }
}

// INPUT: taxi-state, taxi, schedule. OUTPUT: none. UPDATE: the taxi is
// updated; its shortestPath is set based on its schedule, and its
// position and velocity are set based on its shortestPath
internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int numberOfPassengers,
            Point position, List<int> *schedule)
{
  // TODO(brendan): assert taxi != 0, 
  taxi->numberOfPassengers = numberOfPassengers;
  taxi->position = position;
  taxi->schedule = schedule;
  if (schedule) {
    // NOTE(brendan): map co-ords to vertex
    float pitch = (float)taxiState->screenHeight/DIMENSION;
    float stride = (float)taxiState->screenWidth/DIMENSION;
    int taxiCurrentVertex = 
      (int)((taxi->position.x - stride/2.0f)/stride + 
            ((taxi->position.y - pitch/2.0f)/pitch)*(float)DIMENSION);
    taxi->shortestPath = getShortestPath(taxiCurrentVertex, 
                                         schedule->item)->edgeList;
    setTaxiVelocity(taxi, taxiState);
  }
  else {
    // TODO(brendan): init position?
    taxi->velocity.x = 0.0f;
    taxi->velocity.y = 0.0f;
  }
}
