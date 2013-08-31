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

#include <stdbool.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"
#include "milestone/util/random.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

#define AZ_AVATAR_RADIUS 20.0

void az_tick_play_state(az_play_state_t *state, double time) {
  // Move avatar:
  state->avatar_velocity = az_vcaplen(state->avatar_velocity, 500.0);
  az_vpluseq(&state->avatar_position,
             az_vmul(state->avatar_velocity, time));

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
  int targets_remaining = 0;
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->wave <= state->current_wave) ++targets_remaining;
  }
  if (targets_remaining == 0) {
    state->bonus_round = true;
    // TODO: add bonus targets
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
      if (target->wave < state->current_wave) {
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
        target->position.x = az_random(10, AZ_SCREEN_WIDTH - 10);
        target->position.y = az_random(30, AZ_SCREEN_HEIGHT - 30);
        --num_new_targets;
        if (num_new_targets <= 0) break;
      }
    }
    state->max_wave_on_board = new_max_wave_on_board;
  }
}

/*===========================================================================*/
