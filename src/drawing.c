/*
 * Functions for drawing shapes
 *
 */

#include "drawing.h"


/* --==[ PIXEL METHODS ]==-- */

/* draw pixel on a surface at x,y
    pixel format: 0xAABBGGRR */
void drawPixel (Image *img, int x, int y, Uint32 pixel) {
    if (x <= 0 || x >= IMGWIDTH || y <= 0 || y >= IMGHEIGHT)
        return;
    int bpp = img->surface->format->BytesPerPixel;
    /* get pixel address */
    (NUMTHREADS > 1) ? pthread_mutex_lock (img->mutex + y * img->surface->pitch + x * bpp) : 0;
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
    (NUMTHREADS > 1) ? pthread_mutex_unlock (img->mutex + y * img->surface->pitch + x * bpp) : 0;
}

/* get pixel from a surface at x,y
    pixel format: 0xAABBGGRR */
Uint32 getPixel (Image *img, int x, int y) {
    if (x <= 0 || x >= IMGWIDTH || y <= 0 || y >= IMGHEIGHT)
        return 0;
    int bpp = img->surface->format->BytesPerPixel;
    /* get pixel address */
    (NUMTHREADS > 1) ? pthread_mutex_lock (img->mutex + y * img->surface->pitch + x * bpp) : 0;
    Uint8 *p = (Uint8 *)img->surface->pixels + y * img->surface->pitch + x * bpp;
    (NUMTHREADS > 1) ? pthread_mutex_unlock (img->mutex + y * img->surface->pitch + x * bpp) : 0;

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

/* --==[ LINE METHODS ]==-- */

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

/* compare a line from two images, return the difference */
int testLine (Image *img1, Image *img2, int x0, int y0, int x1, int y1, Uint32 c) {

    Uint32 img1Colour, img2Colour, count;
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
        img2Colour += (c) ? c : getPixel (img2, x0, y0);
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



/* --==[ CIRCLE METHODS ]==-- */

/* draw a filled circle */
void drawCircle (Image *img, int x0, int y0, int r, int dummy, Uint32 c) {

    SDL_LockSurface (img->surface);

    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y) {
        for (int x1 = 0; x1 < x; ++x1)
            for (int m1 = -1; m1 <= 1; m1 += 2)
                for (int m2 = -1; m2 <= 1; m2 += 2) {
                    drawPixel (img, x0 + (m1*x1), y0 + (m2*y), c);
                    drawPixel (img, x0 + (m1*y), y0 + (m2*x1), c);
                }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += (-r << 1) + dx;
        }
    }
    SDL_UnlockSurface (img->surface);
}

/* compare a filled circle from two images, return the difference */
int testCircle (Image *img1, Image *img2, int x0, int y0, int r, int dummy, Uint32 c) {

    Uint32 img1Colour, img2Colour;
    img1Colour = img2Colour = 0;
    int count = 1;

    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y) {
        count++;
        for (int x1 = 0; x1 < x; ++x1)
            for (int m1 = -1; m1 <= 1; m1 += 2)
                for (int m2 = -1; m2 <= 1; m2 += 2) {
                    img1Colour += getPixel (img1, x0 + (m1*x1), y0 + (m2*y));
                    img2Colour += (c) ? c : getPixel (img2, x0 + (m1*y), y0 + (m2*x1));
                }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += (-r << 1) + dx;
        }
    }

    return abs ((img1Colour/count)-(img2Colour/count));
}

/* draw an unfilled circle */
void drawEmptyCircle (Image *img, int x0, int y0, int r, int dummy, Uint32 c) {

    SDL_LockSurface (img->surface);

    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y) {
        for (int m1 = -1; m1 <= 1; m1 += 2)
            for (int m2 = -1; m2 <= 1; m2 += 2) {
                drawPixel (img, x0 + (m1*x), y0 + (m2*y), c);
                drawPixel (img, x0 + (m1*y), y0 + (m2*x), c);
            }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += (-r << 1) + dx;
        }
    }
    SDL_UnlockSurface (img->surface);
}

/* compare an unfilled circle from two images, return the difference */
int testEmptyCircle (Image *img1, Image *img2, int x0, int y0, int r, int dummy, Uint32 c) {

    Uint32 img1Colour, img2Colour;
    img1Colour = img2Colour = 0;
    int count = 0;


    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);

    while (x >= y) {
        count++;
        for (int m1 = -1; m1 <= 1; m1 += 2)
            for (int m2 = -1; m2 <= 1; m2 += 2) {
                img1Colour += getPixel (img1, x0 + (m1*x), y0 + (m2*y));
                img1Colour += getPixel (img1, x0 + (m1*y), y0 + (m2*x));
                img2Colour += (c) ? c : getPixel (img2, x0 + (m1*x), y0 + (m2*y));
                img2Colour += (c) ? c : getPixel (img2, x0 + (m1*y), y0 + (m2*x));
            }

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += (-r << 1) + dx;
        }
    }
    return abs ((img1Colour/count)-(img2Colour/count));
}