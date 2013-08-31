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
#ifndef MILESTONE_STATE_PARTICLE_H_
#define MILESTONE_STATE_PARTICLE_H_

#include "milestone/util/color.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

typedef enum {
  AZ_PAR_NOTHING = 0,
  AZ_PAR_NORMAL,
} az_particle_kind_t;

typedef struct {
  az_particle_kind_t kind;
  az_color_t color;
  az_vector_t position;
  az_vector_t velocity;
  double age, lifetime;
} az_particle_t;

/*===========================================================================*/

void az_init_particle(az_particle_t *particle, az_color_t color,
                      az_vector_t position, az_vector_t velocity,
                      double lifetime);

/*===========================================================================*/

#endif // MILESTONE_STATE_PARTICLE_H_
