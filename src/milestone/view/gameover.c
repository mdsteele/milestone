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

#include "milestone/view/gameover.h"

#include <inttypes.h>

#include <GL/gl.h>

#include "milestone/constants.h"
#include "milestone/state/gameover.h"
#include "milestone/util/misc.h"
#include "milestone/view/cursor.h"
#include "milestone/view/string.h"

/*===========================================================================*/

void az_draw_gameover_screen(const az_gameover_state_t *state) {
  // Border:
  glColor3f(0, 0, 1);
  glBegin(GL_LINE_LOOP); {
    glVertex2f(1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, AZ_SCREEN_HEIGHT - 1.5f);
    glVertex2f(1.5f, AZ_SCREEN_HEIGHT - 1.5f);
  } glEnd();
  // Text:
  glColor3f(1, 0, 0);
  az_draw_string(64, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 100, "GAME OVER");
  az_draw_printf(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 250,
                 "On wave %d, with a score of %"PRId64",",
                 state->last_wave, state->score);
  az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 270,
                 "you finally fell behind.");
  // High score:
  if (state->is_high_score) {
    glColor3f(0.5, 0.5, 0.5);
    az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 370,
                   "Please type your name:");
    az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 450,
                   "Press ENTER when done.");
    glColor3f(1, 1, 1);
    az_draw_string(24, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 400,
                   state->name_buffer);
    glBegin(GL_LINE_LOOP); {
      glColor3f(1, 0.5, 0);
      glVertex2f(AZ_SCREEN_WIDTH/2 + 244.5, 393.5);
      glVertex2f(AZ_SCREEN_WIDTH/2 - 244.5, 393.5);
      glVertex2f(AZ_SCREEN_WIDTH/2 - 244.5, 428.5);
      glVertex2f(AZ_SCREEN_WIDTH/2 + 244.5, 428.5);
    } glEnd();
  } else {
    glColor3f(0.5, 0.5, 0.5);
    az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 450,
                   "Press ENTER to return to title.");
  }
  // Cursor:
  az_draw_cursor();
}

/*===========================================================================*/
