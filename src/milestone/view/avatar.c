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

#include "milestone/view/avatar.h"

#include <math.h>

#include <GL/gl.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"

/*===========================================================================*/

static void draw_avatar_internal(az_color_t color) {
  const double outer = AZ_AVATAR_RADIUS;
  const double inner = outer - 3;
  glBegin(GL_TRIANGLE_STRIP); {
    for (int i = 0; i <= 360; i += 30) {
      const double c = cos(AZ_DEG2RAD(i)), s = sin(AZ_DEG2RAD(i));
      glColor3f(0.8, 0.8, 0.8); glVertex2f(inner * c, inner * s);
      glColor3f(0.4, 0.4, 0.4); glVertex2f(outer * c, outer * s);
    }
  } glEnd();
  for (int i = 0; i < 360; i += 120) {
    glBegin(GL_TRIANGLE_STRIP); {
      glColor3f(0.4, 0.4, 0.4); glVertex2f(3, 2); glVertex2f(inner, 1);
      glColor3f(0.8, 0.8, 0.8); glVertex2f(0, 0); glVertex2f(outer, 0);
      glColor3f(0.4, 0.4, 0.4); glVertex2f(3, -2); glVertex2f(inner, -1);
    } glEnd();
    glRotatef(120, 0, 0, 1);
  }
  glBegin(GL_TRIANGLE_FAN); {
    glColor4ub(color.r, color.g, color.b, color.a); glVertex2f(0, 0);
    glColor4ub(color.r / 2, color.g / 2, color.b / 2, color.a);
    for (int i = 0; i <= 360; i += 30) {
      glVertex2d(4 * cos(AZ_DEG2RAD(i)), 4 * sin(AZ_DEG2RAD(i)));
    }
  } glEnd();
}

void az_draw_avatar(const az_play_state_t *state) {
  glPushMatrix(); {
    glTranslated(state->avatar_position.x, state->avatar_position.y, 0);
    glRotated(AZ_RAD2DEG(az_vtheta(state->avatar_velocity)), 0, 0, 1);
    draw_avatar_internal(az_color_for_wave(az_clock_mod(6, 20, state->clock)));
  } glPopMatrix();
}

void az_draw_avatar_icon(int left, int top, int current_wave) {
  glPushMatrix(); {
    glTranslated(left + 8, top + 8, 0);
    const double factor = 8.0 / AZ_AVATAR_RADIUS;
    glScaled(factor, factor, 1);
    draw_avatar_internal(az_color_for_wave(current_wave));
  } glPopMatrix();
}


void az_draw_bomb_icon(int left, int top) {
  glPushMatrix(); {
    glTranslated(left + 8, top + 8, 0);
    for (int j = 0; j < 360; j += 90) {
      glBegin(GL_TRIANGLE_FAN); {
        glColor3f(0.6, 0.6, 0.6); glVertex2f(0, 0); glColor3f(0.3, 0.3, 0.5);
        for (int i = 5; i <= 85; i += 20) {
          glVertex2d(8 * cos(AZ_DEG2RAD(i + j)), 8 * sin(AZ_DEG2RAD(i + j)));
        }
      } glEnd();
    }
    glBegin(GL_TRIANGLE_FAN); {
      glColor3f(1, 0, 0); glVertex2f(0, 0); glColor3f(0.5, 0, 0);
      for (int i = 0; i <= 360; i += 30) {
        glVertex2d(3 * cos(AZ_DEG2RAD(i)), 3 * sin(AZ_DEG2RAD(i)));
      }
    } glEnd();
  } glPopMatrix();
}

/*===========================================================================*/
