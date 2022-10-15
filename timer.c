/*******************************************************************************************
*
*   raylib study [timer.c] - snake
*
*   Game originally created with raylib 4.2, last time updated with raylib 4.2
*
*   Game licensed under MIT.
*
*   Copyright (c) 2022 Fatih S. Solmaz (@solmazfs)
*
********************************************************************************************/

// following guide : https://www.youtube.com/watch?v=vGlvTWUctTQ&t=2s

#include <stddef.h>  // NULL
#include <stdbool.h> // BOOL

#include "raylib.h"  // videogame library
#include "timer.h"   // custom header

void start_timer(Timer *t, float lifetime) {
    if (t != NULL) {
        t->lifetime = lifetime;
    }
}

void update_timer(Timer *t) {
    if (t != NULL && t->lifetime > 0) {
        t->lifetime -= GetFrameTime();
    }
}

bool timer_done(Timer *t) {
    if (t != NULL) {
        return t->lifetime <= 0;
    }
    return false;
}
