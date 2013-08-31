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

#include "milestone/util/key.h"

#include "milestone/util/misc.h"

/*===========================================================================*/

const char *az_key_name(az_key_id_t key) {
  switch (key) {
    case AZ_KEY_UNKNOWN: return "???";
    case AZ_KEY_ESCAPE: return "ESC";
    case AZ_KEY_BACKTICK: return "~";
    case AZ_KEY_HYPHEN: return "-";
    case AZ_KEY_EQUALS: return "=";
    case AZ_KEY_BACKSPACE: return "DEL";
    case AZ_KEY_TAB: return "TAB";
    case AZ_KEY_LEFT_BRACKET: return "{";
    case AZ_KEY_RIGHT_BRACKET: return "}";
    case AZ_KEY_BACKSLASH: return "\\";
    case AZ_KEY_SEMICOLON: return ";";
    case AZ_KEY_QUOTE: return "\"";
    case AZ_KEY_RETURN: return "RET";
    case AZ_KEY_COMMA: return ",";
    case AZ_KEY_PERIOD: return ".";
    case AZ_KEY_SLASH: return "/";
    case AZ_KEY_SPACE: return "SPC";
    case AZ_KEY_UP_ARROW: return "\x11";
    case AZ_KEY_DOWN_ARROW: return "\x12";
    case AZ_KEY_RIGHT_ARROW: return "\x13";
    case AZ_KEY_LEFT_ARROW: return "\x14";
    case AZ_KEY_0: return "0";
    case AZ_KEY_1: return "1";
    case AZ_KEY_2: return "2";
    case AZ_KEY_3: return "3";
    case AZ_KEY_4: return "4";
    case AZ_KEY_5: return "5";
    case AZ_KEY_6: return "6";
    case AZ_KEY_7: return "7";
    case AZ_KEY_8: return "8";
    case AZ_KEY_9: return "9";
    case AZ_KEY_A: return "A";
    case AZ_KEY_B: return "B";
    case AZ_KEY_C: return "C";
    case AZ_KEY_D: return "D";
    case AZ_KEY_E: return "E";
    case AZ_KEY_F: return "F";
    case AZ_KEY_G: return "G";
    case AZ_KEY_H: return "H";
    case AZ_KEY_I: return "I";
    case AZ_KEY_J: return "J";
    case AZ_KEY_K: return "K";
    case AZ_KEY_L: return "L";
    case AZ_KEY_M: return "M";
    case AZ_KEY_N: return "N";
    case AZ_KEY_O: return "O";
    case AZ_KEY_P: return "P";
    case AZ_KEY_Q: return "Q";
    case AZ_KEY_R: return "R";
    case AZ_KEY_S: return "S";
    case AZ_KEY_T: return "T";
    case AZ_KEY_U: return "U";
    case AZ_KEY_V: return "V";
    case AZ_KEY_W: return "W";
    case AZ_KEY_X: return "X";
    case AZ_KEY_Y: return "Y";
    case AZ_KEY_Z: return "Z";
  }
  AZ_ASSERT_UNREACHABLE();
}

/*===========================================================================*/
