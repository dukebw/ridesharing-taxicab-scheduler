/* ========================================================================
File: sdl2_taxicab.cpp
Date: Mar. 19/15
Revision: 1
Creator: Brendan Duke
Notice: (C) Copyright 2015 by ADK Inc. All Rights Reserved.
======================================================================== */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_Render.h> 
//NOTE(Jaeden): I read render can be used to draw a line (and points) without an external library?

// NOTE(brendan): to change static so that we can distinguish between
// global variables etc. then we can e.g. search and fix global vars
#define internal static
#define local_persist static
#define global_variable static

// NOTE(brendan): The window we'll be rendering to
global_variable SDL_Window* gWindow = NULL;
// NOTE(brendan): The window renderer
global_variable SDL_Renderer *gRenderer = NULL;
global_variable SDL_Texture *gGridTexture = NULL;

//NOTE(Jaeden): Scene texture
// global_variable LTexture gDotTexture

// NOTE(brendan): Screen dimension constants
internal const int SCREEN_WIDTH = 640;
internal const int SCREEN_HEIGHT = 480;

// NOTE(brendan): does the necessary initialization of SDL2 at program startup
internal bool 
sdl2Init() {
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
      gRenderer = SDL_CreateRenderer(gWindow, -1, 
          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", 
            SDL_GetError());
        success = false;
      }
      else {
        // NOTE(brendan): Initialize renderer color
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      }
    }
  }

  return success;
}
//NOTE(Jaeden): dot class
class Dot
{
	public:
	//Dimensions of dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	
	//velcity of the dot
	//static const int DOT_VEL = 600;
	
	//Initialize
	Dot();
	
	//event handler called here
	//void handleEvent( SDL_Event& e);
	
	//dot move (hopefully along line)
	//void move(float timeStep);
	
	//show dot
	void render();
/*	
	private:
	float mPosX, mPosY;
	float mVelX, mVelY;
*/	
}
//NOTE(Jaeden): Timer for movement
/*
class DotTimer
{
	public:
		dotTimer();
		
		void start();
		void stop();
		void pause();
		void unpause();
		
		//timer's time
		Uint32 getTicks();
		
	private:
		Uint mStartTicks;
		Uint mPausedTicks;
		
		
}

*/
internal SDL_Texture * 
loadTexture(char *fileName) {
  // NOTE(Zach): The final optimized image
  SDL_Texture *newTexture = NULL;

  // NOTE(Zach): Load image at specified path
  SDL_Surface *loadedSurface = SDL_LoadBMP(fileName); //use a c string
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
      newTexture = SDL_CreateTextureFromSurface(gRenderer, scaledSurface);
      if (newTexture == NULL) {
        printf("Unable to create texture from %s! SDL Error: %s\n", 
            fileName,SDL_GetError());
      } 

      // NOTE(Zach): Get rid of old loaded surface
      SDL_FreeSurface(scaledSurface);
    }
  }
  return newTexture;
}

internal bool 
sdl2LoadMedia() {
  // NOTE(brendan): Loading success flag
  bool success = true;

  // NOTE(brendan): Load grid texture
  // TODO(brendan): remove; testing
  gGridTexture = loadTexture((char *)"../misc/simple_grid.bmp");
  if (gGridTexture == 0) {
    printf( "Failed to load grid texture!\n" );
    success = false;
  }

  return success;
}

// NOTE(brendan): handle events, e.g. mouse clicks, key presses etc.
// returns false if quit button is pressed; true otherwise.
internal bool
sdl2HandleEvents() {
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
sdl2Render() {
 //Note(Jaeden): timeStep
 /* 
 float timeStep = stepTimer.getTicks()/1000.0f;
 dot.move(timeStep);
 stepTimer.start();
 
 */
 
  // NOTE(brendan): Clear screen
  // TODO(brendan): need to set draw colour here?
  SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(gRenderer);
  SDL_RenderCopy(gRenderer, gGridTexture, 0, 0);
  SDL_RenderPresent(gRenderer);
}
//NOTE(Jaeden): dot movement
/*
DotTimer:: DotTimer()
{
	mStartTicks = 0;
	mPausedTicks = 0;
	
	mPaused = false;
	mStarted = false;
}
void DotTimer::start()
{
	mStarted = true;
	mPaused = false;
	
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}
void DotTimer::stop()
{
	mStartTicks = 0;
	mPausedTicks = 0;
	
	mStarted = false;
	mPaused = false;
	
}
void DotTimer::pause()
{
	if (mStarted && !mPaused)
	{
		mPause = true;
		
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}
void DotTimer::unpause()
{
	if( mStarted && mPaused)
	{ mPaused = false; mStartTicks = SDL_GetTicks() - mPausedTicks; mPaused Ticks = 0; }
}
Uint32 DotTimer::getTicks()
{
	Uint32 timer = 0;
	
	if (mStarted)
	{
		if (mPaused)
		{ time = mPausedTicks;}
		else
		{ time = SDL_GetTicks() - mStartTicks; }
		
	}
	return time;
}
void DOT::move( float timeStep)
{
	//Move left or right
	mPosX += mVelX * timeStep;
	
	//too far left or right
	if( mPosX < 0)
	{ mPosX = 0;}
	else if ( mPosX > SCREEN_WIDTH - DOT_WIDTH )
	{ mPosX = SCREENWIDTH - DOT_WIDTH;}
	
	//up or down
	mPosY += mVelY * timeStep;
	
	//too far up or down
	if ( mPosY < 0)
	{ mPosY = 0;}
	else if ( mPosY > SCREEN_HEIGHT - DOT_HEIGHT )
	{ mPosY = SCREEN_HEIGHT - DOT_HEIGHT; }
}
*/
//NOTE(Jaeden): the dot
Dot::Dot()
{
	mPosX = 0; mPosY = 0;
	mVelX = 0; mVelY = 0;
}
//NOTE(Jaeden): dot event handling
/*
void Dot::handleEvent( SDL_Event& e)
{
	I haven't figured out how to read the pixel locations of a line an have the dot follow it
}




*/
//NOTE(Jaeden): draw dot
void Dot:: render()
{
	//(int)mPosX, (int)mPosY;
	gDotTexture.render( 300, 300 );
	
}
//NOTE(Jaeden): set pixel, I also read that there's an alt: RenderDraw (funct)?
void setPixel( SDL_Surface* surface, int X, int Y, Uint32 Color)
{
	Uint8* pixel = (Uint8*) surface->pixels;
	pixel += (Y * surface->pitch)+(X*BytesPerPixel);
	*((Uint32*)pixel) = Color;
}

//NOTE(Jaeden): draw line
void Line( float x1, float y1, float x2, float y2, const Color& color)
{
	//line alg
	const bool steep = (fabs(y2-y1) > fabs(x2-x1));
	if (steep)
	{ std::swap(x1,y1); std::swap(x2,y2); }
	if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {SetPixel(y,x, color); }
        else
        { SetPixel(x,y, color);   }

        error -= dy;
        if(error < 0)
        { y += ystep;  error += dx;  }
    }
	
}

bool loadMedia()
{
	if (!gDotTexture.loadFromFile("dot.bmp"))
	{printf("Failed to load dot texture! \n "); return false;}
	
	return true;
}
// NOTE(brendan): free resources and quit SDL
void sdl2Close() {
	// NOTE(brendan): Free loaded images
  SDL_DestroyTexture(gGridTexture);
  gGridTexture = NULL;
  
  //NOTE(Jaeden) : free dot texture
  gDotTexture.free();

	// NOTE(brendan): Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// NOTE(brendan): Quit SDL subsystems
	SDL_Quit();
}


int main(int argc, char *argv[]) {
  if (!sdl2Init()) {
    printf("Failed to initialize!\n");
  }
  else if (!sdl2LoadMedia()) {
    printf("Failed to load media!\n");
  }
  else {
    // NOTE(brendan): Main loop flag
    bool globalRunning = true;

    // NOTE(brendan): while application is running
    while (globalRunning) {
      globalRunning = sdl2HandleEvents();
      //NOTE(Jaeden): dot class
      Dot dot;
      dot.render();
      sdl2Render();
    }
  }

	// NOTE(brendan): Free resources and close SDL
	sdl2Close();

	return 0;
}
