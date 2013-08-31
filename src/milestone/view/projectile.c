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

#include "milestone/view/projectile.h"

#include <assert.h>
#include <math.h>

#include <GL/gl.h>

#include "milestone/state/play.h"
#include "milestone/util/clock.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

static void draw_projectile(const az_projectile_t *proj, az_clock_t clock) {
  assert(proj->kind != AZ_PROJ_NOTHING);
  switch (proj->kind) {
    case AZ_PROJ_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_PROJ_BULLET:
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.7, 0.7, 0.7); glVertex2f(0, 0); glColor3f(0.4, 0.4, 0.4);
        const double radius = 3.0;
        for (int i = 0; i <= 360; i += 30) {
          glVertex2d(radius * cos(AZ_DEG2RAD(i)), radius * sin(AZ_DEG2RAD(i)));
        }
      } glEnd();
      break;
    case AZ_PROJ_TANK_SHELL:
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.5, 0.5, 0.5); glVertex2f(0, 0); glColor3f(0, 0.5, 0);
        const double radius = 5.0;
        for (int i = 0; i <= 360; i += 30) {
          glVertex2d(radius * cos(AZ_DEG2RAD(i)), radius * sin(AZ_DEG2RAD(i)));
        }
      } glEnd();
      break;
  }
}

void az_draw_projectiles(const az_play_state_t *state) {
  AZ_ARRAY_LOOP(proj, state->projectiles) {
    if (proj->kind == AZ_PROJ_NOTHING) continue;
    glPushMatrix(); {
      glTranslated(proj->position.x, proj->position.y, 0);
      draw_projectile(proj, state->clock);
    } glPopMatrix();
  }
}

/*===========================================================================*/
