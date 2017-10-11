/*
 * Re-create an image with lines
 *
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <stdlib.h>

#include "globals.h"
#include "util.h"


int main (int argc, char *argv[]) {

    srand (time (NULL));

    unsigned long int iterations, imgCount;
    int x0, y0, x1, y1;

    Uint32 colour;

    char *pathToImage;

    SDL_Surface *imageOrig;
    SDL_Surface *image1;
    SDL_Surface *image2;


    // command line args
    if (argc > 1) {
        pathToImage = malloc (strlen (argv[1]));
        memcpy (pathToImage, argv[1], strlen (argv[1]));
    }
    else
        pathToImage = "img.png";

    // init libs
    if (!initLibs())
        puts ("initLibs failed!");

    // load images
    imageOrig = IMG_Load (pathToImage);
    if (!imageOrig) {
        printf ("IMG_Load: failed to load %s: %s\n", pathToImage, IMG_GetError());
        imageOrig = IMG_Load ("img.png");
        PICDELAY = atoi (argv[1]);
    }

    image1 = IMG_Load (pathToImage);
    image2 = IMG_Load (pathToImage);
    SDL_LockSurface (image1);
    SDL_LockSurface (image2);
    memset (image1->pixels, 0xFFFFFFFF, imageOrig->h*imageOrig->pitch);
    memset (image2->pixels, 0xFFFFFFFF, imageOrig->h*imageOrig->pitch);
    SDL_UnlockSurface (image1);
    SDL_UnlockSurface (image2);

    // init globals
    if (!initGlobals (imageOrig->w, imageOrig->h))
        SDL_Log ("Global init failed!\n");

    iterations = imgCount = 0;
    SDL_Event e;
    int quit = 0;
    // main loop
    while (iterations < 250000 && !quit) {
        while (SDL_PollEvent (&e)) {
            switch (e.type) {
                // quit event
                case SDL_QUIT:
                quit = 1;
                break;
            }
        }

        // create a random line
        x0 = rand() % IMGWIDTH, y0 = rand() % IMGHEIGHT;
        x1 = x0 + (rand() % (1 + IMGWIDTH/(10 + rand() % 50)));
        y1 = y0 + (rand() % (1 + IMGHEIGHT/(10 + rand() % 50)));
        colour = getPixel (imageOrig, x0, y0);

        // draw the line
        drawLine (image1, x0, y0, x1, y1, colour);

        // check if the line is a good change
        int diff_o2 = imgCompare (image2, imageOrig, x0, y0, x1, y1);
        int diff_o1 = imgCompare (image1, imageOrig, x0, y0, x1, y1);

        if (diff_o2 < diff_o1) {
            SDL_BlitSurface (image2, NULL, image1, NULL);
            continue;
        }
        SDL_BlitSurface (image1, NULL, image2, NULL);

        // rendering
        if (iterations % PICDELAY == 0) {
            char outname[50];
            sprintf (outname, "pics/%lu.png", imgCount++);
            saveImage (image1, outname);
        }
        iterations++;
    }

    // save image
    char outname[50];
    sprintf (outname, "pics/%lu.png", imgCount);
    saveImage (image1, outname);
    printf ("Done in %i iterations\n", iterations);

    SDL_FreeSurface (imageOrig);
    imageOrig = NULL;
    SDL_FreeSurface (image1);
    image1 = NULL;
    SDL_FreeSurface (image2);
    image2 = NULL;

    cleanup();

    return 0;
}
