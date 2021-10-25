#include "header.h"
#include "resources.h"
#include "player.h"
#include "utils.h"
#include "enemies.h"

#include <stdio.h>
#include <stdlib.h>

ALLEGRO_COLOR primary_text_color;
ALLEGRO_COLOR secondary_text_color;
ALLEGRO_BITMAP *life_img;

char const *score_text = "SCORE";
char const *score_text_1 = "1";

int score_number = 0;
char *score_number_text;
int score_number_text_x;

int score_enemies_count = 0;

static char ascii_num_to_char(int num) {
  assert(num >= 0);
  assert(num <= 9);
  // ascii table -> 0 = 48, 9 = 57
  return (char)(num + 48);
}

void header_init(void) {
  primary_text_color = al_map_rgb(0, 255, 173);
  secondary_text_color = al_map_rgb(237, 0, 0);
  score_number_text_x = al_get_text_width(retro_font_small, score_text) + 10;
  life_img = assert_not_null(al_load_bitmap("sprites/ship_life.png"), "Ship Life Image");
  score_enemies_count = enemies_get_count();
  score_number_text = (char *)assert_not_null(malloc(7 * sizeof(char)), "Score Memory");
  for (int i = 0; i < 6; i++) {
    score_number_text[i] = '0';
  }
  score_number_text[6] = '\0';
}

void header_update(ALLEGRO_TIMER_EVENT event) {
  int current_enemies = enemies_get_count();
  int enemies_killed = score_enemies_count - current_enemies;
  if (enemies_killed > 0) {
    score_enemies_count = current_enemies;
    score_number += enemies_killed * 100;
    snprintf(score_number_text, 7, "%06d", score_number);
    score_number_text[6] = '\0';
  }
}

int header_get_score(void) {
  return score_number;
}

void header_redraw(void) {
  al_draw_text(retro_font_small, primary_text_color, 10, 0, 0, score_text);
  al_draw_text(retro_font_small, secondary_text_color, score_number_text_x, 0, 0, score_text_1);
  al_draw_text(retro_font_small, secondary_text_color, 10, 15, 0, score_number_text);
  al_draw_bitmap(life_img, 35, 35, 0);
  char player_life[] = {ascii_num_to_char(player_get_life()), '\0'};
  al_draw_text(retro_font_small, secondary_text_color, 55, 33, 0, player_life);
}

void header_destroy(void) {
  char *text = score_number_text;
  score_number_text = NULL;
  free(text);
}