#include "map.h"
#include "resources.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <allegro5/color.h>
#include <allegro5/allegro_font.h>

#define HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH 40

ALLEGRO_COLOR text_color;

char *full_screen_text = NULL;
int full_screen_text_width = 0;
int full_screen_text_height = 0;

char *full_screen_text_sub = NULL;
int full_screen_text_sub_width = 0;
int full_screen_text_sub_height = 0;

bool game_complete = false;

static char* text_get_printf_cpy(char const *const format, ...) {
  char *text = (char *)assert_not_null(
      malloc(HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH * sizeof(char)),
      "Screen Text");
  va_list args;
  va_start(args, format);
  vsnprintf(text, HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH, format, args);
  va_end(args);
  char *text_cpy = _strdup(text);
  free(text);
  text = NULL;
  return text_cpy;
}

static void map_destroy_text(void) {
  free(full_screen_text);
  full_screen_text = NULL;
  free(full_screen_text_sub);
  full_screen_text_sub = NULL;
}

void map_set_full_screen_text_printf(char const* const format, ...) {
  char *text = (char *)assert_not_null(
      malloc(HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH * sizeof(char)),
      "Screen Text");
  va_list args;
  va_start(args, format);
  vsnprintf(text, HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH, format, args);
  va_end(args);
  char *text_cpy = _strdup(text);
  free(text);
  text = NULL;

  full_screen_text = text_cpy;
  full_screen_text_width = al_get_text_width(retro_font_large, text_cpy);
}


void map_set_full_screen_text_printf_sub(char const* const format, ...) {
  char *text = (char *)assert_not_null(
      malloc(HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH * sizeof(char)),
      "Screen Text");
  va_list args;
  va_start(args, format);
  vsnprintf(text, HEADER_MAXIMUM_FULL_SCREEN_TEXT_LENGTH, format, args);
  va_end(args);
  char *text_cpy = _strdup(text);
  free(text);
  text = NULL;

  full_screen_text_sub = text_cpy;
  full_screen_text_sub_width = al_get_text_width(retro_font_large, text_cpy);
}

void map_init(void) {
  text_color = al_map_rgb(255, 255, 255);
  full_screen_text_height = al_get_font_ascent(retro_font_large);
  full_screen_text_sub_height = full_screen_text_height;
}

void map_redraw(void) {
  int primary_text_y = (DISPLAY_HEIGHT / 2.0f) - full_screen_text_height;
  if (full_screen_text != NULL) {
    al_draw_text(
        retro_font_large, text_color,
        (DISPLAY_WIDTH / 2.0f) - (full_screen_text_width / 2.0f),
        primary_text_y, 0,
        full_screen_text
    );
  }
  if (full_screen_text_sub != NULL) {
    al_draw_text(retro_font_large, text_color,
                 (DISPLAY_WIDTH / 2.0f) - (full_screen_text_width / 2.0f),
                 primary_text_y + full_screen_text_height, 0,
                 full_screen_text_sub);
  }
}

void map_destroy(void) { 
  map_destroy_text(); 
}