/*=============================================================================
| Copyright 2013 Matthew D. Steele <mdsteele@alum.mit.edu>                    |
|                                                                             |
| This file is part of Milestone.                                             |
|                                                                             |
| Milestone is free software: you can redistribute it and/or modify it under  |
| the terms of the GNU General Public License as published by the Free        |
| Software Foundation, either version 3 of the License, or (at your option)   |
| any later version.                                                          |
|                                                                             |
| Milestone is distributed in the hope that it will be useful, but WITHOUT    |
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       |
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   |
| more details.                                                               |
|                                                                             |
| You should have received a copy of the GNU General Public License along     |
| with Milestone.  If not, see <http://www.gnu.org/licenses/>.                |
=============================================================================*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <SDL/SDL.h>

#include "milestone/constants.h"
#include "milestone/gui/audio.h"
#include "milestone/gui/event.h"
#include "milestone/gui/screen.h"
#include "milestone/state/gameover.h"
#include "milestone/state/highscore.h"
#include "milestone/state/play.h"
#include "milestone/system/resource.h"
#include "milestone/tick/play.h"
#include "milestone/tick/title.h"
#include "milestone/util/random.h"
#include "milestone/view/gameover.h"
#include "milestone/view/play.h"
#include "milestone/view/title.h"

/*===========================================================================*/

static az_highscore_list_t highscore_list;
static az_highscore_t *last_game = NULL;
static az_title_state_t title_state;
static az_play_state_t play_state;
static az_gameover_state_t gameover_state;

static void load_highscore_list(void) {
  const char *data_dir = az_get_app_data_directory();
  if (data_dir != NULL) {
    char path_buffer[strlen(data_dir) + 16u];
    sprintf(path_buffer, "%s/highscores.txt", data_dir);
    if (az_load_highscore_list(path_buffer, &highscore_list)) return;
  }
  az_clear_highscore_list(&highscore_list);
}

static void save_highscore_list(void) {
  const char *data_dir = az_get_app_data_directory();
  if (data_dir != NULL) {
    char path_buffer[strlen(data_dir) + 16u];
    sprintf(path_buffer, "%s/highscores.txt", data_dir);
    if (az_save_highscore_list(&highscore_list, path_buffer)) return;
  }
  fprintf(stderr, "WARNING: Failed to save high scores.\n");
}

static void clear_highscore_list(void) {
  az_clear_highscore_list(&highscore_list);
}

/*===========================================================================*/

typedef enum {
  TITLE_MODE,
  PLAY_MODE,
  GAMEOVER_MODE
} az_mode_t;

static az_mode_t run_title_mode(void) {
  az_init_title_state(&title_state, &highscore_list, last_game);

  while (true) {
    // Tick the state and redraw the screen.
    az_tick_title_state(&title_state, 1.0 / 60.0);
    az_tick_audio_mixer(&title_state.soundboard);
    az_start_screen_redraw(); {
      az_draw_title_screen(&title_state);
    } az_finish_screen_redraw();

    // Get and process GUI events.
    az_event_t event;
    while (az_poll_event(&event)) {
      switch (event.kind) {
        case AZ_EVENT_MOUSE_DOWN:
          return PLAY_MODE;
        default: break;
      }
    }
  }
}

#define MOUSE_NEUTRAL_X (AZ_SCREEN_WIDTH / 2)
#define MOUSE_NEUTRAL_Y (AZ_SCREEN_HEIGHT / 2)

static az_mode_t run_play_mode(void) {
  az_init_play_state(&play_state);
  az_set_mouse_position(MOUSE_NEUTRAL_X, MOUSE_NEUTRAL_Y);

  while (true) {
    // Tick the state and redraw the screen.
    az_tick_play_state(&play_state, 1.0 / 60.0);
    az_tick_audio_mixer(&play_state.soundboard);
    if (play_state.num_lives < 0) return GAMEOVER_MODE;
    az_start_screen_redraw(); {
      az_draw_play_screen(&play_state);
    } az_finish_screen_redraw();

    // Get and process GUI events.
    az_event_t event;
    while (az_poll_event(&event)) {
      switch (event.kind) {
        case AZ_EVENT_KEY_DOWN:
          if (event.key.id == AZ_KEY_ESCAPE) return GAMEOVER_MODE;
          else if (event.key.id == AZ_KEY_SPACE) {
            az_play_apply_spacebar(&play_state);
          }
          break;
        case AZ_EVENT_MOUSE_DOWN:
          az_play_apply_mouse_click(&play_state);
          break;
        case AZ_EVENT_MOUSE_MOVE:
          az_play_apply_mouse_motion(&play_state,
                                     event.mouse.x - MOUSE_NEUTRAL_X,
                                     event.mouse.y - MOUSE_NEUTRAL_Y);
          az_set_mouse_position(MOUSE_NEUTRAL_X, MOUSE_NEUTRAL_Y);
          break;
        default: break;
      }
    }
  }
}

static az_mode_t run_gameover_mode(void) {
  az_init_gameover_state(&gameover_state);
  gameover_state.last_wave = play_state.current_wave;
  gameover_state.score = play_state.score;
  az_highscore_t *entry = az_submit_highscore(
      &highscore_list, gameover_state.score, gameover_state.last_wave);
  gameover_state.is_high_score = (entry != NULL);

  bool done = false;
  while (!done) {
    // Tick the state and redraw the screen.
    az_tick_audio_mixer(&gameover_state.soundboard);
    az_start_screen_redraw(); {
      az_draw_gameover_screen(&gameover_state);
    } az_finish_screen_redraw();

    // Get and process GUI events.
    az_event_t event;
    while (az_poll_event(&event)) {
      switch (event.kind) {
        case AZ_EVENT_KEY_DOWN:
          if (event.key.id == AZ_KEY_RETURN) done = true;
          else if (event.key.id == AZ_KEY_BACKSPACE) {
            if (gameover_state.cursor > 0) {
              gameover_state.name_buffer[--gameover_state.cursor] = '\0';
            }
          } else if (gameover_state.cursor < 20 &&
                     event.key.character > '\0' &&
                     strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz"
                            "0123456789 -_.,?/!@#$%^&*()|~",
                            event.key.character) != NULL) {
            gameover_state.name_buffer[gameover_state.cursor++] =
              event.key.character;
          }
          break;
        case AZ_EVENT_MOUSE_DOWN:
          if (!gameover_state.is_high_score) done = true;
          break;
        default: break;
      }
    }
  }

  if (entry != NULL) {
    entry->name = strdup(gameover_state.cursor == 0 ? "Mysterious Stranger" :
                         gameover_state.name_buffer);
    save_highscore_list();
  }
  last_game = entry;
  return TITLE_MODE;
}

/*===========================================================================*/

int main(int argc, char **argv) {
  atexit(clear_highscore_list);
  az_init_random();
  load_highscore_list();
  az_init_gui(false);

  az_mode_t mode = TITLE_MODE;
  while (true) {
    switch (mode) {
      case TITLE_MODE:    mode = run_title_mode();    break;
      case PLAY_MODE:     mode = run_play_mode();     break;
      case GAMEOVER_MODE: mode = run_gameover_mode(); break;
    }
  }

  return EXIT_SUCCESS;
}

/*===========================================================================*/
