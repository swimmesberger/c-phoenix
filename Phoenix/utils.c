// file: src/utils.c
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <allegro5/allegro5.h>

void init_or_exit(bool is_initialized, const char* name) {
    if (!is_initialized) {
        printf("could not initialize %s: ERRNO %d\n", name, al_get_errno());
        exit(1);
    }
}

void* assert_not_null(void* ptr, const char* name) {
    if (!ptr) {
        printf("could not create %s: ERRNO %d\n", name, al_get_errno());
        exit(1);
    }
    return ptr;
}

void assert_handler(const char* expr, const char* file, int line, const char* func) {
    printf("Expression (%s) failed on line %d of file %s in function %s\n",
        expr, line, file, func);
    assert(false);
}

int random_number(int lower, int upper) {
  return (rand() % (upper - lower + 1)) + lower;
}

bool is_rect_intersect(float x1, float y1, int width1, int height1, float x2, float y2, int width2, int heigth2) {
  float o1_top = y1;
  float o1_bottom = o1_top + height1;
  float o1_left = x1;
  float o1_right = o1_left + width1;

  float o2_top = y2;
  float o2_bottom = o2_top + heigth2;
  float o2_left = x2;
  float o2_right = o2_left + width2;

  return (o1_left <= o2_right && o2_left <= o1_right && o1_top <= o2_bottom && o2_top <= o1_bottom);
}