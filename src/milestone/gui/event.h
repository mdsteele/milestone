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
#ifndef MILESTONE_GUI_EVENT_H_
#define MILESTONE_GUI_EVENT_H_

#include <stdbool.h>

#include "milestone/util/key.h"

/*===========================================================================*/

typedef enum {
  AZ_EVENT_KEY_DOWN,
  AZ_EVENT_KEY_UP,
  AZ_EVENT_MOUSE_DOWN,
  AZ_EVENT_MOUSE_UP,
  AZ_EVENT_MOUSE_MOVE
} az_event_kind_t;

typedef union {
  az_event_kind_t kind;
  struct {
    az_event_kind_t kind;
    az_key_id_t id;
    bool command; // true if Command/Ctrl (depending on OS) key is held
    bool shift; // true if Shift key is held
    int character; // unicode character
  } key;
  struct {
    az_event_kind_t kind;
    int x, y; // current mouse position
    int dx, dy; // change in mouse position (for MOUSE_MOVE only)
    bool pressed; // true if left mouse button is held
  } mouse;
} az_event_t;

// Get the next event in the queue and return true, or return false if the
// event queue is empty.
bool az_poll_event(az_event_t *event);

// Get the current position of the mouse in the window and return true, or
// return false if the mouse is not currently in the window.
bool az_get_mouse_position(int *x, int *y);

// Determine if the (left) mouse button is currently being held down.
bool az_is_mouse_held(void);

// Determine if a particular key is currently being held down.  The argument
// must not be AZ_KEY_UNKNOWN.
bool az_is_key_held(az_key_id_t key);

/*===========================================================================*/

#endif // MILESTONE_GUI_EVENT_H_
