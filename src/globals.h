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


int IMGWIDTH, IMGHEIGHT, PICDELAY, OUTPUTLEVEL, PROGIMGS, MAXITERS;

Image ORIG, IMG1, IMG2;


#endif