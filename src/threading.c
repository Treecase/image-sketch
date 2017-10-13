#if MULTITHREADING
/*
 * Multithreading support
 *
 */

#include "threading.h"


/* a function to operate on a certain section of the image
    from (minx, miny) to (maxx, maxy) */
void *handleBlock (void *argStruct) {

    puts ("assigning args");
    ThreadArgs *args = argStruct;

    int minx, miny, maxx, maxy;

    puts ("assigning vars");
    minx = args->minx;
    miny = args->miny;
    maxx = args->maxx;
    maxy = args->maxy;

    int *iterations, skipped;
    int x0, y0, x1, y1;
    Uint32 colour;

    (OUTPUTLEVEL) ? puts ("Entering main loop") : 0;
    // main loop
    skipped = 0;
    SDL_Event e;
    int quit = 0;
    puts ("while");
    while (*iterations < MAXITERS && !quit) {
        // handle events
        while (SDL_PollEvent (&e)) {
            switch (e.type) {
                case SDL_QUIT:
                quit = 1;
                break;
            }
        }
        (OUTPUTLEVEL > 1) ? printf ("iteration #%i\n", *iterations) : 0;

        // create a random line
        x0 = minx + rand() % maxx, y0 = miny + rand() % maxy;
        while (x1 <= maxx)
            x1 = x0 + (rand() % (1 + IMGWIDTH/(10 + rand() % 50)));
        while (y1 <= maxy)
            y1 = y0 + (rand() % (1 + IMGHEIGHT/(10 + rand() % 50)));

        (OUTPUTLEVEL > 1) ? printf ("x0 %i, y0 %i,  x1 %i, y1 %i\n", x0, y0, x1, y1) : 0;
        colour = getPixel (&ORIG, x0, y0);

        /* check if the line is a good change
            and trash it if it isn't */
        if (imgCompare (&ORIG, &IMG1, x0, y0, x1, y1)
         <= imgCompare (&ORIG, &IMG2, x0, y0, x1, y1)) {
            // draw the line
            drawLine (&IMG1, x0, y0, x1, y1, colour);
        }
        else {
            (OUTPUTLEVEL > 1) ? printf ("Skipped %i\n", ++skipped) : 0;
            continue;
        }
        blitArea (&IMG1, &IMG2, minx, miny, maxx, maxy);

        /*// save progress image if necessary
        if (PROGIMGS && iterations % PICDELAY == 0) {
            char outname[50];
            sprintf (outname, "pics/%lu.png", imgCount++);
            saveImage (image1, outname);
            (OUTPUTLEVEL > 1) ? printf ("saved image pics/%lu.png\n", imgCount) : 0;
        }*/
        *iterations++;
    }
    pthread_exit (iterations);
}

/* copy an area of img1 to img2 */
void blitArea (Image *img1, Image *img2, int minx, int miny, int maxx, int maxy) {

    for (int x = minx; x < maxx; ++x)
        for (int y = miny; y < miny; ++y)
            drawPixel (img2, x, y, getPixel (img1, x, y));

}

#endif