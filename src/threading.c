/*
 * Multithreading support
 *
 */

#include "threading.h"


/* a function to operate on a certain section of the image
    from (minx, miny) to (maxx, maxy) */
void *handleBlock (void *argStruct) {

    unsigned long tID;
    int minx, miny, maxx, maxy;

    ThreadArgs *args = (ThreadArgs *)argStruct;

    tID = args->tID;
    minx = args->minx;
    miny = args->miny;
    maxx = args->maxx;
    maxy = args->maxy;

    printLog (2, "%p: args are: %i %i %i %i\n", tID, minx, miny, maxx, maxy);

    unsigned long iterations, skipped;
    int x0, y0, x1, y1;
    Uint32 colour;

    printLog (1, "%p: Entering main loop\n", tID);
    // main loop
    iterations = skipped = 0;
    while (iterations < MAXITERS) {

        printLog (2, "%p: iteration #%i\n", tID, iterations);

        // create a random line
        x0 = minx + (rand() % maxx), y0 = miny + (rand() % maxy);
            x1 = x0 + rand() % maxx;
            y1 = y0 + rand() % maxy;

        printLog (2, "%p: x0 %i, y0 %i,  x1 %i, y1 %i\n", tID, x0, y0, x1, y1);
        colour = getPixel (&ORIG, x0, y0);

        /* check if the line is a good change
            and trash it if it isn't */
        if (compare[SHAPE] (&ORIG, &IMG1, x0, y0, x1, y1, colour)
          < compare[SHAPE] (&ORIG, &IMG2, x0, y0, x1, y1, 0)) {
            // draw the line
            printLog (2, "%p: Drawing line\n", tID);
            draw[SHAPE] (&IMG1, x0, y0, x1, y1, colour);
        }
        else {
            printLog (2, "%p: Skipped %i\n", tID, ++skipped);
            continue;
        }
        blitArea (&IMG1, &IMG2, minx, miny, maxx, maxy);

        /*// save progress image if necessary
        if (PROGIMGS && iterations % PICDELAY == 0) {
            char outname[50];
            sprintf (outname, "pics/%lu.png", imgCount++);
            saveImage (image1, outname);
            printLog (2, "saved image pics/%lu.png\n", imgCount);
        }*/
        iterations++;
    }
    printLog (1, "%p: FINISHED (did %lu iterations)\n", tID, iterations);
    ((ThreadArgs *)argStruct)->retVal = iterations;
    pthread_exit (NULL);
}

/* copy an area of img1 to img2 */
void blitArea (Image *img1, Image *img2, int minx, int miny, int maxx, int maxy) {

    for (int x = minx; x < maxx; ++x)
        for (int y = miny; y < miny; ++y)
            drawPixel (img2, x, y, getPixel (img1, x, y));

}
