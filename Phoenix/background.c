#include "background.h"
#include "utils.h"
#include "resources.h"
#include <allegro5/allegro_audio.h>

#define BACKGROUND_SOUND_GAIN 0.5f

ALLEGRO_BITMAP* background_img;
ALLEGRO_SAMPLE* background_sound;

float background_pos_y;
float background_height;

void background_init(void) {
  background_img = assert_not_null(al_load_bitmap("sprites/background.jpg"), "Background Image");
  background_height = al_get_bitmap_height(background_img);
  background_pos_y = 0;
  background_sound = assert_not_null(al_load_sample("sounds/PHOENIX2.WAV"), "Background Sound");
  al_play_sample(background_sound, BACKGROUND_SOUND_GAIN, 0.5f, 1.0f, ALLEGRO_PLAYMODE_LOOP, NULL);
}

void background_update(ALLEGRO_TIMER_EVENT event) {
  background_pos_y += 1;
  if (background_pos_y > background_height) {
    background_pos_y = 0;
  }
}

void background_redraw(void) {
  al_draw_bitmap(background_img, 0, background_pos_y - background_height, 0);
  al_draw_bitmap(background_img, 0, background_pos_y, 0);
  al_draw_bitmap(background_img, 0, background_pos_y + background_height, 0);
}

void background_destroy(void) {
  al_destroy_bitmap(background_img);
  al_destroy_sample(background_sound);
}