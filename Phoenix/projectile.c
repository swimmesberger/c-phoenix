#include "projectile.h"
#include "resources.h"
#include "utils.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <assert.h>

// we pre-init an array of projectiles - this is the maximum possible amount
#define MAX_PROJECTILE_COUNT 50
// how many pixels a projectile moves each tick
#define PROJECTILE_SPEED 10

typedef struct GAME_PROJECTILE {
  float pos_x;
  float pos_y;
  ALLEGRO_COLOR color;
  PROJECTILE_MOVE_TYPE moveType;
  bool enabled;
} GAME_PROJECTILE;

GAME_PROJECTILE* projectiles[MAX_PROJECTILE_COUNT];
ALLEGRO_COLOR shoot_color;

static void projectile_disable(GAME_PROJECTILE* projectile) {
  projectile->pos_x = 0;
  projectile->pos_y = 0;
  projectile->color = shoot_color;
  projectile->moveType = PROJECTILE_MOVE_TYPE_UP;
  projectile->enabled = false;
}

static void projectile_enable(GAME_PROJECTILE* projectile, int pos_x, int pos_y, ALLEGRO_COLOR color, PROJECTILE_MOVE_TYPE moveType) {
  projectile->pos_x = pos_x;
  projectile->pos_y = pos_y;
  projectile->color = color;
  projectile->moveType = moveType;
  projectile->enabled = true;
}

static void projectile_update(GAME_PROJECTILE* projectile) {
  if (projectile->moveType == PROJECTILE_MOVE_TYPE_UP) {
    projectile->pos_y = projectile->pos_y - PROJECTILE_SPEED;
  } else {
    projectile->pos_y = projectile->pos_y + PROJECTILE_SPEED;
  }

  int projectile_top = projectile->pos_y;
  int projectile_bottom = projectile_top + PROJECTILE_HEIGHT;
  if (projectile_bottom < 0) {
    projectile_disable(projectile);
  }
  if (projectile_top > DISPLAY_HEIGHT) {
    projectile_disable(projectile);
  }
}

static void projectile_redraw(GAME_PROJECTILE* projectile) {
  al_draw_filled_rectangle(projectile->pos_x, projectile->pos_y,
                           (projectile->pos_x) + PROJECTILE_WIDTH,
                           (projectile->pos_y) + PROJECTILE_HEIGHT,
                           projectile->color);
}

static bool projectile_instance_hit(GAME_PROJECTILE* projectile, float pos_x, float pos_y, int width, int height) {
  float obj_top = pos_y;
  float obj_bottom = obj_top + height;
  float obj_left = pos_x;
  float obj_right = obj_left + width;

  float projectile_top = projectile->pos_y;
  float projectile_bottom = projectile_top + PROJECTILE_HEIGHT;
  float projectile_left = projectile->pos_x;
  float projectile_right = projectile_left + PROJECTILE_WIDTH;

  return (projectile_left <= obj_right && 
          obj_left <= projectile_right &&
          projectile_top <= obj_bottom && 
          obj_top <= projectile_bottom);
}

GAME_PROJECTILE* projectile_add(float pos_x, float pos_y, ALLEGRO_COLOR color, PROJECTILE_MOVE_TYPE moveType) {
  GAME_PROJECTILE* projectile = NULL;
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    projectile = projectiles[i];
    if (!(projectile->enabled)) {
      projectile_enable(projectile, pos_x, pos_y, color, moveType);
      break;
    }
  }
  return projectile;
}

void projectile_remove(GAME_PROJECTILE* projectile) {
  projectile_disable(projectile);
}

bool projectile_enabled(GAME_PROJECTILE* projectile) {
  return projectile->enabled;
}

GAME_PROJECTILE* projectile_hit(float pos_x, float pos_y, int width, int height, PROJECTILE_MOVE_TYPE moveType) {
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = projectiles[i];
    if (projectile->enabled && projectile->moveType == moveType && projectile_instance_hit(projectile, pos_x, pos_y, width, height)) {
      return projectile;
    }
  }
  return NULL;
}

void projectile_engine_init(void) {
  shoot_color = al_map_rgb(255, 255, 255);

  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = (GAME_PROJECTILE*)assert_not_null(malloc(sizeof(GAME_PROJECTILE)), "Projectile");
    projectile_disable(projectile);
    projectiles[i] = projectile;
  }
}

void projectile_engine_destroy(void) {
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = projectiles[i];
    projectiles[i] = NULL;
    free(projectile);
  }
}

void projectile_engine_update(ALLEGRO_TIMER_EVENT event) {
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = projectiles[i];
    if (projectile->enabled) {
      projectile_update(projectile);
    }
  }
}

void projectile_engine_redraw(void) {
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = projectiles[i];
    if (projectile->enabled) {
      projectile_redraw(projectile);
    }
  }
}