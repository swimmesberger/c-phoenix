#include "sprite_anim.h"
#include "utils.h"
#include "resources.h"

typedef struct SPRITE_ANIM {
  ALLEGRO_BITMAP* img;
  int img_height;
  int img_width;
  int sprite_count;
  int* sprite_x_offsets;
  int frame_tick_expected;
  int frame_idx;
  int frame_tick;
  bool started;
  SPRITE_ANIM_PLAY_MODE play_mode;
} SPRITE_ANIM;


SPRITE_ANIM* sprite_animation_load(const char* filename, int sprite_count, int sprite_x_offsets[], float speed) {
  assert(speed <= 1.0f);
  assert(speed >= 0.0f);

  ALLEGRO_BITMAP* img = al_load_bitmap(filename);
  if (img == NULL) {
    return NULL;
  }
  SPRITE_ANIM* sprite_anim = (SPRITE_ANIM*)malloc(sizeof(SPRITE_ANIM));
  if (sprite_anim == NULL) {
    al_destroy_bitmap(img);
    return NULL;
  }
  sprite_anim->img = img;
  sprite_anim->img_width = al_get_bitmap_width(img);
  sprite_anim->img_height = al_get_bitmap_height(img);
  sprite_anim->sprite_count = sprite_count;
  sprite_anim->sprite_x_offsets = sprite_x_offsets;
  sprite_anim->frame_tick_expected = speed * FRAME_COUNT;
  sprite_anim->frame_idx = 0;
  sprite_anim->frame_tick = 0;
  sprite_anim->started = false;
  return sprite_anim;
}

int sprite_animation_get_width(SPRITE_ANIM* sprite) {
  int frame_idx = sprite->frame_idx;
  int sprite_x_offset = sprite->sprite_x_offsets[frame_idx];
  int next_sprite_x_offset = (frame_idx+1) == sprite->sprite_count ? sprite->img_width : sprite->sprite_x_offsets[frame_idx + 1];
  return next_sprite_x_offset - sprite_x_offset;
}

int sprite_animation_get_height(SPRITE_ANIM* sprite) {
  return sprite->img_height;
}

bool sprite_animation_is_started(SPRITE_ANIM* sprite) {
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
        sprite->started = false;
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

  int sprite_x_offset = sprite->sprite_x_offsets[sprite->frame_idx];
  int sprite_width = sprite_animation_get_width(sprite);
  al_draw_bitmap_region(sprite->img, sprite_x_offset, 0, sprite_width,
                        sprite->img_height, pos_x, pos_y, 0);
}

void sprite_animation_destroy(SPRITE_ANIM** sprite) {
  al_destroy_bitmap((*sprite)->img);
  free((*sprite)->sprite_x_offsets);
  free(sprite);
  sprite = NULL;
}