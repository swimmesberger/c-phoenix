// file: src/resources.c
#include "resources.h"

#include "utils.h"

bool done = false;
bool redraw = true;

ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* queue;
ALLEGRO_DISPLAY* disp;
ALLEGRO_FONT* font;

ALLEGRO_BITMAP* background_img;
ALLEGRO_BITMAP* ship_img;

void init_assets(void) {
	timer = assert_not_null(al_create_timer(REFRESH_RATE), "timer");
	queue = assert_not_null(al_create_event_queue(), "event queue");
	disp = assert_not_null(al_create_display(WIDTH, HEIGHT), "display");
	font = assert_not_null(al_create_builtin_font(), "built-in font");

  background_img = assert_not_null(al_load_bitmap("background.jpg"), "Background Image");
  ship_img = assert_not_null(al_load_bitmap("ship.png"), "Ship Image");
}

void destroy_assets(void) {
  al_destroy_bitmap(ship_img);
  al_destroy_bitmap(background_img);
	al_destroy_font(font);
	al_destroy_display(disp);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
}

void register_events(void) {
	al_register_event_source(queue, al_get_timer_event_source(timer));
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(disp));
}

void unregister_events(void) {
	al_unregister_event_source(queue, al_get_display_event_source(disp));
	al_unregister_event_source(queue, al_get_mouse_event_source());
	al_unregister_event_source(queue, al_get_keyboard_event_source());
	al_unregister_event_source(queue, al_get_timer_event_source(timer));
}