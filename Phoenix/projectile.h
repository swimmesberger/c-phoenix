#pragma once

#include <allegro5/allegro5.h>

#define PROJECTILE_WIDTH 5
#define PROJECTILE_HEIGHT 10

typedef enum PROJECTILE_MOVE_TYPE { Up, Down } PROJECTILE_MOVE_TYPE;

void projectile_add(int pos_x, int pos_y, ALLEGRO_COLOR color, PROJECTILE_MOVE_TYPE moveType);
bool projectile_hit(int pos_x, int pos_y, int width, int height);

void projectile_engine_init(void);
void projectile_engine_update(ALLEGRO_TIMER_EVENT event);
void projectile_engine_redraw(void);
void projectile_engine_destroy(void);