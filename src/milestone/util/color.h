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
#ifndef MILESTONE_UTIL_COLOR_H_
#define MILESTONE_UTIL_COLOR_H_

#include <stdint.h>

/*===========================================================================*/

// Represents a color, with red, green, blue, and alpha components.
typedef struct {
  uint8_t r, g, b, a;
} az_color_t;

az_color_t az_color_for_wave(int wave);

/*===========================================================================*/

#endif // MILESTONE_UTIL_COLOR_H_
