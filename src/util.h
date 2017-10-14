/*
 * Declare misc utility functions
 *
 */

#ifndef UTIL_H
#define UTIL_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "globals.h"
#include "threading.h"

void printUsage (char invokeName[]);

int getImageColours (Image *image, Uint32 *container);

int imgCompare (Image *image1, Image *image2, int x0, int y0, int x1, int y1);

int testLine (Image *image, int x0, int y0, int x1, int y1, Uint32 lineColour);

void saveImage (Image *image, char name[]);

void drawPixel (Image *image, int x, int y, Uint32 colour);

Uint32 getPixel (Image *image, int x, int y);

void drawLine (Image *image, int x0, int y0, int x1, int y1, Uint32 colour);

int initLibs();

void cleanup();


#endif
