/* ========================================================================
File: sdl2_taxicab.cpp
Date: Mar. 19/15
Revision: 1
Creator: Brendan Duke
Notice: (C) Copyright 2015 by ADK Inc. All Rights Reserved.
======================================================================== */

#include "helper.h"
#include "taxicab.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>

// NOTE(brendan): The window we'll be rendering to
global_variable SDL_Window* gWindow = NULL;
global_variable SDL_Texture *gBackgroundTexture = NULL;
global_variable SDL_Texture *gTaxiTexture = NULL;

// NOTE(brendan): Screen dimension constants
internal const int SCREEN_WIDTH = 640;
internal const int SCREEN_HEIGHT = 480;

// NOTE(brendan): does the necessary initialization of SDL2 at program startup
internal bool 
sdl2Init(SDL_Renderer **renderer)
{
  // NOTE(brendan): Initialization flag
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else {
    // NOTE(brendan): Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", 
          SDL_GetError());
      success = false;
    }
    else {
      // NOTE(brendan): Create vsynced renderer for window
      *renderer = SDL_CreateRenderer(gWindow, -1, 
          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", 
            SDL_GetError());
        success = false;
      }
      else {
        // NOTE(brendan): Initialize renderer color
        SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      }
    }
  }

  return success;
}

internal SDL_Texture * 
sdl2LoadTexture(char *fileName, SDL_Renderer *renderer) 
{
  // NOTE(brendan): The final optimized image
  SDL_Texture *newTexture = NULL;

  // NOTE(brendan): Load image at specified path
  SDL_Surface *loadedSurface = SDL_LoadBMP(fileName);
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", fileName, 
        SDL_GetError());
  }
  else {
    SDL_Rect scaleRect;
    scaleRect.x = 0;
    scaleRect.y = 0;
    scaleRect.w = loadedSurface->w;
    scaleRect.h = loadedSurface->h;
    SDL_Surface *scaledSurface = 
      SDL_CreateRGBSurface(0, scaleRect.w, scaleRect.h, 32, 0, 0, 0, 0);
    if (scaledSurface == NULL) {
      printf("Couldn't create surface scaledSurface");
    } 
    else {
      SDL_BlitScaled(loadedSurface, NULL, scaledSurface, &scaleRect);

      // Color key image
      SDL_SetColorKey( scaledSurface, SDL_TRUE, 
          SDL_MapRGB( scaledSurface->format, 0xFF, 0xFF, 0xFF));

      //Create texture from surface pixels
      newTexture = SDL_CreateTextureFromSurface(renderer, scaledSurface);
      if (newTexture == NULL) {
        printf("Unable to create texture from %s! SDL Error: %s\n", 
            fileName,SDL_GetError());
      } 

      // NOTE(brendan): Get rid of old loaded surface
      SDL_FreeSurface(scaledSurface);
    }
  }
  return newTexture;
}

internal bool 
sdl2LoadMedia(SDL_Renderer *renderer) 
{
  // NOTE(brendan): Loading success flag
  bool success = true;

  // NOTE(brendan): Load grid texture
  // TODO(brendan): remove; testing
  gBackgroundTexture = sdl2LoadTexture((char *)"../misc/simple_grid.bmp", renderer);
  if (gBackgroundTexture == 0) {
    printf( "Failed to load grid texture!\n" );
    success = false;
  }

  return success;
}

// NOTE(brendan): handle events, e.g. mouse clicks, key presses etc.
// returns false if quit button is pressed; true otherwise.
internal bool
sdl2HandleEvents() 
{
  // NOTE(brendan): event handler
  SDL_Event e;
  // NOTE(brendan): Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    //User requests quit
    if (e.type == SDL_QUIT) {
      return false;
    }
  }
  return true;
}

internal void
sdl2Render(SDL_Renderer *renderer) 
{
  // NOTE(brendan): Clear screen
  // TODO(brendan): need to set draw colour here?
  SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, gBackgroundTexture, 0, 0);
  SDL_RenderPresent(renderer);
}

// NOTE(brendan): free resources and quit SDL
void sdl2Close() 
{
	// NOTE(brendan): Free loaded images
  SDL_DestroyTexture(gBackgroundTexture);
  gBackgroundTexture = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// NOTE(brendan): Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char *argv[]) 
{
  // NOTE(brendan): state (model) of the taxis on the road network
  TaxiState taxiState = {};

  if (!sdl2Init(&taxiState.renderer)) {
    printf("Failed to initialize!\n");
  }
  else if (!sdl2LoadMedia(taxiState.renderer)) {
    printf("Failed to load media!\n");
  }
  else {
    // NOTE(brendan): Main loop flag
    bool globalRunning = true;

    srand((unsigned)time(0));

    // NOTE(brendan): while application is running
    while (globalRunning) {
      globalRunning = sdl2HandleEvents();
      updateAndRender(&taxiState);
      sdl2Render(taxiState.renderer);
    }
  }

	// NOTE(brendan): Free resources and close SDL
	sdl2Close();

	return 0;
}
