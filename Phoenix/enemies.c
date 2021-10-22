#include "enemies.h"
#include <stdbool.h>
#include <allegro5/events.h>

#define MAX_FLYING_ENEMIES 5

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

ENEMY* enemies[] = {0};
int enemies_count = 0;

static void enemies_init_level1(void) {

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

static void enemie_update_flying(void) {
  int flying_count = enemies_flying_count();
  if (flying_count < MAX_FLYING_ENEMIES) {
    bool should_start_flying = (rand() % (10 - 1) + 1) == 1;
    if (should_start_flying) {
      int start_flying_enemy_idx = rand() % enemies_count;
      enemies[start_flying_enemy_idx]->isFlying = true;
    }
  }
}

void enemies_init(LEVEL_TYPE levelType) {
  switch (levelType) {
    case Level1:
      enemies_init_level1();
      break;
  }
}

void enemies_update(ALLEGRO_TIMER_EVENT event) {

}

void enemies_redraw(void) {

}

void enemies_destroy(void) {

}