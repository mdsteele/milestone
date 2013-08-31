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

#include <string.h>

#include "milestone/constants.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

void az_init_play_state(az_play_state_t *state) {
  memset(state, 0, sizeof(*state));
  state->avatar_position.x = AZ_SCREEN_WIDTH / 2;
  state->avatar_position.y = AZ_SCREEN_HEIGHT / 2;
  state->avatar_velocity = AZ_VZERO;
}

/*===========================================================================*/
