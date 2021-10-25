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