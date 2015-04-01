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
#include "dijkstra_sp.h"
#include "helper.h"

// TODO(brendan): testing; remove
#define DIMENSION 8

// -------------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------------

internal float speed();

inline void
placeImage(SDL_Renderer *renderer, SDL_Texture *image, int x, int y, 
           int width, int height);

// -------------------------------------------------------------------------
// Access functions
// ------------------------------------------------------------------------

// NOTE(brendan): does updating and rendering for applications
void updateAndRender(TaxiState *taxiState) 
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

    // TODO(brendan): remove; debugging
    printf("edges: %d\n", taxiState->roadNetwork.edges);
    printGraph(&taxiState->roadNetwork);

    DijkstraSPTree spTree = {};
    makeDijkstraSPTree(&spTree, &taxiState->roadNetwork, 0);
    List<int> *testPath = pathTo(&spTree, 63);
    for (List<int> *pathPtr = testPath;
         (pathPtr != 0);
         pathPtr = pathPtr->next) {
      printf((pathPtr->next != 0) ? "%d -> " : "%d\n", pathPtr->item);
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
