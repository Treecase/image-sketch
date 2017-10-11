/*
 * Declare misc utility functions
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#ifndef UTIL_H
#define UTIL_H


int getImageColours (SDL_Surface *image, Uint32 *container);

int imgCompare (SDL_Surface *image1, SDL_Surface *image2, int x0, int y0, int x1, int y1);

void saveImage (SDL_Surface *image, char name[]);

void drawPixel (SDL_Surface *surface, int x, int y, Uint32 colour);

Uint32 getPixel (SDL_Surface *surface, int x, int y);

void drawLine (SDL_Surface *surface, int x0, int y0, int x1, int y1, Uint32 colour);

int initLibs();

void cleanup();


#endif
