/*
 * Multithreading support
 *
 */

#include "threading.h"


/* a function to operate on a certain section of the image
    from (minx, miny) to (maxx, maxy) */
void *handleBlock (void *argStruct) {

    unsigned long int tID = pthread_self();

    int minx, miny, maxx, maxy;

    (OUTPUTLEVEL > 1) ? printf ("%p: assigning args\n", tID) : 0;
    ThreadArgs *args = (ThreadArgs *)argStruct;

    minx = (int)args->minx;
    miny = (int)args->miny;
    maxx = (int)args->maxx;
    maxy = (int)args->maxy;

    printf ("%p: args are: %i %i %i %i\n", tID, minx, miny, maxx, maxy);

    int iterations, skipped;
    int x0, y0, x1, y1;
    Uint32 colour;

    (OUTPUTLEVEL) ? printf ("%p: Entering main loop\n", tID) : 0;
    // main loop
    iterations = skipped = 0;
    while (iterations < MAXITERS) {

        (OUTPUTLEVEL > 1) ? printf ("%p: iteration #%i\n", tID, iterations) : 0;

        // create a random line
        x0 = minx + (rand() % maxx), y0 = miny + (rand() % maxy);
            x1 = x0 + rand() % maxx;
            y1 = y0 + rand() % maxy;

        (OUTPUTLEVEL > 1) ? printf ("%p: x0 %i, y0 %i,  x1 %i, y1 %i\n", tID, x0, y0, x1, y1) : 0;
        colour = getPixel (&ORIG, x0, y0);

        /* check if the line is a good change
            and trash it if it isn't */
        if (imgCompare (&ORIG, &IMG1, x0, y0, x1, y1)
         <= imgCompare (&ORIG, &IMG2, x0, y0, x1, y1)) {
            // draw the line
            (OUTPUTLEVEL > 1) ? printf ("%p: Drawing line\n", tID) : 0;
            drawLine (&IMG1, x0, y0, x1, y1, colour);
        }
        else {
            (OUTPUTLEVEL > 1) ? printf ("%p: Skipped %i\n", tID, ++skipped) : 0;
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
        iterations++;
    }
    pthread_exit (&iterations);
}

/* copy an area of img1 to img2 */
void blitArea (Image *img1, Image *img2, int minx, int miny, int maxx, int maxy) {

    for (int x = minx; x < maxx; ++x)
        for (int y = miny; y < miny; ++y)
            drawPixel (img2, x, y, getPixel (img1, x, y));

}
