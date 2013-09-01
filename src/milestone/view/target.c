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

static void draw_target(const az_target_t *target, az_clock_t clock) {
  const GLfloat alpha = 1.0 - target->invisibility;
  if (target->kind == AZ_TARG_BONUS) {
    glRotated(2 * az_clock_mod(360, 1, clock), 0, 0, 1);
    glBegin(GL_TRIANGLE_FAN); {
      glColor4f(1, 1, 1, 0);
      glVertex2f(0, 0);
      switch (az_clock_mod(6, 1, clock)) {
        case 0: glColor4f(1.0, 0.5, 0.5, alpha); break;
        case 1: glColor4f(1.0, 1.0, 0.5, alpha); break;
        case 2: glColor4f(0.5, 1.0, 0.5, alpha); break;
        case 3: glColor4f(0.5, 1.0, 1.0, alpha); break;
        case 4: glColor4f(0.5, 0.5, 1.0, alpha); break;
        case 5: glColor4f(1.0, 0.5, 1.0, alpha); break;
      }
      for (int i = 0; i <= 10; ++i) {
        const double radius = target->presence *
          (i % 2 ? AZ_TARGET_RADIUS + 2 : AZ_TARGET_RADIUS - 2);
        const double theta = AZ_DEG2RAD(i * 36);
        glVertex2d(radius * cos(theta), radius * sin(theta));
      }
    } glEnd();
  } else {
    glRotated(120 * sin(AZ_DEG2RAD(2 * az_clock_mod(360, 1, clock))), 0, 0, 1);
    glBegin(GL_TRIANGLE_FAN); {
      glColor4f(1, 1, 1, 0);
      glVertex2f(0, 0);
      const az_color_t color = az_target_color(target);
      glColor4ub(color.r, color.g, color.b, alpha * color.a);
      const GLfloat side = 8.0 * target->presence;
      glVertex2f(side, side); glVertex2f(-side, side);
      glVertex2f(-side, -side); glVertex2f(side, -side);
      glVertex2f(side, side);
    } glEnd();
  }
}

void az_draw_targets(const az_play_state_t *state) {
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->presence <= 0.0) continue;
    if (target->invisibility >= 1.0) continue;
    glPushMatrix(); {
      glTranslated(target->position.x, target->position.y, 0);
      draw_target(target, state->clock + 25 * target->wave);
    } glPopMatrix();
  }
}

/*===========================================================================*/
