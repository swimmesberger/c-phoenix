#pragma once

#include <allegro5/allegro5.h>

void player_init(void);
void player_update(ALLEGRO_TIMER_EVENT event);
void player_redraw(void);

void player_key_down(ALLEGRO_KEYBOARD_EVENT event);
void player_key_up(ALLEGRO_KEYBOARD_EVENT event);

void player_destroy(void);