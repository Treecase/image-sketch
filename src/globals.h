/*
 * Declare global variables
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H


#include <SDL2/SDL.h>


typedef struct Image {
    SDL_Surface *surface;
    pthread_mutex_t *mutex;
} Image;

enum Shapes {
    SHAPES_LINE,
    SHAPES_CIRCLE,
    SHAPES_EMPTYCIRCLE,
    SHAPES_LENGTH
} SHAPE;

enum LoggingLevels {
    SILENT,
    DEFAULT,
    VERBOSE
} OUTPUTLEVEL;


int IMGWIDTH, IMGHEIGHT, PICDELAY, PROGIMGS, MAXITERS, NUMTHREADS;

void (*draw[SHAPES_LENGTH])(Image *, int, int, int, int, Uint32);
int (*compare[SHAPES_LENGTH])(Image *, Image *, int, int, int, int, Uint32);

Image ORIG, IMG1, IMG2;


#endif
