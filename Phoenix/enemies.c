#include "enemies.h"
#include "utils.h"
#include "sprite_anim.h"
#include "projectile.h"
#include "explosions.h"
#include "resources.h"
#include "map.h"

#include <stdbool.h>
#include <allegro5/events.h>
#include <allegro5/allegro_audio.h>

#define MAX_FLYING_ENEMIES 5

#define FLY_ANIMATION_FRAME_COUNT 10
#define FLY_FRAME_WIDTH 29
#define FLY_FRAME_0_X 0
#define FLY_FRAME_1_X (FLY_FRAME_0_X + FLY_FRAME_WIDTH)
#define FLY_FRAME_2_X (FLY_FRAME_1_X + FLY_FRAME_WIDTH)
#define FLY_FRAME_SPEED 0.833f
#define FLY_FRAME_COUNT 3

#define ENEMY_SHOOT_WAIT_TICK (FRAME_COUNT * 0.6f) // 3/5 of a second
#define ENEMY_SHOOT_CHECK_TICK (FRAME_COUNT * 0.2f) // 1/5 of a second
#define ENEMY_SHOOT_PERCENTAGE 80 // 1 in 80 chance that an enemy shoots when asked

#define ENEMY_MOVE_FORWARD_TICK (FRAME_COUNT * 7.0f) // every 7 seconds move forward
#define ENEMY_MOVE_FORWARD_PIXELS 15.0f
#define ENEMY_MOVE_FORWARD_BOTTOM_SPACING 60.0f // how many pixels spacing between bottom screen when moving forward

/*
 when the enemy moves back to the form position, a "diff" should be added that
 checks if the expected position is "nearly" reached and then the enemy should lock into the form position and reset isFlying
*/
typedef struct ENEMY {
  float pos_x; // the real x position e.g. when flying
  float pos_y; // the real y position -^
  float form_pos_x;  // the x position in the enemy form - reserved for future use to return back to the "original" position after "kamikaze"
  float form_pos_y; // the y position in the enemy form -^
  int last_shoot_tick_count; // "cooldown" for shooting - 0 = allowed to shoot, >0 = not allowed to shoot
  bool is_flying; // reserved for future use for "kamikaze attack" ai
  bool is_destroyed; // we do not "remove" the enemy when destroyed we simply set this flag and "ignore" it, this allows us to still keep the "original formation"
} ENEMY;

typedef enum ENEMY_FORMATION_MOVE_TYPE {
  ENEMY_FORMATION_MOVE_TYPE_LEFT,
  ENEMY_FORMATION_MOVE_TYPE_RIGHT
} ENEMY_FORMATION_MOVE_TYPE;

SPRITE_ANIM* enemy_img;
ALLEGRO_SAMPLE* enemy_shoot_sound;
ALLEGRO_COLOR enemy_shoot_color;
int enemy_width;
int enemy_height;
ENEMY** enemies = {0};
int enemies_count = 0;

ENEMY_FORMATION_MOVE_TYPE enemies_formation_move_type = ENEMY_FORMATION_MOVE_TYPE_LEFT;
int enemy_last_shoot_tick_check_count = ENEMY_SHOOT_CHECK_TICK;
int enemy_move_forward_tick = ENEMY_MOVE_FORWARD_TICK;

static void enemies_init_enemy(ENEMY* enemy, int pos_x, int pos_y) {
  enemy->pos_x = pos_x;
  enemy->pos_y = pos_y;
  enemy->form_pos_x = pos_x;
  enemy->form_pos_y = pos_y;
  enemy->last_shoot_tick_count = 0;
  enemy->is_flying = false;
  enemy->is_destroyed = false;
}

static void enemies_init_level1(void) {
  enemies = (ENEMY**)assert_not_null(malloc(16 * sizeof(ENEMY*)), "Enemies Array");
  enemies_count = 16;
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = (ENEMY*)assert_not_null(malloc(sizeof(ENEMY)), "Enemy");
    enemies[i] = enemy;
  }

  enemies_init_enemy(enemies[0], 50, 90);
  enemies_init_enemy(enemies[1], 50, 120);

  enemies_init_enemy(enemies[2], 80, 75);
  enemies_init_enemy(enemies[3], 80, 135);

  enemies_init_enemy(enemies[4], 110, 60);
  enemies_init_enemy(enemies[5], 110, 150);

  enemies_init_enemy(enemies[6], 140, 165);

  enemies_init_enemy(enemies[7], 180, 135);
  enemies_init_enemy(enemies[8], 180, 165);

  enemies_init_enemy(enemies[9], 220, 165);

  enemies_init_enemy(enemies[10], 250, 60);
  enemies_init_enemy(enemies[11], 250, 150);

  enemies_init_enemy(enemies[12], 280, 75);
  enemies_init_enemy(enemies[13], 280, 135);

  enemies_init_enemy(enemies[14], 310, 90);
  enemies_init_enemy(enemies[15], 310, 120);
}

static bool enemy_shoot(ENEMY* enemy) {
  // do not allow shooting when it is still on "cooldown"
  if (enemy->last_shoot_tick_count > 0) {
    return false;
  }

  al_play_sample(enemy_shoot_sound, 1.0f, 0.5f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
  // spawn the projectile at the center at the top of the ship
  projectile_add(enemy->pos_x + enemy_width / 2.0f - PROJECTILE_WIDTH / 2.0f,
                 enemy->pos_y + enemy_height, enemy_shoot_color, PROJECTILE_MOVE_TYPE_DOWN);
  enemy->last_shoot_tick_count = ENEMY_SHOOT_WAIT_TICK;
  return true;
}

static void enemies_update_shooting_cooldown(void) {
  // update global shoot cooldown
  if (enemy_last_shoot_tick_check_count > 0) {
    enemy_last_shoot_tick_check_count -= 1;
  }
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    // update enemy specific shoot cooldown
    if (enemy->last_shoot_tick_count > 0) {
      enemy->last_shoot_tick_count -= 1;
    }
  }
}

static void enemies_update_shooting(void) {
  enemies_update_shooting_cooldown();
  if (enemy_last_shoot_tick_check_count > 0) {
    // global shoot cooldown has not elapsed yet
    return;
  }

  // reset global shoot cooldown
  enemy_last_shoot_tick_check_count = ENEMY_SHOOT_CHECK_TICK;
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    // destroyed enemies are not allowed to shoot
    if (enemy->is_destroyed) {
      continue;
    }

    // roll the dice if the enemy is allowed to shoot
    bool should_start_shooting = random_number(1, ENEMY_SHOOT_PERCENTAGE) == 1;
    if (should_start_shooting) {
      enemy_shoot(enemy);
      // only a single enemy per shoot tick is allowed to shoot 
      break;
    }
  }
}

static void enemies_update_ai(void) {
  // enemies_update_flying(); // not implemented yet -> KAMIKAZE AI
  enemies_update_shooting();
}

static void enemy_hit(int idx, ENEMY* enemy, GAME_PROJECTILE* hit_projectile) {
  float pos_center_x = enemy->pos_x + enemy_width / 2.0f;
  float pos_center_y = enemy->pos_y + enemy_height / 2.0f;
  float expl_center_x = EXPLOSION_FRAME_WIDTH / 2.0f;
  float expl_center_y = EXPLOSION_FRAME_HEIGHT / 2.0f;
  explosion_add(pos_center_x - expl_center_x, pos_center_y - expl_center_y);
  projectile_remove(hit_projectile);
  enemies[idx]->is_destroyed = true;
}

static void enemies_check_projectile_hit(void) {
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }

    int enemy_width = sprite_animation_get_width(enemy_img);
    int enemy_height = sprite_animation_get_height(enemy_img);
    // check if the current enemy got hit by an projectile
    GAME_PROJECTILE* hit_projectile = projectile_hit(enemy->pos_x, enemy->pos_y, enemy_width, enemy_height, PROJECTILE_MOVE_TYPE_UP);
    if (hit_projectile != NULL) {
      enemy_hit(i, enemy, hit_projectile);
    }
  }
}

// get a bounding box of the whole enemy formation based on the non-destroyed enemies
static void enemies_get_bounding(ENEMY** left, ENEMY** top, ENEMY** right, ENEMY** bottom) {
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }

    if (left != NULL && ((*left) == NULL ||  enemy->form_pos_x < (*left)->form_pos_x)) {
      *left = enemy;
    }
    if (top != NULL && ((*top) == NULL || enemy->form_pos_y < (*top)->form_pos_y)) {
      *top = enemy;
    }
    if (right != NULL && ((*right) == NULL || enemy->form_pos_x > (*right)->form_pos_x)) {
      *right = enemy;
    }
    if (bottom != NULL && ((*bottom) == NULL || enemy->form_pos_y > (*bottom)->form_pos_y)) {
      *bottom = enemy;
    }
  }
}

static void enemies_update_formation(void) {
  // when the left most enemy reaches the most left position change motion to
  // left -> right
  ENEMY* left = NULL;
  ENEMY* right = NULL;
  ENEMY* bottom = NULL;
  enemies_get_bounding(&left, NULL, &right, &bottom);
  if (left != NULL && left->form_pos_x <= 0) {
    enemies_formation_move_type = ENEMY_FORMATION_MOVE_TYPE_RIGHT;
  } else if (right != NULL &&  (right->form_pos_x + enemy_width) >=  DISPLAY_WIDTH) {
    // when the right most enemy reaches the most right position change motion to right -> left
    enemies_formation_move_type = ENEMY_FORMATION_MOVE_TYPE_LEFT;
  }
  // +-1px right/left based on current move type
  float x_delta = enemies_formation_move_type == ENEMY_FORMATION_MOVE_TYPE_LEFT ? -1 : +1;

  // move the whole formation forward as long as the most bottom enemy has still some space to the screen bottom
  float y_delta = 0;
  // we only move forward in a specified time
  enemy_move_forward_tick -= 1;
  if (enemy_move_forward_tick <= 0) {
    enemy_move_forward_tick = ENEMY_MOVE_FORWARD_TICK;
    if (bottom != NULL && (bottom->form_pos_y + enemy_height) < (DISPLAY_HEIGHT - ENEMY_MOVE_FORWARD_BOTTOM_SPACING)) {
      y_delta = ENEMY_MOVE_FORWARD_PIXELS;
    }
  }

  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    enemy->form_pos_x += x_delta;
    enemy->form_pos_y += y_delta;
    // do not change the real position when the enems is in kamikaze mode
    if (!enemy->is_flying) {
      enemy->pos_x = enemy->form_pos_x;
      enemy->pos_y = enemy->form_pos_y;
    }
  }
}

void enemies_init(LEVEL_TYPE levelType) {
  // for 60 fps we end up with around a frame change every 10th frame
  enemy_img = assert_not_null(sprite_animation_load("sprites/enemy_sprites.png", FLY_FRAME_COUNT, FLY_FRAME_WIDTH, FLY_FRAME_SPEED), "Enemy Image");
  enemy_width = sprite_animation_get_width(enemy_img);
  enemy_height = sprite_animation_get_height(enemy_img);
  enemy_shoot_sound = assert_not_null(al_load_sample("sounds/SHOT8.WAV"), "Enemy Shoot Sound");
  enemy_shoot_color = al_map_rgb(255, 255, 255);
  enemies_count = 0;

  switch (levelType) {
    case LEVEL_TYPE_LEVEL_1:
      enemies_init_level1();
      break;
  }

  // every enemy is always in "sync" with the other so we only need to animate a single sprite that is used for every enemy
  sprite_animation_start(enemy_img, SPRITE_ANIM_PLAY_MODE_LOOP);
}

void enemies_update(ALLEGRO_TIMER_EVENT event) {
  sprite_animation_update(enemy_img);
  // these things does not need update when the game has ended
  if (!game_complete) {
    enemies_update_formation();
    enemies_check_projectile_hit();
    enemies_update_ai();
  }
}

void enemies_redraw(void) {
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }
    sprite_animation_draw(enemy_img, enemy->pos_x, enemy->pos_y);
  }
}

// count of still alive enemies
int enemies_get_count(void) {
  int count = 0;
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }
    count += 1;
  }
  return count;
}

// is the passed rectangle (e.g. the player) hit by one of the enemies
bool enemies_hit(float pos_x, float pos_y, int width, int height) {
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }
    if (is_rect_intersect(pos_x, pos_y, width, height, enemy->pos_x,
                          enemy->pos_y, enemy_width, enemy_height)) {
      return true;
    }
  }
  return false;
}

void enemies_destroy(void) {
  sprite_animation_destroy(&enemy_img);
  al_destroy_sample(enemy_shoot_sound);
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    enemies[i] = NULL;
    free(enemy);
  }
}