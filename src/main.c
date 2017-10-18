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
#include "threading.h"
#include "drawing.h"


int main (int argc, char *argv[]) {

    srand (time (NULL));

    SHAPE = SHAPES_LINE;

    extern void (*draw[])(Image *, int, int, int, int, Uint32);
    extern int (*compare[])(Image *, Image *, int, int, int, int, Uint32);

    draw[SHAPES_LINE] = drawLine;
    draw[SHAPES_CIRCLE] = drawCircle;
    draw[SHAPES_EMPTYCIRCLE] = drawEmptyCircle;

    compare[SHAPES_LINE] = testLine;
    compare[SHAPES_CIRCLE] = testCircle;
    compare[SHAPES_EMPTYCIRCLE] = testEmptyCircle;


    /* picdelay is # of iters between progress images
        outputlevel is level of verbosity:
        0 = silent, 1 = default, 2 = verbose

        progimgs is whether or not progress images
        should output */
    extern int IMGWIDTH, IMGHEIGHT, PICDELAY, PROGIMGS, MAXITERS,
                NUMTHREADS;
    NUMTHREADS = 1;

    // original image, working copy of image, image backbuffer
    extern Image ORIG, IMG1, IMG2;
    pthread_mutex_t *origMutex, *img1Mutex, *img2Mutex;
    SDL_Surface *origSurf, *img1Surf, *img2Surf;
    origSurf = img1Surf, img2Surf = NULL;

    ORIG.surface = origSurf, IMG1.surface = img1Surf, IMG2.surface = img2Surf;
    ORIG.mutex = origMutex, IMG1.mutex = img1Mutex, IMG2.mutex = img2Mutex;


    // skipped is the number of images skipped
    unsigned long iterations, imgCount, skipped, limit;
    int x0, y0, x1, y1;         // line coords

    Uint32 colour;              // line colour

    char *pathToImage = NULL;   // path to the image


    pthread_t *threads[NUMTHREADS];
    ThreadArgs tArgs[NUMTHREADS];


    PROGIMGS = 1;
    OUTPUTLEVEL = 1;
    MAXITERS = 10000;


    // command line args
    if (argc == 1)
        printf ("%s: WARNING! Using default input file 'img.png'!\n", argv[0]);
    for (int ac = 1; ac < argc; ++ac) {
        // print help info
        if (!strcmp (argv[ac], "-h") || !strcmp (argv[ac], "--help")) {
            printUsage (argv[0]);
            return 0;
        }
        // the path to the image
        else if (ac+1 < argc && !strcmp (argv[ac], "-i") || !strcmp (argv[ac], "--image")) {
            pathToImage = malloc (strlen (argv[++ac]));
            pathToImage = strcpy (pathToImage, argv[ac]);
        }
        // # of iterations between progress images
        else if (ac+1 < argc && !strcmp (argv[ac], "-d") || !strcmp (argv[ac], "--delay"))
            PICDELAY = atoi (argv[++ac]);

        // maximum iterations
        else if (ac+1 < argc && !strcmp (argv[ac], "-l") || !strcmp (argv[ac], "--limit"))
            MAXITERS = atoi (argv[++ac]);

        // number of threads to use
        else if (ac+1 < argc && !strcmp (argv[ac], "-t") || !strcmp (argv[ac], "--threads"))
            NUMTHREADS = atoi (argv[++ac]);

        // the shape to use
        else if (ac+1 < argc && !strcmp (argv[ac], "--shape"))
            SHAPE = atoi (argv[++ac]);

        // size limit of the shape
        else if (ac+1 < argc && !strcmp (argv[ac], "--size-limit"))
            limit = atoi (argv[++ac]);

        // no output
        else if (!strcmp (argv[ac], "-s") || !strcmp (argv[ac], "--silent"))
            OUTPUTLEVEL = SILENT;

        // verbose output
        else if (!strcmp (argv[ac], "-v") || !strcmp (argv[ac], "--verbose"))
            OUTPUTLEVEL = VERBOSE;

        else if (!strcmp (argv[ac], "--no-output"))
            PROGIMGS = OUTPUTLEVEL = 0;

        else
            return printf ("%s: invalid option -- '%s'. Try -h or --help for more information.\n", argv[0], argv[ac]);
    }

    PICDELAY = (PICDELAY > 0) ? PICDELAY : 1 + (MAXITERS * 0.1);

    // init libs
    if (!initLibs())
        return puts ("initLibs failed!");

    // load images
    printLog (VERBOSE, "opening images\n");
    if (pathToImage == NULL)
        pathToImage = "img.png";
    ORIG.surface = IMG_Load (pathToImage);
    if (!ORIG.surface) {
        return printf ("IMG_Load: failed to load %s: %s\n", pathToImage, IMG_GetError());
    }
    IMGWIDTH = ORIG.surface->w;
    IMGHEIGHT = ORIG.surface->h;
    printLog (DEFAULT, "w: %i h: %i\n", IMGWIDTH, IMGHEIGHT);

    printLog (VERBOSE, "creating buffers\n");
    // create the working and backbuffer images
    IMG1.surface = IMG_Load (pathToImage);
    IMG2.surface = IMG_Load (pathToImage);
    // clear the images
    SDL_LockSurface (IMG1.surface);
    SDL_LockSurface (IMG2.surface);
    memset (IMG1.surface->pixels, getPixel (&ORIG, 0, 0), ORIG.surface->h*ORIG.surface->pitch);
    memset (IMG2.surface->pixels, getPixel (&ORIG, 0, 0), ORIG.surface->h*ORIG.surface->pitch);
    SDL_UnlockSurface (IMG1.surface);
    SDL_UnlockSurface (IMG2.surface);



    /* USING MULTITHREADING */
    if (NUMTHREADS > 1) {

        // create image mutexes
        printLog (VERBOSE, "creating mutexes\n");
        ORIG.mutex = malloc (sizeof(pthread_mutex_t)*ORIG.surface->h*ORIG.surface->pitch);
        IMG1.mutex = malloc (sizeof(pthread_mutex_t)*IMG1.surface->h*IMG1.surface->pitch);
        IMG2.mutex = malloc (sizeof(pthread_mutex_t)*IMG2.surface->h*IMG2.surface->pitch);

        pthread_mutex_init (ORIG.mutex, NULL);
        pthread_mutex_init (IMG1.mutex, NULL);
        pthread_mutex_init (IMG2.mutex, NULL);

        // create threads
        printLog (VERBOSE, "creating threads\n");
        for (int i = 0; i < NUMTHREADS; ++i) {
            threads[i] = malloc (sizeof(pthread_t));

            tArgs[i].tID = i+1;
            tArgs[i].minx = i*(IMGWIDTH / (NUMTHREADS/2));
            tArgs[i].miny = i*(IMGHEIGHT / (NUMTHREADS/2));
            tArgs[i].maxx = (i+1)*(IMGWIDTH / (NUMTHREADS/2));
            tArgs[i].maxy = (i+1)*(IMGHEIGHT / (NUMTHREADS/2));
            pthread_create (threads[i], NULL, handleBlock, &tArgs[i]);
        }



        printLog (DEFAULT, "MAIN: ENTERING LOOP!\n");
        for (int i = 0; i < NUMTHREADS; ++i) {
            pthread_join (*threads[i], NULL);
            iterations += tArgs[i].retVal;
        }

        // deallocate threads when they are finished working
        for (int i = 0; NUMTHREADS > 1 && i < NUMTHREADS; ++i) {
            free (threads[i]);
        }

        // deallocate mutexes
        pthread_mutex_destroy (ORIG.mutex);
        pthread_mutex_destroy (IMG1.mutex);
        pthread_mutex_destroy (IMG2.mutex);

    }
    /* NOT USING MULTITHREADING */
    else {
        printLog (DEFAULT, "NOT USING MULTITHREADING\n");
        // main loop
        int quit = 0;
        limit = (limit) ? limit : 100;
        SDL_Event e;
        iterations = imgCount = 0;
        while (iterations < MAXITERS && !quit) {
            // handle events
            while (SDL_PollEvent (&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                    quit = 1;
                    break;
                }
            }
            printLog (VERBOSE, "iteration #%i\n", iterations);

            // create a random line
            x0 = rand() % IMGWIDTH, y0 = rand() % IMGHEIGHT;
            x1 = genXY (SHAPE, x0, limit);
            y1 = genXY (SHAPE, y0, limit);
            printLog (VERBOSE, "x0 %i, y0 %i,  x1 %i, y1 %i\n", x0, y0, x1, y1);
            colour = getPixel (&ORIG, x0, y0);  // midpoint = (n1+n2)/2

            printLog (VERBOSE, "Comparing imgs\n");

            /* check if the line is a good change
                and trash it if it isn't */
            if (compare[SHAPE] (&ORIG, &IMG1, x0, y0, x1, y1, colour)
              < compare[SHAPE] (&ORIG, &IMG2, x0, y0, x1, y1, 0))
                // draw the line
                draw[SHAPE] (&IMG1, x0, y0, x1, y1, colour);
            else {
                printLog (VERBOSE, "Skipped %i\n", ++skipped);
                continue;
            }
            SDL_BlitSurface (IMG1.surface, NULL, IMG2.surface, NULL);

            // save progress image if necessary
            if (PROGIMGS && iterations % PICDELAY == 0) {
                char outname[50];
                sprintf (outname, "pics/%lu.png", imgCount++);
                saveImage (&IMG1, outname);
                printLog (VERBOSE, "saved image pics/%lu.png\n", imgCount);
            }
            if ((limit >> 1) > 1 && iterations > 0 && iterations % (MAXITERS/5) == 0) {
                limit >>= 1;
            }
            iterations++;
        }
    }

    // save final image
    char outname[50];
    if (PROGIMGS) {
        sprintf (outname, "pics/%lu.png", imgCount);
        saveImage (&IMG1, outname);
    }
    sprintf (outname, "%lu.png", iterations);
    saveImage (&IMG1, outname);
    printLog (DEFAULT, "\nDone in %i iterations\n", iterations);
    printLog (DEFAULT, "Skipped %i total\n", skipped);

    // free the surfaces
    SDL_FreeSurface (ORIG.surface);
    ORIG.surface = NULL;
    SDL_FreeSurface (IMG1.surface);
    IMG1.surface = NULL;
    SDL_FreeSurface (IMG2.surface);
    IMG2.surface = NULL;

    // shutdown libs
    cleanup();

    return 0;
}
