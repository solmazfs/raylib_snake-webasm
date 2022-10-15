/*******************************************************************************************
*
*   raylib study [timer.h] - snake
*
*   Game originally created with raylib 4.2, last time updated with raylib 4.2
*
*   Game licensed under MIT.
*
*   Copyright (c) 2022 Fatih S. Solmaz (@solmazfs)
*
********************************************************************************************/


#ifndef TIMER_H__
#define TIMER_H__

#include <stdbool.h>

typedef struct {
    float lifetime;
} Timer;

void start_timer(Timer *t, float lifetime);
void update_timer(Timer *t);
bool timer_done(Timer *t);

#endif
