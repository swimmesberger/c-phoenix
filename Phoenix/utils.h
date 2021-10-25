#pragma once
#include <stdbool.h>
#include <allegro5/allegro5.h>

#define ARRAY_LENGTH(X) (sizeof(X) / sizeof(X[0]))

void init_or_exit(bool is_initialized, const char* name);
void* assert_not_null(void* ptr, const char* name);
void assert_handler(const char* expr, const char* file, int line, const char* func);

ALLEGRO_COLOR color_random(float min_value);
bool color_equal(ALLEGRO_COLOR a, ALLEGRO_COLOR b);

bool is_within(int val, int min, int max);