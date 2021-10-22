#include "player.h"
#include "projectile.h"

#include "resources.h"
#include "utils.h"
#include <allegro5/allegro_audio.h>
#include <stdlib.h>

#define PLAYER_SPEED 5
#define PLAYER_Y_AREA 100
#define PLAYER_Y_MAX (DISPLAY_HEIGHT - PLAYER_Y_AREA)

ALLEGRO_BITMAP* ship_img;
ALLEGRO_SAMPLE* ship_shoot_sound;

int player_width;
int player_height;
float player_pos_x;
float player_pos_y;

float player_x_velocity = 0.0f;
int player_x_down_count = 0;
float player_y_velocity = 0.0f;
int player_y_down_count = 0;

ALLEGRO_COLOR color_white;
GAME_PROJECTILE* current_projectile = NULL;

static bool player_try_move_x(float x_delta) {
  if (x_delta == 0) {
    return true;
  }

  float player_new_pos_left = player_pos_x + x_delta;
  float player_new_pos_right = player_new_pos_left + player_width;
  bool can_move = true;
  if (player_new_pos_left < 0) {
    can_move = false;
  } else if (player_new_pos_right > DISPLAY_WIDTH) {
    can_move = false;
  }
  if (!can_move) {
    player_pos_x = x_delta < 0 ? 0 : (DISPLAY_WIDTH-player_width);
    return false;
  }

  player_pos_x = player_new_pos_left;

  return true;
}

static bool player_try_move_y(float y_delta) {
  if (y_delta == 0) {
    return true;
  }
  float player_new_pos_top = player_pos_y + y_delta;
  float player_new_pos_bottom = player_new_pos_top + player_height;
  bool can_move = true;
  if (player_new_pos_bottom > DISPLAY_HEIGHT) {
    can_move = false;
  } else if (player_new_pos_top < PLAYER_Y_MAX) {
    can_move = false;
  }
  if (!can_move) {
    player_pos_y = y_delta < 0 ? PLAYER_Y_MAX : (DISPLAY_HEIGHT-player_height);
    return false;
  }

  player_pos_y = player_new_pos_top;

  return true;
}

static bool player_shoot(void) {
  // do not allow shooting when the previous projectile is still active
  if (current_projectile != NULL && projectile_enabled(current_projectile)) {
    return false;
  }

  al_play_sample(ship_shoot_sound, 1.0f, 0.5f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
  current_projectile = projectile_add(player_pos_x + player_width/2.0f - PROJECTILE_WIDTH/2.0f, player_pos_y, color_white, Up);
  return true;
}

void player_init(void) {
  ship_img = assert_not_null(al_load_bitmap("sprites/ship.png"), "Ship Image");
  player_width = al_get_bitmap_width(ship_img);
  player_height = al_get_bitmap_height(ship_img);
  player_pos_x = (DISPLAY_WIDTH - player_width) / 2;
  player_pos_y = DISPLAY_HEIGHT - player_height - 30;
  color_white = al_map_rgb(255, 255, 255);
  ship_shoot_sound = assert_not_null(al_load_sample("sounds/SHOT8.WAV"), "Ship Shoot Sound");
}

void player_destroy(void) {
  al_destroy_bitmap(ship_img);
  al_destroy_sample(ship_shoot_sound);
}

void player_update(ALLEGRO_TIMER_EVENT event) {
  player_try_move_x(player_x_velocity);
  player_try_move_y(player_y_velocity);
}

void player_redraw(void) {
  al_draw_bitmap(ship_img, player_pos_x, player_pos_y, 0);
}

void player_key_down(ALLEGRO_KEYBOARD_EVENT event) {
  switch (event.keycode) {
    case ALLEGRO_KEY_LEFT:
      player_x_velocity = -PLAYER_SPEED;
      player_x_down_count += 1;
      break;
    case ALLEGRO_KEY_RIGHT:
      player_x_velocity = +PLAYER_SPEED;
      player_x_down_count += 1;
      break;
    case ALLEGRO_KEY_UP:
      player_y_velocity = -PLAYER_SPEED;
      player_y_down_count += 1;
      break;
    case ALLEGRO_KEY_DOWN:
      player_y_velocity = +PLAYER_SPEED;
      player_y_down_count += 1;
      break;
    case ALLEGRO_KEY_SPACE:
      player_shoot();
      break;
  }
}

void player_key_up(ALLEGRO_KEYBOARD_EVENT event) {
  switch (event.keycode) {
    case ALLEGRO_KEY_RIGHT:
      player_x_down_count -= 1;
      break;
    case ALLEGRO_KEY_LEFT:
      player_x_down_count -= 1;
      break;
    case ALLEGRO_KEY_UP:
      player_y_down_count -= 1;
      break;
    case ALLEGRO_KEY_DOWN:
      player_y_down_count -= 1;
      break;
  }
  if (player_x_down_count <= 0) {
    player_x_velocity = 0;
    player_x_down_count = 0;
  }
  if (player_y_down_count <= 0) {
    player_y_velocity = 0;
    player_y_down_count = 0;
  }
}