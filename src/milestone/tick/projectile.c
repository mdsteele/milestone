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

#include "milestone/tick/projectile.h"

#include <assert.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

// Return true if projectile should disappear.
static bool tick_projectile(az_play_state_t *state, az_projectile_t *proj,
                            double time) {
  assert(proj->kind != AZ_PROJ_NOTHING);

  // Age the projectile, removing it if it's too old:
  proj->age += time;
  if (proj->age > 10.0) {
    return true;
  }

  // Move the projectile, removing it if it leaves the board:
  az_vpluseq(&proj->position, az_vmul(proj->velocity, time));
  if (proj->position.x < AZ_BOARD_MIN_X || proj->position.x > AZ_BOARD_MAX_X ||
      proj->position.y < AZ_BOARD_MIN_Y || proj->position.y > AZ_BOARD_MAX_Y) {
    return true;
  }

  // Apply special logic for the projectile kind:
  switch (proj->kind) {
    case AZ_PROJ_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_PROJ_BULLET:
      // TODO: kill baddies
      break;
    case AZ_PROJ_TANK_SHELL:
      if (az_vwithin(proj->position, state->avatar_position,
                     AZ_AVATAR_RADIUS)) {
        az_vpluseq(&state->avatar_velocity, az_vmul(proj->velocity, 0.5));
        return true;
      }
      break;
  }

  return false;
}

void az_tick_projectiles(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(proj, state->projectiles) {
    if (proj->kind == AZ_PROJ_NOTHING) continue;
    if (tick_projectile(state, proj, time)) proj->kind = AZ_PROJ_NOTHING;
  }
}

/*===========================================================================*/
