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

#include "milestone/tick/play.h"

#include <math.h>
#include <stdbool.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"
#include "milestone/util/random.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

#define AZ_AVATAR_RADIUS 20.0

#define EDGE_ELASTICITY 0.5

void az_tick_play_state(az_play_state_t *state, double time) {
  ++state->clock;

  // Move avatar:
  state->avatar_velocity = az_vcaplen(state->avatar_velocity, 500.0);
  az_vpluseq(&state->avatar_position,
             az_vmul(state->avatar_velocity, time));
  // Bounce off edges of board:
  if (state->avatar_position.x < AZ_BOARD_MIN_X) {
    state->avatar_position.x = AZ_BOARD_MIN_X;
    if (state->avatar_velocity.x < 0) {
      state->avatar_velocity.x = -EDGE_ELASTICITY * state->avatar_velocity.x;
    }
  }
  if (state->avatar_position.x > AZ_BOARD_MAX_X) {
    state->avatar_position.x = AZ_BOARD_MAX_X;
    if (state->avatar_velocity.x > 0) {
      state->avatar_velocity.x = -EDGE_ELASTICITY * state->avatar_velocity.x;
    }
  }
  if (state->avatar_position.y < AZ_BOARD_MIN_Y) {
    state->avatar_position.y = AZ_BOARD_MIN_Y;
    if (state->avatar_velocity.y < 0) {
      state->avatar_velocity.y = -EDGE_ELASTICITY * state->avatar_velocity.y;
    }
  }
  if (state->avatar_position.y > AZ_BOARD_MAX_Y) {
    state->avatar_position.y = AZ_BOARD_MAX_Y;
    if (state->avatar_velocity.y > 0) {
      state->avatar_velocity.y = -EDGE_ELASTICITY * state->avatar_velocity.y;
    }
  }

  // Collect targets:
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (az_vwithin(target->position, state->avatar_position,
                   AZ_AVATAR_RADIUS)) {
      target->kind = AZ_TARG_NOTHING;
      state->score += 250;
    }
  }

  // If all of this wave's targets are collected, go to the bonus round.
  int current_wave_targets_remaining = 0;
  int next_wave_targets_remaining = 0;
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->kind == AZ_TARG_BONUS) continue;
    if (target->wave <= state->current_wave) {
      ++current_wave_targets_remaining;
    } else if (target->wave == state->current_wave + 1) {
      ++next_wave_targets_remaining;
    }
  }
  if (state->current_wave > 0 && current_wave_targets_remaining == 0) {
    if (next_wave_targets_remaining == 0) {
      ++state->num_lives;
      state->wave_time_remaining = 0.0;
      ++state->current_wave;
    } else if (!state->bonus_round) {
      state->bonus_round = true;
      int num_bonus_targets = 8 + state->current_wave / 2;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind != AZ_TARG_NOTHING) continue;
        target->kind = AZ_TARG_BONUS;
        target->wave = state->current_wave;
        target->position.x =
          az_random(AZ_BOARD_MIN_X + 10, AZ_BOARD_MAX_X - 10);
        target->position.y =
          az_random(AZ_BOARD_MIN_Y + 10, AZ_BOARD_MAX_Y - 10);
        --num_bonus_targets;
        if (num_bonus_targets <= 0) break;
      }
    }
  }

  // Check for wave being over:
  state->wave_time_remaining -= time;
  if (state->wave_time_remaining <= 0.0) {
    ++state->current_wave;
    state->wave_time_remaining = AZ_SECONDS_PER_WAVE;
    state->bonus_round = false;

    bool delayed = false;
    AZ_ARRAY_LOOP(target, state->targets) {
      if (target->kind == AZ_TARG_NOTHING) continue;
      if (target->kind == AZ_TARG_BONUS) {
        target->kind = AZ_TARG_NOTHING;
      } else if (target->wave < state->current_wave) {
        target->wave = state->current_wave;
        delayed = true;
      }
    }
    if (delayed) {
      --state->num_lives;
      // TODO: check for game over
    }
    const int new_max_wave_on_board = state->current_wave +
      az_num_waves_at_once_for_wave(state->current_wave) - 1;
    for (int wave = state->max_wave_on_board + 1;
         wave <= new_max_wave_on_board; ++wave) {
      int num_new_targets = 5 + wave;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind != AZ_TARG_NOTHING) continue;
        target->kind = AZ_TARG_RUN_OVER;
        target->wave = wave;
        target->position.x =
          az_random(AZ_BOARD_MIN_X + 10, AZ_BOARD_MAX_X - 10);
        target->position.y =
          az_random(AZ_BOARD_MIN_Y + 10, AZ_BOARD_MAX_Y - 10);
        --num_new_targets;
        if (num_new_targets <= 0) break;
      }
    }
    state->max_wave_on_board = new_max_wave_on_board;
  }
}

/*===========================================================================*/
