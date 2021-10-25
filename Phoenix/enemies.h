#pragma once
#include <allegro5/events.h>

typedef enum LEVEL_TYPE { Level1 } LEVEL_TYPE;

void enemies_init(LEVEL_TYPE levelType);
void enemies_update(ALLEGRO_TIMER_EVENT event);
void enemies_redraw(void);
int enemies_get_count(void);
void enemies_destroy(void);