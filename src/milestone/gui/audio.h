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
#ifndef MILESTONE_GUI_AUDIO_H_
#define MILESTONE_GUI_AUDIO_H_

#include "milestone/util/audio.h"

/*===========================================================================*/

// Call this once per frame to update our audio system.  The audio system must
// be initialized first (by calling az_init_gui, which will in turn call
// az_init_audio_mixer above).
void az_tick_audio_mixer(az_soundboard_t *soundboard);

// Set the global volume for music or sound effects, respectively.  The
// argument should be between 0.0 (silent) and 1.0 (full volume), inclusive.
// The audio system must be initialized first (by calling az_init_gui, which
// will in turn call az_init_audio_mixer above).
void az_set_global_music_volume(float volume);
void az_set_global_sound_volume(float volume);

/*===========================================================================*/

// Initialize our audio system (once the GUI has been initialized).  This is
// called by az_init_gui, and should not be called from elsewhere.
void az_init_audio_mixer(void);

// Pause and unpause all audio (sounds and music).  These are automatically
// called from event.c and screen.c when the app goes out/in of focus, and
// should not be called from elsewhere.
void az_pause_all_audio(void);
void az_unpause_all_audio(void);

/*===========================================================================*/

#endif // MILESTONE_GUI_AUDIO_H_
