// file: src/game.c
#include "game.h"
#include "player.h"
#include "projectile.h"
#include "background.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro5.h>

#include "resources.h"
#include "utils.h"

ALLEGRO_COLOR color_back;

void game_init(void) {
	srand(time(NULL));
  color_back = al_map_rgb(0, 0, 0);
  background_init();
  projectile_engine_init();
  player_init();
}

void game_destroy(void) {
  background_destroy();
	player_destroy();
  projectile_engine_destroy();
}

void game_update(ALLEGRO_TIMER_EVENT event) {
  background_update(event);
	player_update(event);
  projectile_engine_update(event);
}

void game_redraw(void) { 
  al_clear_to_color(color_back);
  background_redraw();
  player_redraw();
  projectile_engine_redraw();

	al_flip_display();
}

void mouse_move(ALLEGRO_MOUSE_EVENT event) {
}

void mouse_down(ALLEGRO_MOUSE_EVENT event) {
}

void mouse_up(ALLEGRO_MOUSE_EVENT event) {
}

void key_down(ALLEGRO_KEYBOARD_EVENT event) {
  player_key_down(event);
}

void key_up(ALLEGRO_KEYBOARD_EVENT event) {
  player_key_up(event);
}

void key_char(ALLEGRO_KEYBOARD_EVENT event) {

}