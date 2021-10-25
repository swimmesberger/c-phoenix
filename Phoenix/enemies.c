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

#define ENEMY_SHOOT_WAIT_TICK (FRAME_COUNT * 0.6f);
#define ENEMY_SHOOT_CHECK_TICK (FRAME_COUNT * 0.2f);

/*
 when the enemy moves back to the form position, a "diff" should be added that
 checks if the expected position is "nearly" reached and then the enemy should lock into the form position and reset isFlying
*/
typedef struct ENEMY {
  float pos_x; // the real position e.g. when flying
  float pos_y;
  float form_pos_x;  // the position in the enemy form
  float form_pos_y;
  int last_shoot_tick_count;
  bool is_flying;
  bool is_destroyed;
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

static int enemies_flying_count(void) {
  int flying_count = 0;
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed) {
      continue;
    }
    if (enemy->is_flying) {
      flying_count += 1;
    }
  }
  return flying_count;
}

static void enemies_update_flying(void) {
  int flying_count = enemies_flying_count();
  if (flying_count < MAX_FLYING_ENEMIES) {
    bool should_start_flying = (rand() % (10 - 1) + 1) == 1;
    if (should_start_flying) {
      int start_flying_enemy_idx = rand() % enemies_count;
      enemies[start_flying_enemy_idx]->is_flying = true;
    }
  }
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

static void enemies_update_shooting(void) {
  if (enemy_last_shoot_tick_check_count > 0) {
    enemy_last_shoot_tick_check_count -= 1;
    return;
  }
  enemy_last_shoot_tick_check_count = ENEMY_SHOOT_CHECK_TICK;

  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    if (enemy->is_destroyed || enemy->is_flying) {
      continue;
    }
    if (enemy->last_shoot_tick_count > 0) {
      enemy->last_shoot_tick_count -= 1;
      continue;
    }
    
    bool should_start_shooting = random_number(1, 100) == 1;
    if (should_start_shooting) {
      enemy_shoot(enemy);
    }
  }
}

static void enemies_update_ai(void) {
  // enemies_update_flying();
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
    GAME_PROJECTILE* hit_projectile = projectile_hit(enemy->pos_x, enemy->pos_y, enemy_width, enemy_height, PROJECTILE_MOVE_TYPE_UP);
    if (hit_projectile != NULL) {
      enemy_hit(i, enemy, hit_projectile);
    }
  }
}

static void enemies_update_formation(void) {
  if (enemies[0]->form_pos_x <= 0) {
    enemies_formation_move_type = ENEMY_FORMATION_MOVE_TYPE_RIGHT;
  } else if ((enemies[15]->form_pos_x + enemy_width) >= DISPLAY_WIDTH) {
    enemies_formation_move_type = ENEMY_FORMATION_MOVE_TYPE_LEFT;
  }
  float x_delta = enemies_formation_move_type == ENEMY_FORMATION_MOVE_TYPE_LEFT ? -1 : +1;
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    enemy->form_pos_x += x_delta;
    if (!enemy->is_flying) {
      enemy->pos_x = enemy->form_pos_x;
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
    case Level1:
      enemies_init_level1();
      break;
  }
}

void enemies_update(ALLEGRO_TIMER_EVENT event) {
  if (!sprite_animation_is_started(enemy_img)) {
    sprite_animation_start(enemy_img, SPRITE_ANIM_PLAY_MODE_LOOP);
  }
  sprite_animation_update(enemy_img);
  enemies_update_formation();
  enemies_check_projectile_hit();
  if (!game_complete) {
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

void enemies_destroy(void) {
  sprite_animation_destroy(&enemy_img);
  al_destroy_sample(enemy_shoot_sound);
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    enemies[i] = NULL;
    free(enemy);
  }
}