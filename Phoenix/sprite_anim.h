#pragma once

#include <allegro5/bitmap.h>

typedef struct SPRITE_ANIM SPRITE_ANIM;

typedef enum SPRITE_ANIM_PLAY_MODE {
  SPRITE_ANIM_PLAY_MODE_ONCE,
  SPRITE_ANIM_PLAY_MODE_LOOP,
} SPRITE_ANIM_PLAY_MODE;

SPRITE_ANIM* sprite_animation_load(const char* filename, int sprite_count, int sprite_width, float speed);
SPRITE_ANIM* sprite_animation_create(ALLEGRO_BITMAP* img, int sprite_count, int sprite_width, float speed);
int sprite_animation_get_width(SPRITE_ANIM* sprite);
int sprite_animation_get_height(SPRITE_ANIM* sprite);
bool sprite_animation_is_started(SPRITE_ANIM* sprite);
void sprite_animation_start(SPRITE_ANIM* sprite, SPRITE_ANIM_PLAY_MODE play_mode);
bool sprite_animation_update(SPRITE_ANIM* sprite);
void sprite_animation_draw(SPRITE_ANIM* sprite, int pos_x, int pos_y);
void sprite_animation_destroy(SPRITE_ANIM** sprite);