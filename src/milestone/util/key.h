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
#ifndef MILESTONE_UTIL_KEY_H_
#define MILESTONE_UTIL_KEY_H_

/*===========================================================================*/

typedef enum {
  AZ_KEY_UNKNOWN = 0,
  AZ_KEY_ESCAPE,
  AZ_KEY_BACKTICK,
  AZ_KEY_HYPHEN,
  AZ_KEY_EQUALS,
  AZ_KEY_BACKSPACE,
  AZ_KEY_TAB,
  AZ_KEY_LEFT_BRACKET,
  AZ_KEY_RIGHT_BRACKET,
  AZ_KEY_BACKSLASH,
  AZ_KEY_SEMICOLON,
  AZ_KEY_QUOTE,
  AZ_KEY_RETURN,
  AZ_KEY_COMMA,
  AZ_KEY_PERIOD,
  AZ_KEY_SLASH,
  AZ_KEY_SPACE,
  AZ_KEY_UP_ARROW,
  AZ_KEY_DOWN_ARROW,
  AZ_KEY_RIGHT_ARROW,
  AZ_KEY_LEFT_ARROW,
  AZ_KEY_0,
  AZ_KEY_1,
  AZ_KEY_2,
  AZ_KEY_3,
  AZ_KEY_4,
  AZ_KEY_5,
  AZ_KEY_6,
  AZ_KEY_7,
  AZ_KEY_8,
  AZ_KEY_9,
  AZ_KEY_A,
  AZ_KEY_B,
  AZ_KEY_C,
  AZ_KEY_D,
  AZ_KEY_E,
  AZ_KEY_F,
  AZ_KEY_G,
  AZ_KEY_H,
  AZ_KEY_I,
  AZ_KEY_J,
  AZ_KEY_K,
  AZ_KEY_L,
  AZ_KEY_M,
  AZ_KEY_N,
  AZ_KEY_O,
  AZ_KEY_P,
  AZ_KEY_Q,
  AZ_KEY_R,
  AZ_KEY_S,
  AZ_KEY_T,
  AZ_KEY_U,
  AZ_KEY_V,
  AZ_KEY_W,
  AZ_KEY_X,
  AZ_KEY_Y,
  AZ_KEY_Z
} az_key_id_t;

#define AZ_LAST_KEY_ID AZ_KEY_Z

const char *az_key_name(az_key_id_t key);

/*===========================================================================*/

#endif // MILESTONE_UTIL_KEY_H_
