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

#include "milestone/util/color.h"

#include <assert.h>

#include "milestone/util/misc.h"

/*===========================================================================*/

az_color_t az_color_for_wave(int wave) {
  assert(wave >= 0);
  switch (wave % 6) {
    case 0: return (az_color_t){255,   0, 255, 255};
    case 1: return (az_color_t){255, 255,   0, 255};
    case 2: return (az_color_t){  0, 205, 205, 255};
    case 3: return (az_color_t){255,   0,   0, 255};
    case 4: return (az_color_t){ 64,  64, 255, 255};
    case 5: return (az_color_t){  0, 192,   0, 255};
  }
  AZ_ASSERT_UNREACHABLE();
}

/*===========================================================================*/
