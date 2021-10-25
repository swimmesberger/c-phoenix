#pragma once
#include <stdbool.h>

extern bool game_complete;

void map_set_full_screen_text_printf(char const* const format, ...);
void map_set_full_screen_text_printf_sub(char const* const format, ...);
void map_init(void);
void map_redraw(void);
void map_destroy(void);