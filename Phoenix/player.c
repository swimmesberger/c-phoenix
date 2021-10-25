#include "player.h"
#include "projectile.h"
#include "resources.h"
#include "utils.h"
#include "explosions.h"
#include "map.h"

#include <allegro5/allegro_audio.h>
#include <stdlib.h>

#define PLAYER_SPEED 2.5f
#define PLAYER_Y_AREA 100
#define PLAYER_Y_MAX (DISPLAY_HEIGHT - PLAYER_Y_AREA)
// how many ticks until a new shoot is possible
#define PLAYER_SHOOT_WAIT_TICK (FRAME_COUNT * 0.6f);
#define PLAYER_HIT_WAIT_TICK (FRAME_COUNT * 0.15f);
#define PLAYER_HIT_TICK_PHASES 20;

ALLEGRO_BITMAP* ship_img;
ALLEGRO_SAMPLE* ship_shoot_sound;

int player_width;
int player_height;
float player_pos_x;
float player_pos_y;
bool player_visible = true;

float player_x_velocity = 0.0f;
int player_x_down_count = 0;
float player_y_velocity = 0.0f;
int player_y_down_count = 0;

ALLEGRO_COLOR player_shoot_color;
// disallow shooting as long as this count is > 0
int player_last_shoot_tick_count = 0;

EXPLOSION* player_explosion = NULL;
int player_life = 3;
int player_hit_tick = 0;
int player_hit_tick_phase = 0;

static void player_hit(GAME_PROJECTILE* hit_projectile) {
  float pos_center_x = player_pos_x + player_width / 2.0f;
  float pos_center_y = player_pos_y + player_height / 2.0f;
  float expl_center_x = EXPLOSION_FRAME_WIDTH / 2.0f;
  float expl_center_y = EXPLOSION_FRAME_HEIGHT / 2.0f;
  player_explosion = explosion_add(pos_center_x - expl_center_x,
                                pos_center_y - expl_center_y);
  projectile_remove(hit_projectile);
  player_life -= 1;
  player_visible = false;
}

static bool player_is_exploding(void) {
  return player_explosion != NULL && explosion_visible(player_explosion);
}

static bool player_is_invincible(void) {
  return player_is_exploding() || player_hit_tick_phase > 0;
}

static void player_check_projectile_hit(void) {
  // player is invincible for a short time when he was hit
  if (player_is_invincible()) {
    return;
  }

  GAME_PROJECTILE* hit_projectile =
      projectile_hit(player_pos_x, player_pos_y, player_width, player_height,
                      PROJECTILE_MOVE_TYPE_DOWN);
  if (hit_projectile != NULL) {
    player_hit(hit_projectile);
  }
}

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
  // do not allow shooting when it is still on "cooldown"
  if (player_last_shoot_tick_count > 0 || game_complete || player_is_exploding()) {
    return false;
  }

  al_play_sample(ship_shoot_sound, 1.0f, 0.5f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
  // spawn the projectile at the center at the top of the ship
  projectile_add(player_pos_x + player_width/2.0f - PROJECTILE_WIDTH/2.0f, player_pos_y, player_shoot_color, PROJECTILE_MOVE_TYPE_UP);
  player_last_shoot_tick_count = PLAYER_SHOOT_WAIT_TICK;
  return true;
}

static void player_explosion_update(void) {
  if (!player_visible && player_is_exploding()) {
    player_hit_tick = PLAYER_HIT_WAIT_TICK;
    player_hit_tick_phase = PLAYER_HIT_TICK_PHASES;
    player_visible = true;
  } else if(player_hit_tick_phase > 0) {
    player_explosion = NULL;
    if (player_hit_tick > 0) {
      player_hit_tick -= 1;
    } else if (player_hit_tick_phase > 0) {
      player_hit_tick = PLAYER_HIT_WAIT_TICK;
      player_hit_tick_phase -= 1;
    }
    if (player_hit_tick_phase > 0) {
      player_visible = player_hit_tick_phase % 2 == 0;
    }
  } else {
    player_explosion = NULL;
    player_visible = true;
  }
}

static void player_cooldown_update(void) {
  if (player_last_shoot_tick_count > 0) {
    player_last_shoot_tick_count -= 1;
  }
}

void player_init(void) {
  ship_img = assert_not_null(al_load_bitmap("sprites/ship.png"), "Ship Image");
  player_width = al_get_bitmap_width(ship_img);
  player_height = al_get_bitmap_height(ship_img);
  player_pos_x = (DISPLAY_WIDTH - player_width) / 2;
  player_pos_y = DISPLAY_HEIGHT - player_height - 30;
  player_shoot_color = al_map_rgb(255, 255, 255);
  ship_shoot_sound = assert_not_null(al_load_sample("sounds/SHOT8.WAV"), "Ship Shoot Sound");
}

int player_get_life(void) {
  return player_life;
}

void player_destroy(void) {
  al_destroy_bitmap(ship_img);
  al_destroy_sample(ship_shoot_sound);
}

void player_update(ALLEGRO_TIMER_EVENT event) {
  if (player_life <= 0) {
    return;
  }
  if (!game_complete && !player_is_exploding()) {
    player_try_move_x(player_x_velocity);
    player_try_move_y(player_y_velocity);
  }

  player_check_projectile_hit();
  player_cooldown_update();
  player_explosion_update();
}

void player_redraw(void) {
  if (player_life <= 0) {
    return;
  }
  if (player_visible) {
    al_draw_bitmap(ship_img, player_pos_x, player_pos_y, 0);
  }
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