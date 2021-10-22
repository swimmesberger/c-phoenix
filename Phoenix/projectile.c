#include "projectile.h"
#include "resources.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_PROJECTILE_COUNT 100
#define PROJECTILE_SPEED 10

typedef struct GAME_PROJECTILE {
  int pos_x;
  int pos_y;
  ALLEGRO_COLOR color;
  PROJECTILE_MOVE_TYPE moveType;
  bool enabled;
} GAME_PROJECTILE;

GAME_PROJECTILE* projectiles[MAX_PROJECTILE_COUNT];
ALLEGRO_COLOR color_white;

static void projectile_disable(GAME_PROJECTILE* projectile) {
  projectile->pos_x = 0;
  projectile->pos_y = 0;
  projectile->color = color_white;
  projectile->moveType = Up;
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
  if (projectile->moveType == Up) {
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

static bool projectile_instance_hit(GAME_PROJECTILE* projectile, int pos_x, int pos_y,
                           int width, int height) {
  int obj_top = pos_y;
  int obj_bottom = obj_top + height;
  int obj_left = pos_x;
  int obj_right = obj_left + width;
  int projectile_top = projectile->pos_y;
  int projectile_bottom = projectile_top + PROJECTILE_HEIGHT;
  int projectile_left = projectile->pos_x;
  int projectile_right = projectile_left + PROJECTILE_WIDTH;

  return (projectile_left <= obj_right && 
          obj_left <= projectile_right &&
          projectile_top <= obj_bottom && 
          obj_top <= projectile_bottom);
}

GAME_PROJECTILE* projectile_add(int pos_x, int pos_y, ALLEGRO_COLOR color, PROJECTILE_MOVE_TYPE moveType) {
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

bool projectile_enabled(GAME_PROJECTILE* projectile) {
  return projectile->enabled;
}

bool projectile_hit(int pos_x, int pos_y, int width, int height) {
  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = projectiles[i];
    if (projectile->enabled && projectile_instance_hit(projectile, pos_x, pos_y, width, height)) {
      return true;
    }
  }
  return false;
}

void projectile_engine_init(void) {
  color_white = al_map_rgb(255, 255, 255);

  for (int i = 0; i < MAX_PROJECTILE_COUNT; i++) {
    GAME_PROJECTILE* projectile = (GAME_PROJECTILE*)malloc(sizeof(GAME_PROJECTILE));
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