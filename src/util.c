/*
 * Misc utility functions
 *
 */

#include "util.h"


/* generate random coords (ie x1 and y1) */
int genXY (int shape, int xy0, int limit) {
    switch (shape) {
    case SHAPES_LINE:
        return xy0 + (rand() % (1 + IMGWIDTH/(10 + rand() % limit)));

    case SHAPES_CIRCLE:
        return rand() % limit;
    }
}

/* print usage information */
void printUsage (char invokeName[]) {
    printf ("Usage: %s [options]\n\n", invokeName);
    puts ("-h, --help\t\tDisplay this information.");
    puts ("-i, --image [FILE]\tUse FILE as the image.");
    puts ("-d, --delay [INT]\tSet the number of iterations between progress images.");
    puts ("-l, --limit [INT]\tSet the maximum number of iterations.");
    puts ("-t, --threads [INT]\tSet the number of threads to use.");
    puts ("-s, --silent\t\tNo output.");
    puts ("-v, --verbose\t\tVerbose output.");
    puts ("--no-output\t\tDisable progress images (overrides -d and --delay.");
}

/* print a formatted string */
void printLog (int lvl, char *format, ...) {
    va_list ap;
    va_start (ap, format);
    (OUTPUTLEVEL >= lvl) ? vprintf (format, ap) : 0;
    va_end (ap);
}

/* save image */
void saveImage (Image *img, char name[]) {
    IMG_SavePNG (img->surface, name);
}

/* initialize libs */
int initLibs() {
    int value = 1;

    if (SDL_Init (SDL_INIT_VIDEO)) {
        SDL_Log ("SDL_Init failed: %s\n", SDL_GetError());
        value = 0;
    }
    if (!IMG_Init (IMG_INIT_PNG)) {
        SDL_Log ("IMG_Init failed: %s\n", IMG_GetError());
        value = 0;
    }
    return value;
}

/* destroy objects + shutdown libs */
void cleanup() {

    IMG_Quit();
    SDL_Quit();
}
