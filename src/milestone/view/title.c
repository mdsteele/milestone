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

#include "milestone/view/title.h"

#include <inttypes.h>

#include <GL/gl.h>

#include "milestone/constants.h"
#include "milestone/state/highscore.h"
#include "milestone/state/title.h"
#include "milestone/util/misc.h"
#include "milestone/view/string.h"

/*===========================================================================*/

void az_draw_title_screen(const az_title_state_t *state) {
  // Border:
  glColor3f(0, 0, 1);
  glBegin(GL_LINE_LOOP); {
    glVertex2f(1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, AZ_SCREEN_HEIGHT - 1.5f);
    glVertex2f(1.5f, AZ_SCREEN_HEIGHT - 1.5f);
  } glEnd();
  // Title:
  glColor3f(1, 0, 0);
  az_draw_string(64, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 50, "Milestone");
  az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 450,
                 "Click mouse to start");
  // High scores:
  int top = 400;
  AZ_ARRAY_LOOP(entry, state->highscore_list->entries) {
    if (entry == state->last_game && az_clock_mod(2, 3, state->clock)) {
      glColor3f(1, 1, 0);
    } else glColor3f(0.5, 0.5, 0.5);
    az_draw_printf(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, top,
                   "%20s %3d  %08"PRId64,
                   (entry->name == NULL ? "--------------------" :
                    entry->name), entry->wave, entry->score);
    top -= 20;
  }
  glColor3f(1, 0, 0);
  az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, top - 6,
                 "        Name         Wave   Score ");
}

/*===========================================================================*/
