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
#ifndef MILESTONE_STATE_BADDIE_H_
#define MILESTONE_STATE_BADDIE_H_

#include <stdint.h>

#include "milestone/util/vector.h"

/*===========================================================================*/

typedef enum {
  AZ_BAD_NOTHING = 0,
  AZ_BAD_BASILISK,
  AZ_BAD_GHOST,
  AZ_BAD_GUARD,
  AZ_BAD_TANK,
} az_baddie_kind_t;

typedef struct {
  az_baddie_kind_t kind;
  az_vector_t position;
  az_vector_t velocity;
  double cooldown; // seconds
  double stun; // seconds
  double flare; // 0 to 1
  int hitpoints;
} az_baddie_t;

/*===========================================================================*/

void az_init_baddie(az_baddie_t *baddie, az_baddie_kind_t kind,
                    az_vector_t position);

int64_t az_baddie_point_value(az_baddie_kind_t kind);

/*===========================================================================*/

#endif // MILESTONE_STATE_BADDIE_H_
