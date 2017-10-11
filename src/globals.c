/*
 * Define global variables
 *
 */


#include <SDL2/SDL.h>

#include "globals.h"


/* initialize global variables */
int initGlobals (int w, int h) {

    int failed = 0;

    extern int IMGWIDTH, IMGHEIGHT, PICDELAY;

    IMGWIDTH = w;
    IMGHEIGHT = h;
    PICDELAY = 500;

    return !failed;
}