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
#ifndef MILESTONE_VIEW_AVATAR_H_
#define MILESTONE_VIEW_AVATAR_H_

#include "milestone/state/play.h"

/*===========================================================================*/

void az_draw_avatar(const az_play_state_t *state);

void az_draw_avatar_icon(int left, int top, int current_wave);

void az_draw_bomb_icon(int left, int top);

/*===========================================================================*/

#endif // MILESTONE_VIEW_AVATAR_H_
