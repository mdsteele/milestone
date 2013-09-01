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

#include "milestone/tick/particle.h"

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

void az_tick_particles(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(particle, state->particles) {
    if (particle->kind == AZ_PAR_NOTHING) continue;
    particle->age += time;
    if (particle->age >= particle->lifetime) particle->kind = AZ_PAR_NOTHING;
    else az_vpluseq(&particle->position, az_vmul(particle->velocity, time));
    const az_vector_t position = particle->position;
    if (position.x <= AZ_BOARD_MIN_X || position.x >= AZ_BOARD_MAX_X ||
        position.y <= AZ_BOARD_MIN_Y || position.y >= AZ_BOARD_MAX_Y) {
      particle->kind = AZ_PAR_NOTHING;
    }
  }
}

/*===========================================================================*/
