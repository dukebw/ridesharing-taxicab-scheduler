/* ========================================================================
   File: taxicab.cpp
   Date: Apr. 5/15
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
#define TAXI_QUERY_INTERVAL 3000
#define NO_PATH -1

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal void debugPrintList(List<int> *list);

internal void
updateInsertionPoint(InsertionPoint *insertPoint, List<int> *schedule,
                     int midPoint);

internal void
setMinTaxiQuery(TaxiQuery *minTaxiQuery, Taxi *currentTaxi, 
                InsertionPoint *pickup, InsertionPoint *dropoff);

internal void
initInsertionPoint(InsertionPoint *insertionPoint, int start, int end,
                   int index, float weight);

internal float speed();

inline int randomVertex(EdgeWeightedDigraph *graph);

internal float
netPathWeight(int startPoint, int midPoint, int endPoint);

internal int
getTaxiCurrentVertex(Taxi *taxi, TaxiState *taxiState);

internal void
setTaxiVelocity(Taxi *taxi, TaxiState *taxiState);

internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int numberOfPassengers,
            Point position);

inline void
placeImage(SDL_Renderer *renderer, SDL_Texture *image, int x, int y,
           int width, int height);

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
    // NOTE(brendan): deal with queries; make a new query every queryInterval
    taxiState->timeSinceLastQuery += dt;
    if (taxiState->timeSinceLastQuery > taxiState->queryInterval) {
      taxiState->timeSinceLastQuery = 0;
      int pickupVertex = randomVertex(&taxiState->roadNetwork);
      int dropoffVertex = randomVertex(&taxiState->roadNetwork);
      // TODO(brendan): remove; testing
      printf("pickup: %d dropoff %d\n", pickupVertex, dropoffVertex);

      // NOTE(brendan): we iterate through the taxi's schedule and try to find
      // the cheapest of all combinations for insert points for the pickup and
      // dropoff of the query
      Taxi *currentTaxi = 0;
      TaxiQuery minTaxiQuery = {};
      minTaxiQuery.weight = INFINITY;
      InsertionPoint pickup = {};
      InsertionPoint dropoff = {};
      for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
        currentTaxi = &taxiState->taxis[taxiIndex];
        int currentTaxiVertex = getTaxiCurrentVertex(currentTaxi, taxiState);
        int firstPickupEnd = 
          currentTaxi->schedule ? currentTaxi->schedule->item : NO_PATH;
        float firstPickupWeight = 
          netPathWeight(currentTaxiVertex, pickupVertex, firstPickupEnd);
        initInsertionPoint(&pickup, currentTaxiVertex, firstPickupEnd, 0, 
                           firstPickupWeight);
        for (List<int> *pPickup = currentTaxi->schedule;
             pPickup;
             pPickup = pPickup->next) {
          int firstDropoffEnd;
          if (!List<int>::itemAt(&firstDropoffEnd, currentTaxi->schedule,
                                 pickup.index)) {
            firstDropoffEnd = NO_PATH;
          }
          float firstDropoffWeight = 
            netPathWeight(pickupVertex, dropoffVertex, firstDropoffEnd);
          initInsertionPoint(&dropoff, pickupVertex, firstDropoffEnd, 
                             pickup.index, firstDropoffWeight);
          setMinTaxiQuery(&minTaxiQuery, currentTaxi, &pickup, &dropoff);
          for (List<int> *pDropoff = pPickup;
               pDropoff;
               pDropoff = pDropoff->next) {
            updateInsertionPoint(&dropoff, pDropoff, dropoffVertex);
            setMinTaxiQuery(&minTaxiQuery, currentTaxi, &pickup, &dropoff);
          }
          updateInsertionPoint(&pickup, pPickup, pickupVertex);
        }
        // NOTE(brendan): dropoff will go after pickup if both are at the end
        // of the current schedule (before insertion)
        dropoff.weight = getShortestPath(pickupVertex, 
                                         dropoffVertex)->totalWeight;
        dropoff.start = pickupVertex;
        setMinTaxiQuery(&minTaxiQuery, currentTaxi, &pickup, &dropoff);
      }
      if (minTaxiQuery.taxi) {
        minTaxiQuery.taxi->schedule = 
          List<int>::insertAt(minTaxiQuery.taxi->schedule, pickupVertex, 
                              minTaxiQuery.pickupIndex);
        minTaxiQuery.taxi->schedule = 
          List<int>::insertAt(minTaxiQuery.taxi->schedule, dropoffVertex, 
                              minTaxiQuery.dropoffIndex + 1);
        // TODO(brendan): remove; debugging
        debugPrintList(minTaxiQuery.taxi->schedule);
      }
    }

    // NOTE(brendan): update taxi positions, velocities, schedules and paths
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
          // NOTE(brendan): reached a scheduled vertex
          if (!currentTaxi->shortestPath->next) {
            // TODO(brendan): remove; testing
            printf("reached %d\n", currentTaxi->schedule->item);
            currentTaxi->schedule = 
              List<int>::removeHead(currentTaxi->schedule);
          }
          currentTaxi->shortestPath = currentTaxi->shortestPath->next;
          setTaxiVelocity(currentTaxi, taxiState);
        }

        currentTaxi->position.x += currentTaxi->velocity.x*dt;
        currentTaxi->position.y += currentTaxi->velocity.y*dt;
      }
      // NOTE(brendan): get the next dropoff or pickup spot in the schedule
      // and discard last one
      else if (currentTaxi->schedule) {
        initTaxiCab(currentTaxi, taxiState, 0, currentTaxi->position);
      }
      placeImage(taxiState->renderer, taxiState->textures[TAXI_TEXTURE], 
                 (int)currentTaxi->position.x, 
                 (int)currentTaxi->position.y, 20, 20);
    }
  }
  else {
    // NOTE(brendan): do initialization of graph and taxiState
    taxiState->graphInitialized = true;
    taxiState->queryInterval = TAXI_QUERY_INTERVAL;
    taxiState->timeSinceLastQuery = 0;
    taxiState->maxPassengerCount = 3;
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

    // NOTE(brendan): init taxis
    for (int taxiIndex = 0; taxiIndex < NUMBER_OF_TAXIS; ++taxiIndex) {
      initTaxiCab(&taxiState->taxis[taxiIndex], taxiState, 0, 
                  {stride/2.0f, pitch/2.0f});
      taxiState->taxis[taxiIndex].schedule = 0;
    }
  }

  // NOTE(brendan): paint on the canvas
  SDL_RenderPresent(taxiState->renderer);
}

// -------------------------------------------------------------------------
// Local functions
// ------------------------------------------------------------------------

// TODO(brendan): testing; remove
internal void
debugPrintList(List<int> *list)
{
  for (; list; list = list->next) {
    printf(list->next ? "%d -> " : "%d\n", list->item);
  }
}

// NOTE(brendan): INPUT: min taxi-query, taxi to set, pickup and
// dropoff insertion points. OUTPUT: none. UPDATE: minTaxiQuery is
// conditionally updated if the given insertion points are cheaper
internal void
setMinTaxiQuery(TaxiQuery *minTaxiQuery, Taxi *currentTaxi, 
                InsertionPoint *pickup, InsertionPoint *dropoff)
{
  // TODO(brendan): assert inputs != 0
  float insertWeight = pickup->weight + dropoff->weight;
  if (insertWeight < minTaxiQuery->weight) {
    minTaxiQuery->taxi = currentTaxi;
    minTaxiQuery->weight = insertWeight;
    minTaxiQuery->pickupIndex = pickup->index;
    minTaxiQuery->dropoffIndex = dropoff->index;
  }
}

// NOTE(brendan): INPUT: insertion-point, schedule, mid-point vertex
// OUTPUT: none. UPDATE: insertion point is updated; this is done once
// every loop iteration in the query-insertion alg.
internal void
updateInsertionPoint(InsertionPoint *insertPoint, List<int> *schedule,
                     int midPoint)
{
  int nextEndPoint = schedule->next ? schedule->next->item : NO_PATH;
  float nextPointWeight =
    netPathWeight(schedule->item, midPoint, nextEndPoint);
  initInsertionPoint(insertPoint, schedule->item, nextEndPoint,
                     insertPoint->index + 1, nextPointWeight);
}

// NOTE(brendan): INPUT: insertion-point, start and end points, index and
// weight. OUTPUT: none. UPDATE: the insertion-point; it is initialized
internal void
initInsertionPoint(InsertionPoint *insertPoint, int start, int end,
                   int index, float weight)
{
  // TODO(brendan): assert insertPoint != 0
  insertPoint->start = start;
  insertPoint->end = end;
  insertPoint->index = index;
  insertPoint->weight = weight;
}

// NOTE(brendan): INPUT: a start point, a mid point and an end point.
// OUTPUT: the net weight from replacing start->end to start->mid->end
internal float
netPathWeight(int startPoint, int midPoint, int endPoint)
{
  if (startPoint == NO_PATH) {
    return getShortestPath(midPoint, endPoint)->totalWeight;
  }
  if (endPoint == NO_PATH) {
    return getShortestPath(startPoint, midPoint)->totalWeight;
  }
  float taxiAddedWeight = getShortestPath(startPoint, midPoint)->totalWeight;
  taxiAddedWeight += getShortestPath(midPoint, endPoint)->totalWeight;
  return taxiAddedWeight - getShortestPath(startPoint, endPoint)->totalWeight;
}

inline int randomVertex(EdgeWeightedDigraph *graph)
{
  return (int)(((double)rand()/((double)RAND_MAX + 1.0))*
               (double)graph->vertices);
}

internal float speed() 
{
  local_persist int minSpeed = 40;
  local_persist int maxSpeed = 100;
  return (float)((double)rand()/(unsigned)(RAND_MAX)*
      (maxSpeed - minSpeed) + minSpeed);
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

// INPUT: taxi, taxi-state. OUTPUT: the vertex that that taxi is currently
// at, according to its x and y position
internal int
getTaxiCurrentVertex(Taxi *taxi, TaxiState *taxiState)
{
  // NOTE(brendan): map co-ords to vertex
  float pitch = (float)taxiState->screenHeight/(float)DIMENSION;
  float stride = (float)taxiState->screenWidth/(float)DIMENSION;
  return (int)((taxi->position.x - stride/2.0f)/stride + 
               ((taxi->position.y - pitch/2.0f)/pitch)*(float)DIMENSION);
}

// INPUT: taxi-state, taxi, schedule. OUTPUT: none. UPDATE: the taxi is
// updated; its shortestPath is set based on its schedule, and its
// position and velocity are set based on its shortestPath
internal void
initTaxiCab(Taxi *taxi, TaxiState *taxiState, int numberOfPassengers,
            Point position)
{
  // TODO(brendan): assert taxi != 0, 
  taxi->numberOfPassengers = numberOfPassengers;
  taxi->position = position;
  if (taxi->schedule) {
    int taxiCurrentVertex = getTaxiCurrentVertex(taxi, taxiState);
    taxi->shortestPath = getShortestPath(taxiCurrentVertex, 
                                         taxi->schedule->item)->edgeList;
    setTaxiVelocity(taxi, taxiState);
  }
  else {
    taxi->velocity.x = 0.0f;
    taxi->velocity.y = 0.0f;
    taxi->shortestPath = 0;
  }
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
