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
#include <SDL2/SDL_image.h>
#include <time.h>

// NOTE(brendan): The window we'll be rendering to
global_variable SDL_Window* gWindow = NULL;

// NOTE(brendan): Screen dimension constants
internal const int SCREEN_WIDTH = 1024;
internal const int SCREEN_HEIGHT = 768;

internal char *imageNames[] = {(char *)"../misc/north_york_test.png", 
                               (char *)"../misc/BTaxi.bmp",
                               (char *)"../misc/RTaxi.bmp",
                               (char *)"../misc/SPerson.bmp",
                               (char *)"../misc/NPerson.bmp"};
internal Dimensions imageDims[] = {{0, 0}, {30, 40}, {30, 40}, {30, 40}, 
                                   {44, 30}};

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
        gWindow = SDL_CreateWindow("Ridesharing Taxicab Scheduler", 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   SCREEN_WIDTH, SCREEN_HEIGHT, 
                                   SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL Error: %s\n", 
                   SDL_GetError());
            success = false;
        }
        else {
            // NOTE(brendan): initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
                printf("SDL image could not be initialized! SDL_image error:"
                       " %s\n", IMG_GetError());
                success = false;
            }
            else {
                // NOTE(brendan): Create vsynced renderer for window
                *renderer = SDL_CreateRenderer(gWindow, -1, 
                                               SDL_RENDERER_ACCELERATED | 
                                               SDL_RENDERER_PRESENTVSYNC);
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
    if (loadedSurface == 0) {
        loadedSurface = IMG_Load(fileName);
    }
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
            SDL_SetColorKey(scaledSurface, SDL_TRUE, 
                             SDL_MapRGB(scaledSurface->format, 
                                        0xFF, 0xFF, 0xFF));

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

inline bool
sdl2LoadTextureFromFile(SDL_Texture **texture, SDL_Renderer *renderer, 
                        char *filename)
{
    // NOTE(brendan): Load grid texture
    // TODO(brendan): remove; testing
    *texture = sdl2LoadTexture(filename, renderer);
    if (*texture == 0) {
        printf( "Failed to load texture at %s!\n", filename);
        return false;
    }
    return true;
}

internal bool 
sdl2LoadMedia(TaxiState *taxiState) 
{
    // NOTE(brendan): Loading success flag
    bool success = true;
    for (int imageIndex = 0;
         success && (imageIndex < NUMBER_OF_IMAGES);
         ++imageIndex) {
        success = 
            sdl2LoadTextureFromFile(&taxiState->images[imageIndex].texture,
                                    taxiState->renderer,
                                    imageNames[imageIndex]);
        taxiState->images[imageIndex].width = imageDims[imageIndex].width;
        taxiState->images[imageIndex].height = imageDims[imageIndex].height;
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

// NOTE(brendan): free resources and quit SDL
void sdl2Close() 
{
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // NOTE(brendan): Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char *argv[]) 
{
    // NOTE(brendan): state (model) of the taxis on the road network
    TaxiState taxiState = {};
    taxiState.screenWidth = SCREEN_WIDTH;
    taxiState.screenHeight = SCREEN_HEIGHT;

    if (!sdl2Init(&taxiState.renderer)) {
        printf("Failed to initialize!\n");
    }
    else if (!sdl2LoadMedia(&taxiState)) {
        printf("Failed to load media!\n");
    }
    else {
        // NOTE(brendan): Main loop flag
        bool globalRunning = true;

        srand((unsigned)time(0));

        int previousTime = SDL_GetTicks(), elapsedTime = 0;
        // NOTE(brendan): while application is running
        while (globalRunning) {
            elapsedTime = SDL_GetTicks() - previousTime;
            previousTime = SDL_GetTicks();
            globalRunning = sdl2HandleEvents();
            updateAndRender(&taxiState, elapsedTime);
        }
    }

    // NOTE(brendan): Free resources and close SDL
    sdl2Close();

    return 0;
}
