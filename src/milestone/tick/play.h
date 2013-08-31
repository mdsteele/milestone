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
#ifndef MILESTONE_TICK_PLAY_H_
#define MILESTONE_TICK_PLAY_H_

#include "milestone/state/play.h"

/*===========================================================================*/

void az_tick_play_state(az_play_state_t *state, double time);

void az_play_apply_mouse_motion(az_play_state_t *state, int dx, int dy);

void az_play_apply_mouse_click(az_play_state_t *state);

void az_play_apply_spacebar(az_play_state_t *state);

/*===========================================================================*/

#endif // MILESTONE_TICK_PLAY_H_
