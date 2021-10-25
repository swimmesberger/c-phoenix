#pragma once
#include <stdbool.h>
#include <allegro5/allegro5.h>

#define ARRAY_LENGTH(X) (sizeof(X) / sizeof(X[0]))

void init_or_exit(bool is_initialized, const char* name);
void* assert_not_null(void* ptr, const char* name);
void assert_handler(const char* expr, const char* file, int line, const char* func);

int random_number(int lower, int upper);