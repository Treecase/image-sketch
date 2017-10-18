/*
 * Functions for drawing shapes
 *
 */

#ifndef DRAWING_H
#define DRAWING_H

#include <SDL2/SDL.h>
#include "globals.h"
#include "threading.h"


void drawPixel (Image *image, int x, int y, Uint32 colour);
Uint32 getPixel (Image *image, int x, int y);


void drawLine (Image *image, int x0, int y0, int x1, int y1, Uint32 colour);
int testLine (Image *image1, Image *image2, int x0, int y0, int x1, int y1, Uint32 colour);


void drawCircle (Image *image, int x0, int y0, int radius, int dummy, Uint32 colour);
int testCircle (Image *image1, Image *image2, int x0, int y0, int radius, int dummy, Uint32 colour);

void drawEmptyCircle (Image *image, int x0, int y0, int radius, int dummy, Uint32 colour);
int testEmptyCircle (Image *image1, Image *image2, int x0, int y0, int radius, int dummy, Uint32 colour);


#endif