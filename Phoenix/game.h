// file: src/game.h

#pragma once

#include <allegro5/allegro5.h>

void game_init(void);
void game_update(ALLEGRO_TIMER_EVENT event);
void game_redraw(void);

void mouse_move(ALLEGRO_MOUSE_EVENT event);
void mouse_down(ALLEGRO_MOUSE_EVENT event);
void mouse_up(ALLEGRO_MOUSE_EVENT event);

void key_down(ALLEGRO_KEYBOARD_EVENT event);
void key_up(ALLEGRO_KEYBOARD_EVENT event);
void key_char(ALLEGRO_KEYBOARD_EVENT event);

void game_destroy(void);