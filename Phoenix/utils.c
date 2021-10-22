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

ALLEGRO_COLOR color_random(float min_value) {
    return al_map_rgb_f(
        (float)rand() / RAND_MAX + min_value,
        (float)rand() / RAND_MAX + min_value,
        (float)rand() / RAND_MAX + min_value);
}


bool color_equal(ALLEGRO_COLOR a, ALLEGRO_COLOR b) {
    return memcmp(&a, &b, sizeof(ALLEGRO_COLOR)) == 0;
}

bool is_within(int val, int min, int max) {
    return min <= val && val <= max;
}