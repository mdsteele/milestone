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
#include "milestone/gui/event.h"
#include "milestone/gui/screen.h"
#include "milestone/state/play.h"
#include "milestone/util/vector.h"
#include "milestone/view/play.h"
#include "milestone/view/title.h"

/*===========================================================================*/

static az_play_state_t play_state;

static void tick_game(double time) {
  play_state.avatar_velocity = az_vcaplen(play_state.avatar_velocity, 500.0);
  az_vpluseq(&play_state.avatar_position,
             az_vmul(play_state.avatar_velocity, time));
}

/*===========================================================================*/

typedef enum {
  TITLE_MODE,
  PLAY_MODE,
  GAMEOVER_MODE
} az_mode_t;

static az_mode_t run_title_mode(void) {
  while (true) {
    // Tick the state and redraw the screen.
    az_start_screen_redraw(); {
      az_draw_title_screen();
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

static az_mode_t run_play_mode(void) {
  az_init_play_state(&play_state);
  az_set_mouse_position(400, 250);

  while (true) {
    // Tick the state and redraw the screen.
    tick_game(1.0 / 60.0);
    az_start_screen_redraw(); {
      az_draw_play_screen(&play_state);
    } az_finish_screen_redraw();

    // Get and process GUI events.
    az_event_t event;
    while (az_poll_event(&event)) {
      switch (event.kind) {
        case AZ_EVENT_MOUSE_DOWN:
          return GAMEOVER_MODE;
        case AZ_EVENT_MOUSE_MOVE: {
          const az_vector_t delta = {event.mouse.x - 400, event.mouse.y - 250};
          az_vpluseq(&play_state.avatar_velocity, az_vmul(delta, 0.5));
          az_set_mouse_position(400, 250);
        } break;
        default: break;
      }
    }
  }
}

static az_mode_t run_gameover_mode(void) {
  // TODO: Implement a game over screen.
  printf("Game over!\n");
  return TITLE_MODE;
}

/*===========================================================================*/

int main(int argc, char **argv) {
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
