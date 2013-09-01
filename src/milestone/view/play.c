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
#include <math.h>
#include <string.h>

#include "GL/gl.h"

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/state/target.h"
#include "milestone/gui/event.h"
#include "milestone/util/color.h"
#include "milestone/util/misc.h"
#include "milestone/view/avatar.h"
#include "milestone/view/baddie.h"
#include "milestone/view/particle.h"
#include "milestone/view/projectile.h"
#include "milestone/view/string.h"
#include "milestone/view/target.h"

/*===========================================================================*/

#define STATUS_HEIGHT 20

static void set_color_for_wave(int wave) {
  const az_color_t color = az_color_for_wave(wave);
  glColor4ub(color.r, color.g, color.b, color.a);
}

static void flash_screen(az_color_t color, double alpha) {
  glBegin(GL_TRIANGLE_STRIP); {
    glColor4ub(color.r, color.g, color.b, color.a * alpha);
    glVertex2f(0, 0); glVertex2f(AZ_SCREEN_WIDTH, 0);
    glVertex2f(0, AZ_SCREEN_HEIGHT);
    glVertex2f(AZ_SCREEN_WIDTH, AZ_SCREEN_HEIGHT);
  } glEnd();
}

static void draw_message(const az_message_t *message, int top,
                         az_clock_t clock) {
  if (message->time_remaining <= 0.0) return;
  const GLfloat alpha = fmin(1.0, message->time_remaining);
  glPushMatrix(); {
    glTranslated(AZ_SCREEN_WIDTH / 2, top, 0);
    glBegin(GL_TRIANGLE_STRIP); {
      const int length = strlen(message->text);
      const GLfloat semiwidth = 8 * length + 4;
      glColor4f(0, 0, 0, 0.6 * alpha);
      glVertex2f(-semiwidth, 0); glVertex2f(semiwidth, 0);
      glVertex2f(-semiwidth, 20); glVertex2f(semiwidth, 20);
    } glEnd();
    if (message->flash && az_clock_mod(2, 2, clock)) {
      glColor4f(1, 1, 0, alpha);
    } else glColor4f(1, 1, 1, alpha);
    az_draw_string(16, AZ_ALIGN_CENTER, 0, 3, message->text);
  } glPopMatrix();
}

static void draw_hud(const az_play_state_t *state) {
  // Score bar:
  if (state->bonus_round && az_clock_mod(2, 2, state->clock)) {
    glColor3f(1, 1, 1);
  } else set_color_for_wave(state->current_wave);
  az_draw_printf(16, AZ_ALIGN_LEFT, 10, 3, "Wave %d", state->current_wave);
  glColor3f(1, 1, 1);
  az_draw_printf(16, AZ_ALIGN_CENTER, AZ_SCREEN_WIDTH/2, 3,
                 "%08"PRId64, state->score);

  // Draw bombs:
  for (int i = 0; i < 3 && i < state->num_bombs; ++i) {
    az_draw_bomb_icon(AZ_SCREEN_WIDTH - 134 - 18 * i, 2);
  }
  if (state->num_bombs > 3) {
    glColor3f(1, 1, 1);
    az_draw_printf(16, AZ_ALIGN_RIGHT, AZ_SCREEN_WIDTH - 171, 3, "%d",
                   state->num_bombs);
  }

  // Draw lives:
  for (int i = 0; i < 3 && i < state->num_lives; ++i) {
    az_draw_avatar_icon(AZ_SCREEN_WIDTH - 18 * (i + 1), 2,
                        state->current_wave);
  }
  if (state->num_lives > 3) {
    glColor3f(1, 1, 1);
    az_draw_printf(16, AZ_ALIGN_RIGHT, AZ_SCREEN_WIDTH - 55, 3, "%d",
                   state->num_lives);
  }

  // Timer bar:
  glBegin(GL_TRIANGLE_STRIP); {
    az_color_t color = {255, 255, 255, 255};
    if (!state->bonus_round || az_clock_mod(2, 2, state->clock)) {
      color = az_color_for_wave(state->current_wave);
    }
    const GLfloat left = 2.0f;
    const GLfloat right = left + (AZ_SCREEN_WIDTH - 2 * left) *
      (state->wave_time_remaining / AZ_SECONDS_PER_WAVE);
    const GLfloat top = AZ_SCREEN_HEIGHT - STATUS_HEIGHT + 3.0f;
    const GLfloat bottom = AZ_SCREEN_HEIGHT - 3.0f;
    const GLfloat mid = (top + bottom) * 0.5f;
    glColor4ub(color.r, color.g, color.b, 0.5 * color.a);
    glVertex2f(left, top); glVertex2f(right, top);
    glColor4ub(color.r, color.g, color.b, color.a);
    glVertex2f(left, mid); glVertex2f(right, mid);
    glColor4ub(color.r, color.g, color.b, 0.5 * color.a);
    glVertex2f(left, bottom); glVertex2f(right, bottom);
  } glEnd();

  // Border:
  set_color_for_wave(state->current_wave + (state->bonus_round ? 1 : 0));
  glBegin(GL_LINE_LOOP); {
    const GLfloat left = 1.5f;
    const GLfloat right = AZ_SCREEN_WIDTH - 1.5f;
    const GLfloat top = STATUS_HEIGHT + 0.5f;
    const GLfloat bottom = AZ_SCREEN_HEIGHT - STATUS_HEIGHT - 0.5f;
    glVertex2f(left, top); glVertex2f(right, top);
    glVertex2f(right, bottom); glVertex2f(left, bottom);
  } glEnd();

  // Messages:
  draw_message(&state->tutorial_message, STATUS_HEIGHT + 4, state->clock);
  draw_message(&state->status_message, AZ_SCREEN_HEIGHT - STATUS_HEIGHT - 24,
               state->clock);
}

/*===========================================================================*/

void az_draw_play_screen(const az_play_state_t *state) {
  az_draw_targets(state);
  az_draw_baddies(state);
  az_draw_projectiles(state);
  az_draw_avatar(state);
  az_draw_particles(state);
  if (state->gained_life_flash > 0.0) {
    flash_screen((az_color_t){255, 255, 255, 128}, state->gained_life_flash);
  }
  if (state->lost_life_flash > 0.0) {
    flash_screen((az_color_t){255, 0, 0, 64}, state->lost_life_flash);
  }
  if (state->bonus_flash > 0.0) {
    flash_screen((az_color_t){255, 255, 128, 64}, state->bonus_flash);
  }
  draw_hud(state);
}

/*===========================================================================*/
