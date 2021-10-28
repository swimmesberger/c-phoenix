#include "explosions.h"
#include "utils.h"
#include "sprite_anim.h"

#include <allegro5/allegro_audio.h>
#include <stdlib.h>

// we pre-init an array of explosions - this is the maximum possible amount
#define MAX_EXPLOSION_COUNT 20
#define EXPLOSION_SPEED 0.8f
#define EXPLOSION_FRAME_COUNT 3

struct EXPLOSION {
  float pos_x;
  float pos_y;
  bool enabled;
  SPRITE_ANIM* explosion_sprite;
};

ALLEGRO_SAMPLE* explosion_sound;
ALLEGRO_BITMAP* explosion_img;
EXPLOSION* explosions[MAX_EXPLOSION_COUNT];

static void explosion_disable(EXPLOSION* explosion) {
  explosion->pos_x = 0;
  explosion->pos_y = 0;
  explosion->enabled = false;
}

static void explosion_enable(EXPLOSION* explosion, int pos_x, int pos_y) {
  explosion->pos_x = pos_x;
  explosion->pos_y = pos_y;
  explosion->enabled = true;
}

EXPLOSION* explosion_add(float pos_x, float pos_y) {
  EXPLOSION* explosion = NULL;
  for (int i = 0; i < MAX_EXPLOSION_COUNT; i++) {
    EXPLOSION* inner_exp = explosions[i];
    if (!(inner_exp->enabled)) {
      explosion = inner_exp;
      break;
    }
  }
  if (explosion != NULL) {
    explosion_enable(explosion, pos_x, pos_y);
    al_play_sample(explosion_sound, 1.0f, 0.5f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
    sprite_animation_start(explosion->explosion_sprite, SPRITE_ANIM_PLAY_MODE_ONCE);
  }
  return explosion;
}

bool explosion_visible(EXPLOSION* explosion) {
  return explosion->enabled;
}

void explosions_init(void) {
  explosion_sound = assert_not_null(al_load_sample("sounds/explosion.WAV"), "Enemy Explostion Sound");
  explosion_img = assert_not_null(al_load_bitmap("sprites/explosion_sprites.png"), "Explosion Image");
  for (int i = 0; i < MAX_EXPLOSION_COUNT; i++) {
    EXPLOSION* explosion = (EXPLOSION*)assert_not_null(malloc(sizeof(EXPLOSION)), "Explosion");
    explosion->explosion_sprite = assert_not_null(sprite_animation_create(explosion_img, EXPLOSION_FRAME_COUNT, EXPLOSION_FRAME_WIDTH, EXPLOSION_SPEED), "Explosion Sprite");
    explosion_disable(explosion);
    explosions[i] = explosion;
  }
}

void explosions_update(ALLEGRO_TIMER_EVENT event) {
  for (int i = 0; i < MAX_EXPLOSION_COUNT; i++) {
    EXPLOSION* explosion = explosions[i];
    if (explosion->enabled) {
      if (!sprite_animation_update(explosion->explosion_sprite)) {
        // when the explosion animation finishes we disable the explosion object so it can be reused again
        explosion_disable(explosion);
      }
    }
  }
}

void explosions_redraw(void) {
  for (int i = 0; i < MAX_EXPLOSION_COUNT; i++) {
    EXPLOSION* explosion = explosions[i];
    if (explosion->enabled) {
      sprite_animation_draw(explosion->explosion_sprite, explosion->pos_x, explosion->pos_y);
    }
  }
}

void explosions_destroy(void) {
  for (int i = 0; i < MAX_EXPLOSION_COUNT; i++) {
    EXPLOSION* explosion = explosions[i];
    sprite_animation_destroy(&explosion->explosion_sprite);
  }
  al_destroy_bitmap(explosion_img);
  al_destroy_sample(explosion_sound);
}