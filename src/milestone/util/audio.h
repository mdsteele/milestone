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
#ifndef MILESTONE_UTIL_AUDIO_H_
#define MILESTONE_UTIL_AUDIO_H_

#include <stdbool.h>

/*===========================================================================*/

typedef enum {
  AZ_MUS_NOTHING = 0,
  AZ_MUS_TITLE,
  AZ_MUS_PLAY,
} az_music_key_t;

typedef enum {
  AZ_SND_BONUS_ROUND,
  AZ_SND_BREAK_BONUS_TARGET,
  AZ_SND_BREAK_FAKE_TARGET,
  AZ_SND_BREAK_NORMAL_TARGET,
  AZ_SND_DROP_FAKE_TARGET,
  AZ_SND_FIRE_BOMB,
  AZ_SND_FIRE_BULLET,
  AZ_SND_FIRE_STOPPER,
  AZ_SND_FIRE_TANK_SHELL,
  AZ_SND_FORCEFIELD,
  AZ_SND_GAIN_BOMB,
  AZ_SND_GAIN_LIFE,
  AZ_SND_HIT_TANK_SHELL,
  AZ_SND_HURT_BADDIE,
  AZ_SND_KILL_BASILISK,
  AZ_SND_KILL_GHOST,
  AZ_SND_KILL_GUARD,
  AZ_SND_KILL_TANK,
  AZ_SND_LOSE_LIFE,
  AZ_SND_NEXT_WAVE,
  AZ_SND_STUN_BADDIE,
} az_sound_key_t;

// A soundboard keeps track of what sounds/music we want to play next.  It will
// be periodically read and flushed by our audio system.  To initialize it,
// simply zero it with a memset.  One should not manipulate the fields of this
// struct directly; instead, use the various functions below.
typedef struct {
  enum { AZ_MUSA_NOTHING = 0, AZ_MUSA_CHANGE, AZ_MUSA_STOP } music_action;
  az_music_key_t next_music;
  int music_fade_out_millis;
  int num_oneshots;
  az_sound_key_t oneshots[10];
  int num_persists;
  struct {
    az_sound_key_t sound;
    bool play;
    bool loop;
    bool reset;
  } persists[10];
} az_soundboard_t;

/*===========================================================================*/

// Indicate that we would like to change which music is playing.
void az_change_music(az_soundboard_t *soundboard, az_music_key_t music);

// Indicate that we would like to stop the current music (without playing
// something else next), by fading out for the given amount of time.
void az_stop_music(az_soundboard_t *soundboard, double fade_out_seconds);

// Indicate that we should play the given sound (once).  The sound will not
// loop, and cannot be cancelled or paused once started.
void az_play_sound(az_soundboard_t *soundboard, az_sound_key_t sound);

// Indicate that we should start playing, or continue to play, the given sound.
// To keep the sound going, we must call this function every frame with the
// same sound, otherwise the sound will stop.  As long as we keep calling this
// function, the sound will continue to loop.
void az_loop_sound(az_soundboard_t *soundboard, az_sound_key_t sound);

// Indicate that we should start playing, or continue to play, the given sound.
// To keep the sound going, we must call this function every frame with the
// same sound, otherwise the sound will stop.  The sound will play only once,
// and won't restart until we either stop calling this function for at least
// one frame before calling it again, or we call az_reset_sound.
void az_persist_sound(az_soundboard_t *soundboard, az_sound_key_t sound);

// Indicate that, if the given persisted or looped sound is currently playing,
// we should pause it for this frame.  To keep the sound from resetting, we
// must call this function every frame with the same sound; the sound will
// resume once we start calling az_persist_sound or az_loop_sound again.
void az_hold_sound(az_soundboard_t *soundboard, az_sound_key_t sound);

// Indicate that the given persisted or looped sound should restart from the
// beginning, even if we also call az_persist_sound or az_loop_sound this frame
// to keep the sound going.
void az_reset_sound(az_soundboard_t *soundboard, az_sound_key_t sound);

/*===========================================================================*/

#endif // MILESTONE_UTIL_AUDIO_H_
