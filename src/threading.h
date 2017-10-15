/*
 * Multithreading support
 *
 */

#ifndef THREADING_H
#define THREADING_H


#include <pthread.h>
#include <SDL2/SDL.h>

#include "globals.h"
#include "util.h"
#include "drawing.h"


typedef struct ThreadArgs {
    unsigned long tID;
    unsigned long retVal;
    int minx, miny, maxx, maxy;
} ThreadArgs;


void *handleBlock (void *args);

void blitArea (Image *img1, Image *img2, int minx, int miny, int maxx, int maxy);


#endif