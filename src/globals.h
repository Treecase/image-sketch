/*
 * Declare global variables
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#define MULTITHREADING  0   /* enable multithreading 0 = off, 1 = on */


#include <SDL2/SDL.h>


typedef struct Image {
    SDL_Surface *surface;
    #if MULTITHREADING
    pthread_mutex_t *mutex;
    #endif
} Image;


int IMGWIDTH, IMGHEIGHT, PICDELAY, OUTPUTLEVEL, PROGIMGS, MAXITERS;

Image ORIG, IMG1, IMG2;


#endif