#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>

#include "resources.h"
#include "game.h"
#include "utils.h"

#define RESERVED_SAMPLES 16

static void system_init(void) {
    init_or_exit(al_init(), "allegro");
    init_or_exit(al_install_keyboard(), "keyboard");
    init_or_exit(al_install_mouse(), "mouse");
    init_or_exit(al_install_audio(), "audio");
    init_or_exit(al_init_primitives_addon(), "primitives add-on");
    init_or_exit(al_init_image_addon(), "image add-on");
    init_or_exit(al_init_acodec_addon(), "image add-on");
    init_or_exit(al_init_ttf_addon(), "ttf add-on");

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    al_set_config_value(al_get_system_config(), "trace", "level", "debug");
    al_register_assert_handler(assert_handler);
    al_reserve_samples(RESERVED_SAMPLES);
}

static void main_event_loop(void) {
    ALLEGRO_EVENT event;
    al_start_timer(timer);
    while (true) {
        al_wait_for_event(queue, &event);

        switch (event.type) {
          case ALLEGRO_EVENT_TIMER:
              game_update(event.timer);
              redraw = true;
              break;

          case ALLEGRO_EVENT_MOUSE_AXES: mouse_move(event.mouse); break;
          case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: mouse_down(event.mouse); break;
          case ALLEGRO_EVENT_MOUSE_BUTTON_UP: mouse_up(event.mouse); break;

          case ALLEGRO_EVENT_KEY_DOWN: key_down(event.keyboard); break;
          case ALLEGRO_EVENT_KEY_UP: key_up(event.keyboard); break;
          case ALLEGRO_EVENT_KEY_CHAR: key_char(event.keyboard); break;

          case ALLEGRO_EVENT_DISPLAY_CLOSE: done = true; break;
        }

        if (done) break;

        if (redraw && al_is_event_queue_empty(queue)) {
            game_redraw();
            redraw = false;
        }
    }
}

int main(void) {
    system_init();

    init_assets();
    register_events();

    game_init();
    main_event_loop();

    unregister_events();
    destroy_assets();
    return 0;
}