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

#include "milestone/tick/baddie.h"

#include <assert.h>
#include <math.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

// Return true if baddie should disappear.
static bool tick_baddie(az_play_state_t *state, az_baddie_t *baddie,
                        double time) {
  assert(baddie->kind != AZ_BAD_NOTHING);

  // Move the baddie:
  az_vpluseq(&baddie->position, az_vmul(baddie->velocity, time));
  az_bounce_off_edges(&baddie->position, &baddie->velocity);

  baddie->cooldown = fmax(0.0, baddie->cooldown - time);

  // Apply special logic for the baddie kind:
  switch (baddie->kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_TANK: {
      const az_vector_t delta =
        az_vsub(state->avatar_position, baddie->position);
      baddie->velocity = az_vwithlen(delta, 70.0);
      AZ_ARRAY_LOOP(other, state->baddies) {
        if (other->kind == AZ_BAD_NOTHING) continue;
        if (other == baddie) continue;
        if (az_vwithin(baddie->position, other->position, 100.0)) {
          az_vpluseq(&baddie->velocity, az_vwithlen(
              az_vsub(baddie->position, other->position), 20.0));
        }
      }
      if (baddie->cooldown <= 0.0) {
        az_add_projectile(state, AZ_PROJ_TANK_SHELL, baddie->position,
                          az_vwithlen(delta, 600.0));
        baddie->cooldown = 2.0;
      }
    } break;
  }

  return false;
}

void az_tick_baddies(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) continue;
    tick_baddie(state, baddie, time);
  }
}

/*===========================================================================*/
