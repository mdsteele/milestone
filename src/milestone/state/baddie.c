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

#include "milestone/util/misc.h"

/*===========================================================================*/

int64_t az_baddie_point_value(az_baddie_kind_t kind) {
  assert(kind != AZ_BAD_NOTHING);
  switch (kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_BASILISK: return 500;
    case AZ_BAD_GHOST: return 450;
    case AZ_BAD_GUARD: return 400;
    case AZ_BAD_TANK: return 300;
  }
  AZ_ASSERT_UNREACHABLE();
}

/*===========================================================================*/
