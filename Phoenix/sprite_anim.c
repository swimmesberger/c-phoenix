#include "sprite_anim.h"
#include "utils.h"
#include "resources.h"

typedef struct SPRITE_ANIM {
  ALLEGRO_BITMAP* img;
  int img_height;
  int img_width;
  bool img_internal_loaded;
  int sprite_count;
  int sprite_width;
  int frame_tick_expected;
  int frame_idx;
  int frame_tick;
  bool started;
  SPRITE_ANIM_PLAY_MODE play_mode;
} SPRITE_ANIM;


static void sprite_animation_reset(SPRITE_ANIM* sprite) {
  sprite->frame_idx = 0;
  sprite->frame_tick = 0;
  sprite->started = false;
}

SPRITE_ANIM* sprite_animation_load(const char* filename, int sprite_count, int sprite_width, float speed) {
  ALLEGRO_BITMAP* img = al_load_bitmap(filename);
  if (img == NULL) {
    return NULL;
  }
  SPRITE_ANIM* sprite_anim = sprite_animation_create(img, sprite_count, sprite_width, speed);
  sprite_anim->img_internal_loaded = true;
  return sprite_anim;
}

SPRITE_ANIM* sprite_animation_create(ALLEGRO_BITMAP* img, int sprite_count, int sprite_width, float speed) {
  assert(speed <= 1.0f);
  assert(speed >= 0.0f);

  SPRITE_ANIM* sprite_anim = (SPRITE_ANIM*)malloc(sizeof(SPRITE_ANIM));
  if (sprite_anim == NULL) {
    al_destroy_bitmap(img);
    return NULL;
  }
  sprite_anim->img = img;
  sprite_anim->img_internal_loaded = false;
  sprite_anim->img_width = al_get_bitmap_width(img);
  sprite_anim->img_height = al_get_bitmap_height(img);
  sprite_anim->sprite_count = sprite_count;
  sprite_anim->sprite_width = sprite_width;
  sprite_anim->frame_tick_expected = (1.0f - speed) * FRAME_COUNT; // speed goes from 0 - 1, 1 = every tick, 0 = every 60 tick = every second, 0.5f = every half of a second
  sprite_animation_reset(sprite_anim);
  return sprite_anim;
}

int sprite_animation_get_width(SPRITE_ANIM* sprite) {
  return sprite->sprite_width;
}

int sprite_animation_get_height(SPRITE_ANIM* sprite) {
  return sprite->img_height;
}

bool sprite_animation_is_started(SPRITE_ANIM* sprite) {
  return sprite->started;
}

bool sprite_animation_is_visible(SPRITE_ANIM* sprite) {
  return sprite->started;
}

void sprite_animation_start(SPRITE_ANIM* sprite, SPRITE_ANIM_PLAY_MODE play_mode) {
  sprite->play_mode = play_mode;
  sprite->started = true;
}

bool sprite_animation_update(SPRITE_ANIM* sprite) {
  if (!sprite->started) {
    return false;
  }

  sprite->frame_tick += 1;
  if (sprite->frame_tick > sprite->frame_tick_expected) {
    sprite->frame_tick = 0;
    sprite->frame_idx += 1;
    if (sprite->frame_idx >= sprite->sprite_count) {
      if (sprite->play_mode == SPRITE_ANIM_PLAY_MODE_ONCE) {
        sprite_animation_reset(sprite);
        return false;
      }

      sprite->frame_idx = 0;
    }
  }
  return true;
}

void sprite_animation_draw(SPRITE_ANIM* sprite, int pos_x, int pos_y) {
  if (!sprite->started) {
    return;
  }

  int sprite_x_offset = sprite->frame_idx * sprite->sprite_width;
  int sprite_width = sprite_animation_get_width(sprite);
  al_draw_bitmap_region(sprite->img, sprite_x_offset, 0, sprite_width,
                        sprite->img_height, pos_x, pos_y, 0);
}

void sprite_animation_destroy(SPRITE_ANIM** sprite) {
  if ((*sprite)->img_internal_loaded) {
    al_destroy_bitmap((*sprite)->img);
  }
  free(sprite);
  sprite = NULL;
}