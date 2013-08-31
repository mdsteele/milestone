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

#include "milestone/view/play.h"

#include "GL/gl.h"

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/gui/event.h"

/*===========================================================================*/

void az_draw_play_screen(const az_play_state_t *state) {
  // Border:
  glColor3f(0, 1, 0);
  glBegin(GL_LINE_LOOP); {
    glVertex2f(1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, 1.5f);
    glVertex2f(AZ_SCREEN_WIDTH - 1.5f, AZ_SCREEN_HEIGHT - 1.5f);
    glVertex2f(1.5f, AZ_SCREEN_HEIGHT - 1.5f);
  } glEnd();
  // Cursor:
  int mouse_x, mouse_y;
  if (az_get_mouse_position(&mouse_x, &mouse_y)) {
    glColor3f(0, 1, 1);
    glBegin(GL_LINES); {
      glVertex2f(mouse_x - 5, mouse_y); glVertex2f(mouse_x + 5, mouse_y);
      glVertex2f(mouse_x, mouse_y - 5); glVertex2f(mouse_x, mouse_y + 5);
    } glEnd();
  }
  // Avatar:
  glPushMatrix(); {
    glTranslated(state->avatar_position.x, state->avatar_position.y, 0);
    glColor3f(1, 0, 1);
    glBegin(GL_LINE_LOOP); {
      glVertex2f(10, 0); glVertex2f(0, 10);
      glVertex2f(-10, 0); glVertex2f(0, -10);
    } glEnd();
  } glPopMatrix();
}

/*===========================================================================*/
