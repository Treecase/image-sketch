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


#if MULTITHREADING
#include "threading.h"
#include <pthread.h>
#endif

int main (int argc, char *argv[]) {

    srand (time (NULL));

    /* picdelay is # of iters between progress images
        outputlevel is level of verbosity:
        0 = silent, 1 = default, 2 = verbose

        progimgs is whether or not progress images
        should output */
    extern int IMGWIDTH, IMGHEIGHT, PICDELAY, OUTPUTLEVEL,
                PROGIMGS, MAXITERS;

    // original image, working copy of image, image backbuffer
    extern Image ORIG, IMG1, IMG2;
    #if MULTITHREADING
    pthread_mutex_t *origMutex, *img1Mutex, *img2Mutex;
    #endif
    SDL_Surface *origSurf, *img1Surf, *img2Surf;
    origSurf = img1Surf, img2Surf = NULL;

    ORIG.surface = origSurf, IMG1.surface = img1Surf, IMG2.surface = img2Surf;
    #if MULTITHREADING
    ORIG.mutex = origMutex, IMG1.mutex = img1Mutex, IMG2.mutex = img2Mutex;
    #endif


    // skipped is the number of images skipped
    unsigned long int iterations, imgCount, skipped;
    int x0, y0, x1, y1;         // line coords

    Uint32 colour;              // line colour

    char *pathToImage = NULL;   // path to the image


    #if MULTITHREADING
    int numThreads = 1;
    pthread_t *threads[numThreads];
    ThreadArgs *tArgs[numThreads];
    #endif


    PROGIMGS = 0;
    OUTPUTLEVEL = 2;
    PICDELAY = -1;
    MAXITERS = 10000;


    // command line args
    (OUTPUTLEVEL > 1) ? puts ("handling args") : 0;
    if (argc == 1)
        printf ("%s: WARNING! Using default input file 'img.png'!\n", argv[0]);
    for (int ac = 1; ac < argc; ++ac) {
        if (!strcmp (argv[ac], "-h") || !strcmp (argv[ac], "--help")) {
            printUsage (argv[0]);
            return 0;
        }
        else if (ac+1 < argc && !strcmp (argv[ac], "-i") || !strcmp (argv[ac], "--image")) {
            pathToImage = malloc (strlen (argv[++ac]));
            pathToImage = strcpy (pathToImage, argv[ac]);
        }
        else if (ac+1 < argc && !strcmp (argv[ac], "-d") || !strcmp (argv[ac], "--delay"))
            PICDELAY = atoi (argv[++ac]);

        else if (ac+1 < argc && !strcmp (argv[ac], "-l") || !strcmp (argv[ac], "--limit"))
            MAXITERS = atoi (argv[++ac]);

        else if (!strcmp (argv[ac], "-s") || !strcmp (argv[ac], "--silent"))
            OUTPUTLEVEL = 0;

        else if (!strcmp (argv[ac], "-v") || !strcmp (argv[ac], "--verbose"))
            OUTPUTLEVEL = 2;

        else if (!strcmp (argv[ac], "--no-output"))
            PROGIMGS = OUTPUTLEVEL = 0;

        else
            return printf ("%s: invalid option -- '%s'. Try -h or --help for more information.\n", argv[0], argv[ac]);
    }

    PICDELAY = (PICDELAY > 0) ? PICDELAY : 1 + (MAXITERS * 0.2);

    // init libs
    if (!initLibs())
        return puts ("initLibs failed!");

    // load images
    (OUTPUTLEVEL > 1) ? puts ("opening images") : 0;
    if (pathToImage == NULL)
        pathToImage = "img.png";
    ORIG.surface = IMG_Load (pathToImage);
    if (!ORIG.surface) {
        return printf ("IMG_Load: failed to load %s: %s\n", pathToImage, IMG_GetError());
    }
    IMGWIDTH = ORIG.surface->w;
    IMGHEIGHT = ORIG.surface->h;

    (OUTPUTLEVEL > 1) ? puts ("creating buffers") : 0;
    // create the working and backbuffer images
    IMG1.surface = IMG_Load (pathToImage);
    IMG2.surface = IMG_Load (pathToImage);
    // clear the images
    SDL_LockSurface (IMG1.surface);
    SDL_LockSurface (IMG2.surface);
    memset (IMG1.surface->pixels, 0xFFFFFFFF, ORIG.surface->h*ORIG.surface->pitch);
    memset (IMG2.surface->pixels, 0xFFFFFFFF, ORIG.surface->h*ORIG.surface->pitch);
    SDL_UnlockSurface (IMG1.surface);
    SDL_UnlockSurface (IMG2.surface);

    #if MULTITHREADING
    // create image mutexes
    (OUTPUTLEVEL > 1) ? puts ("creating mutexes") : 0;
    ORIG.mutex = malloc (sizeof(Uint32)*ORIG.surface->h*ORIG.surface->pitch);
    ORIG.mutex = malloc (sizeof(Uint32)*IMG1.surface->h*IMG1.surface->pitch);
    ORIG.mutex = malloc (sizeof(Uint32)*IMG2.surface->h*IMG2.surface->pitch);

    pthread_mutex_init (ORIG.mutex, NULL);
    pthread_mutex_init (IMG1.mutex, NULL);
    pthread_mutex_init (IMG2.mutex, NULL);


    // create threads
    (OUTPUTLEVEL > 1) ? puts ("creating threads") : 0;
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = malloc (sizeof(pthread_t));
        tArgs[i] = malloc (sizeof(ThreadArgs));
        tArgs[i]->minx = i*(IMGWIDTH / numThreads);
        tArgs[i]->miny = i*(IMGHEIGHT / numThreads);
        tArgs[i]->minx = (i+1)*(IMGWIDTH / numThreads);
        tArgs[i]->miny = (i+1)*(IMGHEIGHT / numThreads);
        pthread_create (threads[i], NULL, handleBlock, tArgs[i]);
    }

    puts ("MAIN: ENTERING LOOP!");
    while (1) {

    }
    #else

    // main loop
    iterations = imgCount = 0;
    SDL_Event e;
    int quit = 0;
    while (iterations < MAXITERS && !quit) {
        // handle events
        while (SDL_PollEvent (&e)) {
            switch (e.type) {
                case SDL_QUIT:
                quit = 1;
                break;
            }
        }
        (OUTPUTLEVEL > 1) ? printf ("iteration #%i\n", iterations) : 0;

        // create a random line
        x0 = rand() % IMGWIDTH, y0 = rand() % IMGHEIGHT;
        x1 = x0 + (rand() % (1 + IMGWIDTH/(10 + rand() % 50)));
        y1 = y0 + (rand() % (1 + IMGHEIGHT/(10 + rand() % 50)));
        (OUTPUTLEVEL > 1) ? printf ("x0 %i, y0 %i,  x1 %i, y1 %i\n", x0, y0, x1, y1) : 0;
        colour = getPixel (&ORIG, x0, y0);

        /* check if the line is a good change
            and trash it if it isn't */
        if (imgCompare (&ORIG, &IMG1, x0, y0, x1, y1)
          <= imgCompare (&ORIG, &IMG2, x0, y0, x1, y1))
            // draw the line
            drawLine (&IMG1, x0, y0, x1, y1, colour);
        else {
            (OUTPUTLEVEL > 1) ? printf ("Skipped %i\n", ++skipped) : 0;
            continue;
        }
        SDL_BlitSurface (IMG1.surface, NULL, IMG2.surface, NULL);

        // save progress image if necessary
        if (PROGIMGS && iterations % PICDELAY == 0) {
            char outname[50];
            sprintf (outname, "pics/%lu.png", imgCount++);
            saveImage (&IMG1, outname);
            (OUTPUTLEVEL > 1) ? printf ("saved image pics/%lu.png\n", imgCount) : 0;
        }
        iterations++;
    }

    // save final image
    char outname[50];
    if (PROGIMGS) {
        sprintf (outname, "pics/%lu.png", imgCount);
        saveImage (&IMG1, outname);
    }
    sprintf (outname, "%lu.png", iterations);
    saveImage (&IMG1, outname);
    (OUTPUTLEVEL) ? printf ("Done in %i iterations\n", iterations) : 0;
    #endif

    // free the surfaces
    SDL_FreeSurface (ORIG.surface);
    ORIG.surface = NULL;
    SDL_FreeSurface (IMG1.surface);
    IMG1.surface = NULL;
    SDL_FreeSurface (IMG2.surface);
    IMG2.surface = NULL;

    #if MULTITHREADING
    // deallocate mutexes
    pthread_mutex_destroy (ORIG.mutex);
    pthread_mutex_destroy (IMG1.mutex);
    pthread_mutex_destroy (IMG2.mutex);
    #endif

    // shutdown libs
    cleanup();

    return 0;
}
