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
#ifndef MILESTONE_STATE_PLAY_H_
#define MILESTONE_STATE_PLAY_H_

#include <stdbool.h>
#include <stdint.h>

#include "milestone/util/clock.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

#define AZ_MAX_NUM_BADDIES 100
#define AZ_MAX_NUM_PROJECTILES 200
#define AZ_MAX_NUM_TARGETS 200
#define AZ_SECONDS_PER_WAVE 20.0

typedef enum {
  AZ_BAD_NOTHING = 0,
  AZ_BAD_TANK,
} az_baddie_kind_t;

typedef struct {
  az_baddie_kind_t kind;
  az_vector_t position;
  az_vector_t velocity;
  double cooldown; // seconds
} az_baddie_t;

typedef enum {
  AZ_TARG_NOTHING = 0,
  AZ_TARG_BONUS,
  AZ_TARG_NORMAL,
} az_target_kind_t;

typedef struct {
  az_target_kind_t kind;
  az_vector_t position;
  int wave;
} az_target_t;

typedef enum {
  AZ_PROJ_NOTHING = 0,
  // Player projectiles:
  AZ_PROJ_BULLET,
  AZ_PROJ_BOMB,
  // Baddie projectiles:
  AZ_PROJ_TANK_SHELL,
} az_proj_kind_t;

typedef struct {
  az_proj_kind_t kind;
  az_vector_t position;
  az_vector_t velocity;
  double age;
} az_projectile_t;

typedef struct {
  az_clock_t clock;
  // Player stats:
  int num_lives;
  int num_bombs;
  int64_t score;
  // Wave:
  int current_wave;
  int max_wave_on_board;
  bool bonus_round;
  double wave_time_remaining; // seconds
  int num_baddies_to_spawn;
  double spawn_cooldown; // seconds
  // Avatar:
  az_vector_t avatar_position;
  az_vector_t avatar_velocity;
  // Objects:
  az_baddie_t baddies[AZ_MAX_NUM_BADDIES];
  az_projectile_t projectiles[AZ_MAX_NUM_PROJECTILES];
  az_target_t targets[AZ_MAX_NUM_TARGETS];
} az_play_state_t;

/*===========================================================================*/

void az_init_play_state(az_play_state_t *state);

int az_num_waves_at_once_for_wave(int wave);

void az_add_baddie(az_play_state_t *state, az_baddie_kind_t kind,
                   az_vector_t position);

void az_add_projectile(az_play_state_t *state, az_proj_kind_t kind,
                       az_vector_t position, az_vector_t velocity);

void az_bounce_off_edges(az_vector_t *position, az_vector_t *velocity);

/*===========================================================================*/

#endif // MILESTONE_STATE_PLAY_H_
