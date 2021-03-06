#pragma once
#include <allegro5/events.h>

#define EXPLOSION_FRAME_WIDTH 52
#define EXPLOSION_FRAME_HEIGHT 47

typedef struct EXPLOSION EXPLOSION;

EXPLOSION* explosion_add(float pos_x, float pos_y);
bool explosion_visible(EXPLOSION* explosion);

void explosions_init(void);
void explosions_update(ALLEGRO_TIMER_EVENT event);
void explosions_redraw(void);
void explosions_destroy(void);