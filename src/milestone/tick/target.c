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

#define FADE_TIME 1.0

static void tick_target(az_play_state_t *state, az_target_t *target,
                        double time) {
  if (target->is_invisible) {
    target->invisibility = fmin(1.0, target->invisibility + time / FADE_TIME);
  } else {
    target->invisibility = fmax(0.0, target->invisibility - time / FADE_TIME);
  }
  target->is_invisible = false;
}

void az_tick_targets(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    tick_target(state, target, time);
  }
}

/*===========================================================================*/
