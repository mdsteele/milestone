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

#include "milestone/tick/play.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/tick/baddie.h"
#include "milestone/tick/particle.h"
#include "milestone/tick/projectile.h"
#include "milestone/tick/target.h"
#include "milestone/util/audio.h"
#include "milestone/util/misc.h"
#include "milestone/util/random.h"
#include "milestone/util/vector.h"

/*===========================================================================*/

static az_vector_t pick_spawn_point(const az_play_state_t *state) {
  const az_vector_t avatar = state->avatar_position;
  int corner = az_randint(1, 3);
  if (!(avatar.x >= AZ_BOARD_CENTER_X && avatar.y >= AZ_BOARD_CENTER_Y)) {
    if (--corner == 0) return (az_vector_t){AZ_BOARD_MAX_X, AZ_BOARD_MAX_Y};
  }
  if (!(avatar.x < AZ_BOARD_CENTER_X && avatar.y >= AZ_BOARD_CENTER_Y)) {
    if (--corner == 0) return (az_vector_t){AZ_BOARD_MIN_X, AZ_BOARD_MAX_Y};
  }
  if (!(avatar.x < AZ_BOARD_CENTER_X && avatar.y < AZ_BOARD_CENTER_Y)) {
    if (--corner == 0) return (az_vector_t){AZ_BOARD_MIN_X, AZ_BOARD_MIN_Y};
  }
  assert(!(avatar.x >= AZ_BOARD_CENTER_X && avatar.y < AZ_BOARD_CENTER_Y));
  assert(corner == 1);
  return (az_vector_t){AZ_BOARD_MAX_X, AZ_BOARD_MIN_Y};
}

static void spawn_baddies(az_play_state_t *state, double time) {
  state->spawn_cooldown -= time;
  if (state->spawn_cooldown > 0.0) return;
  state->spawn_cooldown = 1.9;

  if (state->num_baddies_to_spawn <= 0) return;
  --state->num_baddies_to_spawn;

  // TODO: pick a baddie kind based on wave
  const az_baddie_kind_t kind = AZ_BAD_TANK;

  az_add_baddie(state, kind, pick_spawn_point(state));
}

/*===========================================================================*/

static void tick_avatar(az_play_state_t *state, double time) {

  // Move avatar:
  state->avatar_velocity = az_vcaplen(state->avatar_velocity, 500.0);
  az_vpluseq(&state->avatar_position,
             az_vmul(state->avatar_velocity, time));

  // Bounce off of baddies:
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) continue;
    if (baddie->stun > 0.0) continue;
    if (az_vwithin(baddie->position, state->avatar_position,
                   AZ_AVATAR_RADIUS + AZ_BADDIE_RADIUS)) {
      const az_vector_t delta =
        az_vsub(state->avatar_position, baddie->position);
      az_vpluseq(&state->avatar_velocity,
                 az_vmul(az_vproj(state->avatar_velocity, delta), -2.0));
      state->avatar_position =
        az_vadd(baddie->position,
                az_vwithlen(delta, AZ_AVATAR_RADIUS + AZ_BADDIE_RADIUS));
      baddie->stun = 2.5;
    }
  }

  az_bounce_off_edges(&state->avatar_position, &state->avatar_velocity);

  // Collect targets:
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->presence < 0.5) continue;
    if (az_vwithin(target->position, state->avatar_position,
                   AZ_AVATAR_RADIUS + AZ_TARGET_RADIUS)) {
      // Determine the attributes of this target.
      int value = 250;
      double elasticity = 0.75;
      if (target->kind == AZ_TARG_BONUS) {
        value *= sqrt(target->wave);
        elasticity = 2.0;
      }
      // Create particles for the target.
      const az_color_t color = az_target_color(target);
      for (int i = 0; i < 360; i += 5) {
        const az_vector_t unit = az_vpolar(1.0, az_random(0, AZ_TWO_PI));
        az_add_particle(state, color,
                        az_vadd(target->position,
                                az_vmul(unit, AZ_TARGET_RADIUS)),
                        az_vmul(unit, elasticity * az_random(20, 150)),
                        az_random(2.0, 5.0));
      }
      // Bounce the avatar and remove the target.
      const az_vector_t delta =
        az_vsub(state->avatar_position, target->position);
      target->kind = AZ_TARG_NOTHING;
      state->score += value;
      az_vpluseq(&state->avatar_velocity,
                 az_vmul(az_vproj(state->avatar_velocity, delta),
                         -(1.0 + elasticity)));
      // Play a sound.
      az_play_sound(&state->soundboard, AZ_SND_BREAK_TARGET);
    }
  }
}

/*===========================================================================*/

void begin_wave(az_play_state_t *state, bool skip) {
  state->wave_time_remaining = AZ_SECONDS_PER_WAVE;
  state->bonus_round = false;

  // Move any targets from the previous wave to this one, and lose a life if
  // there are any such targets:
  bool delayed = false;
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->kind == AZ_TARG_BONUS) {
      for (int i = 0; i < 360; i += 20) {
        az_add_particle(state, (az_color_t){255, 255, 255, 255},
                        az_vadd(target->position,
                                az_vpolar(AZ_TARGET_RADIUS, AZ_DEG2RAD(i))),
                        az_vpolar(50.0, AZ_DEG2RAD(i + 90)), 1.0);
      }
      target->kind = AZ_TARG_NOTHING;
    } else if (target->wave < state->current_wave) {
      target->wave = state->current_wave;
      delayed = true;
    }
  }
  if (skip) {
    assert(!delayed);
  } else if (delayed) {
    --state->num_lives;
    // TODO: check for game over
    state->lost_life_flash = 1.0;
    az_play_sound(&state->soundboard, AZ_SND_LOSE_LIFE);
  } else az_play_sound(&state->soundboard, AZ_SND_NEXT_WAVE);

  // Add new targets:
  const int new_max_wave_on_board = state->current_wave +
    az_num_waves_at_once_for_wave(state->current_wave) - 1;
  for (int wave = state->max_wave_on_board + 1;
       wave <= new_max_wave_on_board; ++wave) {
    int num_new_targets = 6 + 2 * sqrt(wave);
    AZ_ARRAY_LOOP(target, state->targets) {
      if (target->kind != AZ_TARG_NOTHING) continue;
      az_target_kind_t kind = AZ_TARG_NORMAL;
      if (az_random(0, 1) < 0.1) kind = AZ_TARG_REBEL;
      else if (az_random(0, 1) < 0.5) kind = AZ_TARG_UNPLANNED;
      az_init_target_at_random_position(target, kind, wave);
      --num_new_targets;
      if (num_new_targets <= 0) break;
    }
  }
  state->max_wave_on_board = new_max_wave_on_board;

  // Prepare for spawning baddies:
  state->num_baddies_to_spawn = state->current_wave / 2;
  state->spawn_cooldown = 1.2;
}

/*===========================================================================*/

#define BONUS_FLASH_TIME 0.5
#define GAINED_LIFE_FLASH_TIME 1.0
#define LOST_LIFE_FLASH_TIME 1.5

void az_tick_play_state(az_play_state_t *state, double time) {
  ++state->clock;
  state->bonus_flash = fmax(0.0, state->bonus_flash - time / BONUS_FLASH_TIME);
  state->gained_life_flash =
    fmax(0.0, state->gained_life_flash - time / GAINED_LIFE_FLASH_TIME);
  state->lost_life_flash =
    fmax(0.0, state->lost_life_flash - time / LOST_LIFE_FLASH_TIME);

  az_tick_particles(state, time);
  spawn_baddies(state, time);
  az_tick_projectiles(state, time);
  az_tick_baddies(state, time);
  az_tick_targets(state, time);
  tick_avatar(state, time);

  // If all of this wave's targets are collected, go to the bonus round.
  const int next_wave = state->current_wave + 1;
  int current_wave_all_targets_remaining = 0;
  int current_wave_planned_targets_remaining = 0;
  int next_wave_all_targets_remaining = 0;
  int next_wave_planned_targets_remaining = 0;
  AZ_ARRAY_LOOP(target, state->targets) {
    if (target->kind == AZ_TARG_NOTHING) continue;
    if (target->kind == AZ_TARG_BONUS) continue;
    if (target->wave <= state->current_wave) {
      ++current_wave_all_targets_remaining;
      if (target->kind != AZ_TARG_UNPLANNED) {
        ++current_wave_planned_targets_remaining;
      }
    } else if (target->wave == next_wave) {
      ++next_wave_all_targets_remaining;
      if (target->kind != AZ_TARG_UNPLANNED) {
        ++next_wave_planned_targets_remaining;
      }
    }
  }
  if (current_wave_planned_targets_remaining == 0 &&
      current_wave_all_targets_remaining > 0) {
    AZ_ARRAY_LOOP(target, state->targets) {
      if (target->kind == AZ_TARG_UNPLANNED &&
          target->wave == state->current_wave) {
        target->kind = AZ_TARG_NORMAL;
      }
    }
  }
  if (state->bonus_round && next_wave_planned_targets_remaining == 0 &&
      next_wave_all_targets_remaining > 0) {
    AZ_ARRAY_LOOP(target, state->targets) {
      if (target->kind == AZ_TARG_UNPLANNED &&
          target->wave == next_wave) {
        target->kind = AZ_TARG_NORMAL;
      }
    }
  }
  if (state->current_wave > 0 && current_wave_all_targets_remaining == 0) {
    if (next_wave_all_targets_remaining == 0) {
      ++state->num_lives;
      state->wave_time_remaining = 0.0;
      state->current_wave += 2;
      state->gained_life_flash = 1.0;
      az_play_sound(&state->soundboard, AZ_SND_GAIN_LIFE);
      begin_wave(state, true);
    } else if (!state->bonus_round) {
      state->bonus_round = true;
      state->bonus_flash = 1.0;
      az_play_sound(&state->soundboard, AZ_SND_BONUS_ROUND);
      int num_bonus_targets = 8 + state->current_wave;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind != AZ_TARG_NOTHING) continue;
        az_init_target_at_random_position(
            target, AZ_TARG_BONUS, state->current_wave);
        --num_bonus_targets;
        if (num_bonus_targets <= 0) break;
      }
    }
  }

  // Check for wave being over:
  state->wave_time_remaining -= time;
  if (state->wave_time_remaining <= 0.0) {
    ++state->current_wave;
    begin_wave(state, false);
  }
}

/*===========================================================================*/

void az_play_apply_mouse_motion(az_play_state_t *state, int dx, int dy) {
  const az_vector_t impulse = {0.5 * dx, 0.5 * dy};
  az_vpluseq(&state->avatar_velocity, impulse);
}

void az_play_apply_mouse_click(az_play_state_t *state) {
  az_add_projectile(state, AZ_PROJ_BULLET, state->avatar_position,
                    az_vadd(state->avatar_velocity,
                            az_vwithlen(state->avatar_velocity, 200.0)));
  az_play_sound(&state->soundboard, AZ_SND_FIRE_BULLET);
}

void az_play_apply_spacebar(az_play_state_t *state) {
  if (state->num_bombs > 0) {
    --state->num_bombs;
    az_add_projectile(state, AZ_PROJ_BOMB, state->avatar_position, AZ_VZERO);
    az_play_sound(&state->soundboard, AZ_SND_FIRE_BOMB);
  }
}

/*===========================================================================*/
