#pragma once

#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#define DISPLAY_WIDTH 390
#define DISPLAY_HEIGHT 480
#define FRAME_COUNT 60
#define REFRESH_RATE (1.0 / FRAME_COUNT)

extern bool done;
extern bool redraw;

extern ALLEGRO_TIMER* timer;
extern ALLEGRO_EVENT_QUEUE* queue;
extern ALLEGRO_DISPLAY* disp;
extern ALLEGRO_FONT* font;

void init_assets(void);
void destroy_assets(void);

void register_events(void);
void unregister_events(void);