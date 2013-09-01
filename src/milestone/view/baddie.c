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

static void draw_baddie(const az_baddie_t *baddie, az_vector_t avatar_position,
                        az_clock_t clock) {
  assert(baddie->kind != AZ_BAD_NOTHING);
  if (baddie->stun > 0.0 && az_clock_mod(2, 2, clock)) return;
  const GLfloat flare = baddie->flare;
  switch (baddie->kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_TANK:
      glRotated(AZ_RAD2DEG(az_vtheta(baddie->velocity)), 0, 0, 1);
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(flare, 0.5, 0.25); glVertex2f(-4, 0);
        glColor3f(0.75f * flare, 0.25, 0.1);
        glVertex2f(14, 8); glVertex2f(11, 11); glVertex2f(-11, 11);
        glVertex2f(-14, 8); glVertex2f(-14, -8); glVertex2f(-11, -11);
        glVertex2f(11, -11); glVertex2f(14, -8); glVertex2f(14, 8);
      } glEnd();
      break;
    case AZ_BAD_GUARD:
      glRotated(AZ_RAD2DEG(az_vtheta(az_vsub(avatar_position,
                                             baddie->position))), 0, 0, 1);
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.25f + 0.75f * flare, 0.5f, 1.0f - flare);
        glVertex2f(0, 0);
        glColor3f(0.15f + 0.5f * flare, 0.25f, 0.2f - 0.2f * flare);
        for (int i = 0; i <= 360; i += 60) {
          glVertex2d(AZ_BADDIE_RADIUS * cos(AZ_DEG2RAD(i)),
                     AZ_BADDIE_RADIUS * sin(AZ_DEG2RAD(i)));
        }
      } glEnd();
      break;
    case AZ_BAD_BASILISK:
      glRotated(AZ_RAD2DEG(az_vtheta(baddie->velocity)), 0, 0, 1);
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.5f + 0.5f * flare, 1.0f - 0.5f * flare, 0.25f);
        glVertex2f(0, 0);
        glColor3f(0.25f + 0.25f * flare, 0.5f - 0.25f * flare, 0.12f);
        glVertex2f(15, 0); glVertex2f(-11, 11);
        glVertex2f(-11, -11); glVertex2f(15, 0);
      } glEnd();
      break;
    case AZ_BAD_FAKER:
      glRotated(AZ_RAD2DEG(az_vtheta(baddie->velocity)), 0, 0, 1);
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(1, 0, 0.5f - 0.4f * flare); glVertex2f(0, 0);
        glColor3f(0.5f, 0.1f, 0.25f - 0.1f * flare);
        glVertex2f(12, 10); glVertex2f(10, 12); glVertex2f(-10, 12);
        glVertex2f(-12, 10); glVertex2f(-12, -10); glVertex2f(-10, -12);
        glVertex2f(10, -12); glVertex2f(12, -10); glVertex2f(12, 10);
      } glEnd();
      break;
    case AZ_BAD_GHOST:
      glBegin(GL_TRIANGLE_FAN); {
        const az_vector_t center =
          az_vcaplen(az_vmul(baddie->velocity, 0.07), AZ_BADDIE_RADIUS - 1);
        glColor4f(1, 1 - flare, 1 - flare, 0.25);
        glVertex2d(center.x, center.y);
        glColor4f(1, 1 - flare, 1 - flare, 0);
        for (int i = 0; i <= 360; i += 30) {
          glVertex2d(AZ_BADDIE_RADIUS * cos(AZ_DEG2RAD(i)),
                     AZ_BADDIE_RADIUS * sin(AZ_DEG2RAD(i)));
        }
      } glEnd();
      break;
  }
}

void az_draw_baddies(const az_play_state_t *state) {
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) continue;
    glPushMatrix(); {
      glTranslated(baddie->position.x, baddie->position.y, 0);
      draw_baddie(baddie, state->avatar_position, state->clock);
    } glPopMatrix();
  }
}

/*===========================================================================*/
