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

#include "milestone/state/target.h"

#include <assert.h>
#include <string.h>

#include "milestone/constants.h"
#include "milestone/util/color.h"
#include "milestone/util/random.h"

/*===========================================================================*/

void az_init_target(az_target_t *target, az_target_kind_t kind, int wave,
                    az_vector_t position) {
  memset(target, 0, sizeof(*target));
  target->kind = kind;
  target->wave = wave;
  target->position = position;
}

void az_init_target_at_random_position(
    az_target_t *target, az_target_kind_t kind, int wave) {
  const az_vector_t position = {
    az_random(AZ_BOARD_MIN_X + AZ_TARGET_RADIUS,
              AZ_BOARD_MAX_X - AZ_TARGET_RADIUS),
    az_random(AZ_BOARD_MIN_Y + AZ_TARGET_RADIUS,
              AZ_BOARD_MAX_Y - AZ_TARGET_RADIUS)
  };
  az_init_target(target, kind, wave, position);
}

az_color_t az_target_color(const az_target_t *target) {
  assert(target->kind != AZ_TARG_NOTHING);
  if (target->kind == AZ_TARG_BONUS) return (az_color_t){255, 255, 255, 255};
  return az_color_for_wave(target->wave);
}

/*===========================================================================*/
