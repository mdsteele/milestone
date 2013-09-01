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

#include "milestone/tick/target.h"

#include <assert.h>
#include <math.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

#define PRESENT_TIME 0.6
#define FADE_OUT_TIME 0.6
#define FADE_IN_TIME 1.0

static void tick_target(az_play_state_t *state, az_target_t *target,
                        double time) {
  assert(target->kind != AZ_TARG_NOTHING);

  // Update presence:
  if (target->kind == AZ_TARG_UNPLANNED) {
    if (state->current_wave >= target->wave &&
        state->wave_time_remaining <= 0.5 * AZ_SECONDS_PER_WAVE) {
      target->kind = AZ_TARG_NORMAL;
    }
  } else {
    target->presence = fmin(1.0, target->presence + time / PRESENT_TIME);
  }

  // Update invisibility:
  if (target->is_invisible) {
    target->invisibility =
      fmin(1.0, target->invisibility + time / FADE_OUT_TIME);
  } else {
    target->invisibility =
      fmax(0.0, target->invisibility - time / FADE_IN_TIME);
  }
  target->is_invisible = false;

  // Update position:
  az_vpluseq(&target->position, az_vmul(target->velocity, time));
  az_bounce_off_edges(&target->position, &target->velocity);

  // Update velocity:
  const double old_speed = az_vnorm(target->velocity);
  const double new_speed = fmax(0.0, old_speed - 10.0 * time);
  target->velocity = az_vwithlen(target->velocity, new_speed);
  AZ_ARRAY_LOOP(other, state->targets) {
    if (other->kind == AZ_TARG_NOTHING) continue;
    if (other->presence < 0.5) continue;
    if (other == target) continue;
    if (az_vwithin(target->position, other->position, 2 * AZ_TARGET_RADIUS)) {
      az_vpluseq(&target->velocity,
                 az_vwithlen(az_vsub(target->position, other->position),
                             20.0 * time));
    }
  }
  if (target->kind == AZ_TARG_REBEL) {
    if (az_vwithin(state->avatar_position, target->position, 300.0)) {
      az_vpluseq(&target->velocity,
                 az_vwithlen(az_vsub(target->position, state->avatar_position),
                             200.0 * time));
    }
    const az_vector_t goal = {
      (state->avatar_position.x >= AZ_BOARD_CENTER_X ?
       (AZ_BOARD_CENTER_X + AZ_BOARD_MIN_X) / 2 :
       (AZ_BOARD_CENTER_X + AZ_BOARD_MAX_X) / 2),
      (state->avatar_position.y >= AZ_BOARD_CENTER_Y ?
       (AZ_BOARD_CENTER_Y + AZ_BOARD_MIN_Y) / 2 :
       (AZ_BOARD_CENTER_Y + AZ_BOARD_MAX_Y) / 2)
    };
    az_vpluseq(&target->velocity,
               az_vmul(az_vsub(goal, target->position), 0.5 * time));
  }
}

void az_tick_targets(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    tick_target(state, target, time);
  }
}

/*===========================================================================*/
