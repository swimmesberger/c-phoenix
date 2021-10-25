#pragma once

#include <allegro5/allegro5.h>

void header_init(void);
void header_update(ALLEGRO_TIMER_EVENT event);
int header_get_score(void);
void header_redraw(void);
void header_destroy(void);