#include "game.h"
#include "player.h"
#include "projectile.h"
#include "background.h"
#include "enemies.h"
#include "explosions.h"
#include "resources.h"
#include "map.h"
#include "header.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <allegro5/allegro5.h>

#include "resources.h"
#include "utils.h"

ALLEGRO_COLOR background_color;


static void game_over(void) {
  map_set_full_screen_text_printf("Game Over");
  map_set_full_screen_text_printf_sub("Score %06d", header_get_score());
  game_complete = true;
}

static void game_win(void) {
  map_set_full_screen_text_printf("Level Complete");
  map_set_full_screen_text_printf_sub("Score %06d", header_get_score());
  game_complete = true;
}

static void game_check_end(void) {
  if (enemies_get_count() <= 0 && !game_complete) {
    game_win();
  } else if (player_get_life() <= 0 && !game_complete) {
    game_over();
  }
}

void game_init(void) {
  srand(time(NULL));
  background_color = al_map_rgb(0, 0, 0);

  map_init();
  background_init();
  projectile_engine_init();
  explosions_init();
  enemies_init(LEVEL_TYPE_LEVEL_1);
  player_init();
  header_init();
}

void game_destroy(void) {
  header_destroy();
  player_destroy();
  enemies_destroy();
  projectile_engine_destroy();
  explosions_destroy();
  background_destroy();
  map_destroy();
}

void game_update(ALLEGRO_TIMER_EVENT event) {
  background_update(event);
  enemies_update(event);
  player_update(event);
  projectile_engine_update(event);
  explosions_update(event);
  header_update(event);
  game_check_end();
}

void game_redraw(void) { 
  al_clear_to_color(background_color);
  background_redraw();
  enemies_redraw();
  player_redraw();
  projectile_engine_redraw();
  explosions_redraw();
  map_redraw();
  header_redraw();
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