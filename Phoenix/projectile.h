#pragma once
#include <allegro5/color.h>
#include <allegro5/events.h>

#define PROJECTILE_WIDTH 5
#define PROJECTILE_HEIGHT 10

typedef enum PROJECTILE_MOVE_TYPE {
  PROJECTILE_MOVE_TYPE_UP,
  PROJECTILE_MOVE_TYPE_DOWN
} PROJECTILE_MOVE_TYPE;
typedef struct GAME_PROJECTILE GAME_PROJECTILE;

GAME_PROJECTILE* projectile_add(float pos_x, float pos_y, ALLEGRO_COLOR color,
                                PROJECTILE_MOVE_TYPE moveType);

void projectile_remove(GAME_PROJECTILE* projectile);
// only check projectiles which move in the passed direction
GAME_PROJECTILE* projectile_hit(float pos_x, float pos_y, int width, int height,
                                PROJECTILE_MOVE_TYPE moveType);
bool projectile_enabled(GAME_PROJECTILE* projectile);

void projectile_engine_init(void);
void projectile_engine_update(ALLEGRO_TIMER_EVENT event);
void projectile_engine_redraw(void);
void projectile_engine_destroy(void);