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
#ifndef MILESTONE_STATE_GAMEOVER_H_
#define MILESTONE_STATE_GAMEOVER_H_

#include <stdbool.h>
#include <stdint.h>

#include "milestone/state/highscore.h"
#include "milestone/util/audio.h"
#include "milestone/util/clock.h"

/*===========================================================================*/

typedef struct {
  az_clock_t clock;
  az_soundboard_t soundboard;
  int last_wave;
  int64_t score;
  bool is_high_score;
  int cursor;
  char name_buffer[21];
} az_gameover_state_t;

void az_init_gameover_state(az_gameover_state_t *state);

/*===========================================================================*/

#endif // MILESTONE_STATE_GAMEOVER_H_
