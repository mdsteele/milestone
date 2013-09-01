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

#pragma once
#ifndef MILESTONE_STATE_TARGET_H_
#define MILESTONE_STATE_TARGET_H_

#include "milestone/util/color.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

typedef enum {
  AZ_TARG_NOTHING = 0,
  AZ_TARG_BONUS,
  AZ_TARG_NORMAL,
  AZ_TARG_REBEL,
  AZ_TARG_UNPLANNED,
} az_target_kind_t;

typedef struct {
  az_target_kind_t kind;
  az_vector_t position;
  az_vector_t velocity;
  int wave;
  double presence; // 0.0 (absent) to 1.0 (present)
  bool is_invisible; // true if target should be invisible
  double invisibility; // 0.0 (visible) to 1.0 (visible)
} az_target_t;

/*===========================================================================*/

void az_init_target(az_target_t *target, az_target_kind_t kind, int wave,
                    az_vector_t position);

void az_init_target_at_random_position(
    az_target_t *target, az_target_kind_t kind, int wave);

az_color_t az_target_color(const az_target_t *target);

/*===========================================================================*/

#endif // MILESTONE_STATE_TARGET_H_
