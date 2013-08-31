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

#include "milestone/view/baddie.h"

#include <assert.h>
#include <math.h>

#include <GL/gl.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/clock.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

static void draw_baddie(const az_baddie_t *baddie, az_clock_t clock) {
  assert(baddie->kind != AZ_BAD_NOTHING);
  switch (baddie->kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_TANK:
      glColor3f(0, 0.5, 0.25);
      glBegin(GL_LINE_LOOP); {
        glVertex2f(3, 10); glVertex2f(-3, 10);
        glVertex2f(-10, 3); glVertex2f(-10, -3);
        glVertex2f(-3, -10); glVertex2f(3, -10);
        glVertex2f(10, -3); glVertex2f(10, 3);
      } glEnd();
      break;
    case AZ_BAD_GUARD:
      glColor3f(0.25, 0.5, 1);
      glBegin(GL_LINE_LOOP); {
        for (int i = 0; i < 360; i += 30) {
          glVertex2d(AZ_BADDIE_RADIUS * cos(AZ_DEG2RAD(i)),
                     AZ_BADDIE_RADIUS * sin(AZ_DEG2RAD(i)));
        }
      } glEnd();
      break;
    case AZ_BAD_BASILISK:
      glColor3f(0.5, 1.0, 0.25);
      glBegin(GL_LINE_LOOP); {
        glVertex2f(3, 10); glVertex2f(-3, 10);
        glVertex2f(-10, 3); glVertex2f(-10, -3);
        glVertex2f(-3, -10); glVertex2f(3, -10);
        glVertex2f(10, -3); glVertex2f(10, 3);
      } glEnd();
      break;
  }
}

void az_draw_baddies(const az_play_state_t *state) {
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) continue;
    glPushMatrix(); {
      glTranslated(baddie->position.x, baddie->position.y, 0);
      draw_baddie(baddie, state->clock);
    } glPopMatrix();
  }
}

/*===========================================================================*/
