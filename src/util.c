/*
 * Misc utility functions
 *
 */

#include "util.h"


/* print usage information */
void printUsage (char invokeName[]) {
    printf ("Usage: %s [options]\n\n", invokeName);
    puts ("-h, --help\t\tDisplay this information.");
    puts ("-i, --image [FILE]\tUse FILE as the image.");
    puts ("-d, --delay [INT]\tSet the number of iterations between progress images.");
    puts ("-l, --limit [INT]\tSet the maximum number of iterations.");
    puts ("-s, --silent\t\tNo output.");
    puts ("-v, --verbose\t\tVerbose output.");
    puts ("--no-output\t\tDisable progress images (overrides -d and --delay.");
}

/* compare two images, return the difference */
int imgCompare (Image *img1, Image *img2, int x0, int y0, int x1, int y1) {


    int img1Colour, img2Colour, count;
    img1Colour = img2Colour = 0;


    /* Bresenham's line algorithm */
    int deltax = x1 - x0;
    int deltay = y1 - y0;
    int run1, rise1, run2, rise2;
    int longest, shortest, numerator;

    run1 = run2 = (deltax < 0) ?  -1: 1;    // L or R?
    rise1 = (deltay < 0) ? -1 : 1;          // D or U?
    rise2 = 0;

    // iterate up whichever dimension changes the most
    // (ie while i < deltax if deltax > deltay and vice vera)
    longest = abs (deltax);
    shortest = abs (deltay);

    if (longest < shortest) {
        longest = abs (deltay);
        shortest = abs (deltax);

        rise2 = (deltay < 0) ? -1 : 1;
        run2 = 0;
    }

    // algorithm
    numerator = longest/2;
    for (count = 0; count <= longest; ++count) {
        img1Colour += getPixel (img1, x0, y0);
        img2Colour += getPixel (img2, x0, y0);
        numerator += shortest;
        if (numerator >= longest) {
            numerator -= longest;
            x0 += run1;
            y0 += rise1;
        }
        else {
            x0 += run2;
            y0 += rise2;
        }
    }

    /* return the difference between the average colours
        in the line */
    return abs ((img1Colour/count)-(img2Colour/count));
}

/* tests whether we should draw a line or not */
/*int testLine (SDL_Surface *img, int x0, int y0, int x1, int y1, Uint32 lineColour) {

    Uint32 imgColour = 0;
    int count;

    /* Bresenham's line algorithm *//*
    int deltax = x1 - x0;
    int deltay = y1 - y0;
    int run1, rise1, run2, rise2;
    int longest, shortest, numerator;

    run1 = run2 = (deltax < 0) ?  -1: 1;    // L or R?
    rise1 = (deltay < 0) ? -1 : 1;          // D or U?
    rise2 = 0;

    // iterate up whichever dimension changes the most
    // (ie while i < deltax if deltax > deltay and vice vera)
    longest = abs (deltax);
    shortest = abs (deltay);

    if (longest < shortest) {
        longest = abs (deltay);
        shortest = abs (deltax);

        rise2 = (deltay < 0) ? -1 : 1;
        run2 = 0;
    }

    // algorithm
    numerator = longest/2;
    for (count = 0; count <= longest; ++count) {
        imgColour += getPixel (img, x0, y0);
        numerator += shortest;
        if (numerator >= longest) {
            numerator -= longest;
            x0 += run1;
            y0 += rise1;
        }
        else {
            x0 += run2;
            y0 += rise2;
        }
    }
    return ((imgColour/count) - lineColour);
}*/

/* save image */
void saveImage (Image *img, char name[]) {
    IMG_SavePNG (img->surface, name);
}

/* draw pixel on a surface at x,y
    pixel format: 0xAABBGGRR */
void drawPixel (Image *img, int x, int y, Uint32 pixel) {
    if (x <= 0 || x >= IMGWIDTH || y <= 0 || y >= IMGHEIGHT)
        return;
    int bpp = img->surface->format->BytesPerPixel;
    /* get pixel address */
    #if MULTITHREADING
    pthread_mutex_lock (img->mutex + y * img->surface->pitch + x * bpp);
    #endif
    Uint8 *p = (Uint8 *)img->surface->pixels + y * img->surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
    #if MULTITHREADING
    pthread_mutex_unlock (img->mutex + y * img->surface->pitch + x * bpp);
    #endif
}

/* get pixel from a surface at x,y
    pixel format: 0xAABBGGRR */
Uint32 getPixel (Image *img, int x, int y) {
    if (x <= 0 || x >= IMGWIDTH || y <= 0 || y >= IMGHEIGHT)
        return 0;
    
    int bpp = img->surface->format->BytesPerPixel;
    /* get pixel address */
    Uint8 *p = (Uint8 *)img->surface->pixels + y * img->surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;
    }
}

/* draw a line using Bresenham's line algorithm */
void drawLine (Image *img, int x0, int y0, int x1, int y1, Uint32 c) {

    SDL_LockSurface (img->surface);

    int deltax = x1 - x0;
    int deltay = y1 - y0;
    int run1, rise1, run2, rise2;
    int longest, shortest, numerator;

    run1 = run2 = (deltax < 0) ?  -1: 1;    // L or R?
    rise1 = (deltay < 0) ? -1 : 1;          // D or U?
    rise2 = 0;

    // iterate up whichever dimension changes the most
    // (ie while i < deltax if deltax > deltay and vice vera)
    longest = abs (deltax);
    shortest = abs (deltay);

    if (longest < shortest) {
        longest = abs (deltay);
        shortest = abs (deltax);

        rise2 = (deltay < 0) ? -1 : 1;
        run2 = 0;
    }

    // algorithm
    numerator = longest/2;
    for (int i = 0; i <= longest; ++i) {
        drawPixel (img, x0, y0, c);
        numerator += shortest;
        if (numerator >= longest) {
            numerator -= longest;
            x0 += run1;
            y0 += rise1;
        }
        else {
            x0 += run2;
            y0 += rise2;
        }
    }

    SDL_UnlockSurface (img->surface);
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
