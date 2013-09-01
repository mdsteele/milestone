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

#include "milestone/state/play.h"

#include <assert.h>
#include <string.h>

#include "milestone/constants.h"
#include "milestone/state/baddie.h"
#include "milestone/state/particle.h"
#include "milestone/state/target.h"
#include "milestone/util/audio.h"
#include "milestone/util/misc.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

void az_init_play_state(az_play_state_t *state) {
  memset(state, 0, sizeof(*state));
  state->avatar_position.x = AZ_SCREEN_WIDTH / 2;
  state->avatar_position.y = AZ_SCREEN_HEIGHT / 2;
  state->avatar_velocity = AZ_VZERO;
  state->num_lives = 3;
  state->num_bombs = 3;
}

int az_num_waves_at_once_for_wave(int wave) {
  if (wave >= 20) return 4;
  else if (wave >= 10) return 3;
  else return 2;
}

void az_add_baddie(az_play_state_t *state, az_baddie_kind_t kind,
                   az_vector_t position) {
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) {
      az_init_baddie(baddie, kind, position);
      return;
    }
  }
}

void az_add_particle(az_play_state_t *state, az_color_t color,
                     az_vector_t position, az_vector_t velocity,
                     double lifetime) {
  AZ_ARRAY_LOOP(particle, state->particles) {
    if (particle->kind == AZ_PAR_NOTHING) {
      az_init_particle(particle, color, position, velocity, lifetime);
      return;
    }
  }
}

void az_add_projectile(az_play_state_t *state, az_proj_kind_t kind,
                       az_vector_t position, az_vector_t velocity) {
  AZ_ARRAY_LOOP(proj, state->projectiles) {
    if (proj->kind == AZ_PROJ_NOTHING) {
      memset(proj, 0, sizeof(*proj));
      proj->kind = kind;
      proj->position = position;
      proj->velocity = velocity;
      return;
    }
  }
}

void az_add_target(az_play_state_t *state, az_target_kind_t kind,
                   int wave, az_vector_t position) {
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) {
      az_init_target(target, kind, wave, position);
      return;
    }
  }
}

void az_set_message(az_message_t *message, double seconds, bool flash,
                    const char *text) {
  message->time_remaining = seconds;
  message->flash = flash;
  message->text = text;
}

void az_award_points(az_play_state_t *state, int64_t points) {
  assert(points >= 0);
  const int64_t old_score = state->score;
  state->score += points;
  const int64_t new_score = state->score;
  if (new_score / AZ_POINTS_PER_BOMB > old_score / AZ_POINTS_PER_BOMB) {
    ++state->num_bombs;
    az_set_message(&state->status_message, 2.5, true, "Extra bomb earned!");
    az_play_sound(&state->soundboard, AZ_SND_GAIN_BOMB);
  }
}

#define EDGE_ELASTICITY 0.5

void az_bounce_off_edges(az_vector_t *position, az_vector_t *velocity,
                         double radius) {
  if (position->x < AZ_BOARD_MIN_X + radius) {
    position->x = AZ_BOARD_MIN_X + radius;
    if (velocity->x < 0) velocity->x = -EDGE_ELASTICITY * velocity->x;
  }
  if (position->x > AZ_BOARD_MAX_X - radius) {
    position->x = AZ_BOARD_MAX_X - radius;
    if (velocity->x > 0) velocity->x = -EDGE_ELASTICITY * velocity->x;
  }
  if (position->y < AZ_BOARD_MIN_Y + radius) {
    position->y = AZ_BOARD_MIN_Y + radius;
    if (velocity->y < 0) velocity->y = -EDGE_ELASTICITY * velocity->y;
  }
  if (position->y > AZ_BOARD_MAX_Y - radius) {
    position->y = AZ_BOARD_MAX_Y - radius;
    if (velocity->y > 0) velocity->y = -EDGE_ELASTICITY * velocity->y;
  }
}

/*===========================================================================*/
