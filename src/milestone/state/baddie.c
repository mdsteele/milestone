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

#include "milestone/state/baddie.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "milestone/util/misc.h"

/*===========================================================================*/

static int baddie_hitpoints(az_baddie_kind_t kind) {
  assert(kind != AZ_BAD_NOTHING);
  switch (kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_BASILISK: return 2;
    case AZ_BAD_FAKER: return 3;
    case AZ_BAD_FORCER: return 2;
    case AZ_BAD_GHOST: return 1;
    case AZ_BAD_GUARD: return 4;
    case AZ_BAD_TANK: return 1;
  }
  AZ_ASSERT_UNREACHABLE();
}

void az_init_baddie(az_baddie_t *baddie, az_baddie_kind_t kind,
                    az_vector_t position) {
  assert(kind != AZ_BAD_NOTHING);
  memset(baddie, 0, sizeof(*baddie));
  baddie->kind = kind;
  baddie->position = position;
  baddie->cooldown = 1.0;
  baddie->hitpoints = baddie_hitpoints(kind);
}

int64_t az_baddie_point_value(az_baddie_kind_t kind) {
  assert(kind != AZ_BAD_NOTHING);
  switch (kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_BASILISK: return 500;
    case AZ_BAD_FAKER: return 750;
    case AZ_BAD_FORCER: return 1000;
    case AZ_BAD_GHOST: return 450;
    case AZ_BAD_GUARD: return 400;
    case AZ_BAD_TANK: return 300;
  }
  AZ_ASSERT_UNREACHABLE();
}

/*===========================================================================*/
