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
#ifndef MILESTONE_STATE_TITLE_H_
#define MILESTONE_STATE_TITLE_H_

#include "milestone/state/highscore.h"
#include "milestone/util/audio.h"
#include "milestone/util/clock.h"

/*===========================================================================*/

typedef struct {
  az_clock_t clock;
  az_soundboard_t soundboard;
  const az_highscore_list_t *highscore_list;
} az_title_state_t;

void az_init_title_state(az_title_state_t *state,
                         const az_highscore_list_t *highscore_list);

/*===========================================================================*/

#endif // MILESTONE_STATE_TITLE_H_
