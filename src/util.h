/*
 * Declare misc utility functions
 *
 */

#ifndef UTIL_H
#define UTIL_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdarg.h>

#include "globals.h"
#include "threading.h"


int genXY (int shape, int xy0, int limit);

void printLog (int outputlvl, char *format, ...);

void printUsage (char invokeName[]);

void saveImage (Image *image, char name[]);

int initLibs();

void cleanup();


#endif
