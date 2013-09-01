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

#include <inttypes.h>

#include "GL/gl.h"

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/state/target.h"
#include "milestone/gui/event.h"
#include "milestone/util/color.h"
#include "milestone/util/misc.h"
#include "milestone/view/baddie.h"
#include "milestone/view/particle.h"
#include "milestone/view/projectile.h"
#include "milestone/view/string.h"

/*===========================================================================*/

#define STATUS_HEIGHT 20

static void set_color_for_wave(int wave, double alpha) {
  const az_color_t color = az_color_for_wave(wave);
  glColor4ub(color.r, color.g, color.b, alpha * color.a);
}

static void draw_avatar(const az_play_state_t *state) {
  glPushMatrix(); {
    glTranslated(state->avatar_position.x, state->avatar_position.y, 0);
    glColor3f(1, 0, 1);
    glBegin(GL_LINE_LOOP); {
      glVertex2f(10, 0); glVertex2f(0, 10);
      glVertex2f(-10, 0); glVertex2f(0, -10);
    } glEnd();
  } glPopMatrix();
}

static void draw_targets(const az_play_state_t *state) {
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->presence <= 0.0) continue;
    if (target->invisibility >= 1.0) continue;
    glPushMatrix(); {
      glTranslated(target->position.x, target->position.y, 0);
      const az_color_t color = az_target_color(target);
      glColor4ub(color.r, color.g, color.b,
                 (1.0 - target->invisibility) * color.a);
      glBegin(GL_LINE_LOOP); {
        const GLfloat side = 8.0 * target->presence;
        glVertex2f(side, side); glVertex2f(-side, side);
        glVertex2f(-side, -side); glVertex2f(side, -side);
      } glEnd();
    } glPopMatrix();
  }
}

void az_draw_play_screen(const az_play_state_t *state) {
  draw_targets(state);
  az_draw_baddies(state);
  az_draw_projectiles(state);
  draw_avatar(state);
  az_draw_particles(state);

  // Score bar:
  glColor3f(1, 1, 1);
  az_draw_printf(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 3,
                 "%08"PRId64, state->score);
  az_draw_printf(16, AZ_ALIGN_RIGHT, AZ_SCREEN_WIDTH - 10, 3,
                 "Bombs:%d Delays:%d", state->num_bombs, state->num_lives);

  // Timer bar:
  if (state->bonus_round && az_clock_mod(2, 2, state->clock)) {
    glColor3f(1, 1, 1);
  } else {
    set_color_for_wave(state->current_wave, 1);
  }
  az_draw_printf(16, AZ_ALIGN_LEFT, 10, 3, "Wave %d", state->current_wave);
  glBegin(GL_TRIANGLE_STRIP); {
    const GLfloat left = 5.0f;
    const GLfloat right = left + (AZ_SCREEN_WIDTH - 10.0) *
      (state->wave_time_remaining / AZ_SECONDS_PER_WAVE);
    const GLfloat top = AZ_SCREEN_HEIGHT - STATUS_HEIGHT + 3.0f;
    const GLfloat bottom = AZ_SCREEN_HEIGHT - 3.0f;
    glVertex2f(left, top); glVertex2f(right, top);
    glVertex2f(left, bottom); glVertex2f(right, bottom);
  } glEnd();

  // Border:
  set_color_for_wave(state->current_wave + (state->bonus_round ? 1 : 0), 1);
  glBegin(GL_LINE_LOOP); {
    const GLfloat left = 1.5f;
    const GLfloat right = AZ_SCREEN_WIDTH - 1.5f;
    const GLfloat top = STATUS_HEIGHT + 0.5f;
    const GLfloat bottom = AZ_SCREEN_HEIGHT - STATUS_HEIGHT - 0.5f;
    glVertex2f(left, top); glVertex2f(right, top);
    glVertex2f(right, bottom); glVertex2f(left, bottom);
  } glEnd();
}

/*===========================================================================*/
