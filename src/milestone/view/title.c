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

#include "GL/gl.h"

#include "milestone/constants.h"
#include "milestone/view/string.h"

/*===========================================================================*/

void az_draw_title_screen(void) {
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
  az_draw_string(40, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 200, "Milestone");
  az_draw_string(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 300,
                 "Click mouse to start");
}

/*===========================================================================*/
