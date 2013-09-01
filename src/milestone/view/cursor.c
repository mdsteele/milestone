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

#include "milestone/view/cursor.h"

#include <GL/gl.h>

#include "milestone/gui/event.h"

/*===========================================================================*/

void az_draw_cursor(void) {
  int x = 0, y = 0;
  if (!az_get_mouse_position(&x, &y)) return;
  glPushMatrix(); {
    glTranslatef(x + 0.5f, y + 0.5f, 0);
    glColor4f(0, 1, 0, 0.5); // green tint
    glBegin(GL_TRIANGLE_FAN); {
      glVertex2f(0, 0); glVertex2f(15, 15);
      glVertex2f(5, 10); glVertex2f(0, 15);
    } glEnd();
    glColor3f(0, 1, 0); // green
    glBegin(GL_LINE_LOOP); {
      glVertex2f(0, 0); glVertex2f(15, 15);
      glVertex2f(5, 10); glVertex2f(0, 15);
    } glEnd();
  } glPopMatrix();
}

/*===========================================================================*/
