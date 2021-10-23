#include "enemies.h"
#include "utils.h"
#include <stdbool.h>
#include <allegro5/events.h>

#define MAX_FLYING_ENEMIES 5

#define FLY_ANIMATION_FRAME_COUNT 10
#define FLY_FRAME_HEIGHT 15
#define FLY_FRAME_WIDTH 29
#define FLY_FRAME_0_X 0
#define FLY_FRAME_1_X (FLY_FRAME_0_X + FLY_FRAME_WIDTH + 2)
#define FLY_FRAME_2_X (FLY_FRAME_1_X + FLY_FRAME_WIDTH + 2)

/*
 when the enemy moves back to the form position, a "diff" should be added that
 checks if the expected position is "nearly" reached and then the enemy should lock into the form position and reset isFlying
*/
typedef struct ENEMY {
  float pos_x; // the real position e.g. when flying
  float pos_y;
  float form_pos_x;  // the position in the enemy form
  float form_pos_y;
  bool isFlying;
} ENEMY;

ALLEGRO_BITMAP* enemy_img;
int enemy_width;
int enemy_height;

int enemy_formation_frame = 0;
int enemy_formation_frame_count = 0;

ENEMY** enemies = {0};
int enemies_count = 0;

static void enemies_init_enemy(ENEMY* enemy, int pos_x, int pos_y) {
  enemy->pos_x = pos_x;
  enemy->pos_y = pos_y;
  enemy->form_pos_x = pos_x;
  enemy->form_pos_y = pos_y;
  enemy->isFlying = false;
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
    if (enemy->isFlying) {
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
      enemies[start_flying_enemy_idx]->isFlying = true;
    }
  }
}

static void enemies_draw_formation_frame(int pos_x, int pos_y) {
  switch (enemy_formation_frame) {
    case 1:
      al_draw_bitmap_region(enemy_img, FLY_FRAME_1_X, 0, FLY_FRAME_WIDTH,
                            enemy_height, pos_x, pos_y, 0);
      break;
    case 2:
      al_draw_bitmap_region(enemy_img, FLY_FRAME_2_X, 0, FLY_FRAME_WIDTH,
                            enemy_height, pos_x, pos_y, 0);
      break;
    default:
      al_draw_bitmap_region(enemy_img, FLY_FRAME_0_X, 0, FLY_FRAME_WIDTH,
                            enemy_height, pos_x, pos_y, 0);
      break;
  }
}

void enemies_init(LEVEL_TYPE levelType) {
  enemy_img = assert_not_null(al_load_bitmap("sprites/enemy_sprites.png"), "Enemy Image");
  enemy_width = al_get_bitmap_width(enemy_img);
  enemy_height = al_get_bitmap_height(enemy_img);
  enemies_count = 0;

  switch (levelType) {
    case Level1:
      enemies_init_level1();
      break;
  }
}

void enemies_update(ALLEGRO_TIMER_EVENT event) {
  enemy_formation_frame_count += 1;
  if (enemy_formation_frame_count > FLY_ANIMATION_FRAME_COUNT) {
    enemy_formation_frame_count = 0;
    enemy_formation_frame += 1;
    if (enemy_formation_frame > 2) {
      enemy_formation_frame = 0;
    }
  }
}

void enemies_redraw(void) {
  for (int i = 0; i < enemies_count; i++) {
    ENEMY* enemy = enemies[i];
    enemies_draw_formation_frame(enemy->pos_x, enemy->pos_y);
  }
}

void enemies_destroy(void) {

}