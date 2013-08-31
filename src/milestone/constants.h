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
#ifndef MILESTONE_CONSTANTS_H_
#define MILESTONE_CONSTANTS_H_

/*===========================================================================*/

// The dimensions of the screen, in pixels:
#define AZ_SCREEN_WIDTH  800
#define AZ_SCREEN_HEIGHT 500

// The bounds of the board:
#define AZ_BOARD_MIN_X 1
#define AZ_BOARD_CENTER_X (AZ_SCREEN_WIDTH / 2)
#define AZ_BOARD_MAX_X (AZ_SCREEN_WIDTH - 1)
#define AZ_BOARD_MIN_Y 21
#define AZ_BOARD_CENTER_Y (AZ_SCREEN_HEIGHT / 2)
#define AZ_BOARD_MAX_Y (AZ_SCREEN_HEIGHT - 21)

// Hit radius of the player's avatar, in pixels:
#define AZ_AVATAR_RADIUS 20
// Hit radius of baddies, in pixels:
#define AZ_BADDIE_RADIUS 20

#define AZ_BOMB_EXPAND_TIME 0.5
#define AZ_BOMB_MAX_RADIUS 350

/*===========================================================================*/

#endif // MILESTONE_CONSTANTS_H_
