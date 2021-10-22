#pragma once

#include <allegro5/allegro5.h>

void background_init(void);
void background_update(ALLEGRO_TIMER_EVENT event);
void background_redraw(void);
void background_destroy(void);