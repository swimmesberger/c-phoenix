#pragma once
#include <allegro5/events.h>

typedef enum LEVEL_TYPE { LEVEL_TYPE_LEVEL_1 } LEVEL_TYPE;

void enemies_init(LEVEL_TYPE levelType);
void enemies_update(ALLEGRO_TIMER_EVENT event);
void enemies_redraw(void);
int enemies_get_count(void);
bool enemies_hit(float pos_x, float pos_y, int width, int height);
void enemies_destroy(void);