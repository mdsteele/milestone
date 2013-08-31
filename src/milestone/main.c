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
#include "milestone/view/string.h"

/*===========================================================================*/

static void draw_screen(void) {
  glColor3f(1, 0, 0);
  az_draw_string(24, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 100, "Hello, world!");
  glColor3f(0, 1, 0);
  glBegin(GL_LINE_LOOP); {
    glVertex2f(1, 1);
    glVertex2f(AZ_SCREEN_WIDTH - 1, 1);
    glVertex2f(AZ_SCREEN_WIDTH - 1, AZ_SCREEN_HEIGHT - 1);
    glVertex2f(1, AZ_SCREEN_HEIGHT - 1);
  } glEnd();
}

int main(int argc, char **argv) {
  az_init_gui(false);

  while (true) {
    // Tick the state and redraw the screen.
    az_start_screen_redraw(); {
      draw_screen();
    } az_finish_screen_redraw();

    // Get and process GUI events.
    az_event_t event;
    while (az_poll_event(&event)) {
      switch (event.kind) {
        case AZ_EVENT_MOUSE_DOWN:
          return EXIT_SUCCESS;
        default: break;
      }
    }
  }

  return EXIT_SUCCESS;
}

/*===========================================================================*/
