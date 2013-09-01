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

#include "milestone/tick/projectile.h"

#include <assert.h>
#include <math.h>

#include "milestone/constants.h"
#include "milestone/state/baddie.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

static void kill_baddie(az_play_state_t *state, az_baddie_t *baddie) {
  assert(baddie->kind != AZ_BAD_NOTHING);
  az_award_points(state, az_baddie_point_value(baddie->kind));
  // TODO: particles for dead baddie
  az_sound_key_t sound = AZ_SND_KILL_TANK;
  switch (baddie->kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_BASILISK: sound = AZ_SND_KILL_BASILISK; break;
    case AZ_BAD_GHOST: sound = AZ_SND_KILL_TANK; break; // TODO
    case AZ_BAD_GUARD: sound = AZ_SND_KILL_GUARD; break;
    case AZ_BAD_TANK: sound = AZ_SND_KILL_TANK; break;
  }
  az_play_sound(&state->soundboard, sound);
  baddie->kind = AZ_BAD_NOTHING;
}

static void hurt_baddie(az_play_state_t *state, az_baddie_t *baddie) {
  --baddie->hitpoints;
  if (baddie->hitpoints <= 0) kill_baddie(state, baddie);
  else {
    baddie->flare = 1.0;
    // TODO: play sound
  }
}

// Return true if projectile should disappear.
static bool tick_projectile(az_play_state_t *state, az_projectile_t *proj,
                            double time) {
  assert(proj->kind != AZ_PROJ_NOTHING);

  // Age the projectile, removing it if it's too old:
  proj->age += time;
  if (proj->age > 10.0) {
    return true;
  }

  // Move the projectile, removing it if it leaves the board:
  az_vpluseq(&proj->position, az_vmul(proj->velocity, time));
  if (proj->position.x < AZ_BOARD_MIN_X || proj->position.x > AZ_BOARD_MAX_X ||
      proj->position.y < AZ_BOARD_MIN_Y || proj->position.y > AZ_BOARD_MAX_Y) {
    return true;
  }

  // Apply special logic for the projectile kind:
  switch (proj->kind) {
    case AZ_PROJ_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_PROJ_BULLET:
      AZ_ARRAY_LOOP(baddie, state->baddies) {
        if (baddie->kind == AZ_BAD_NOTHING) continue;
        if (az_vwithin(baddie->position, proj->position, AZ_BADDIE_RADIUS)) {
          hurt_baddie(state, baddie);
          return true;
        }
      }
      break;
    case AZ_PROJ_BOMB: {
      if (proj->age > 2.0 * AZ_BOMB_EXPAND_TIME) return true;
      const double radius = AZ_BOMB_MAX_RADIUS *
        fmin(1.0, proj->age / AZ_BOMB_EXPAND_TIME);
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind == AZ_TARG_NOTHING) continue;
        if (target->kind == AZ_TARG_BONUS) continue;
        if (az_vwithin(target->position, proj->position, radius)) {
          target->kind = AZ_TARG_NORMAL;
          target->velocity = AZ_VZERO;
          if (target->wave <= state->current_wave) {
            target->wave = state->current_wave + 1;
          }
        }
      }
      AZ_ARRAY_LOOP(baddie, state->baddies) {
        if (baddie->kind == AZ_BAD_NOTHING) continue;
        if (az_vwithin(baddie->position, proj->position, radius)) {
          kill_baddie(state, baddie);
        }
      }
    } break;
    case AZ_PROJ_TANK_SHELL:
      if (az_vwithin(proj->position, state->avatar_position,
                     AZ_AVATAR_RADIUS)) {
        az_vpluseq(&state->avatar_velocity, az_vmul(proj->velocity, 0.5));
        az_play_sound(&state->soundboard, AZ_SND_HIT_TANK_SHELL);
        return true;
      }
      break;
    case AZ_PROJ_STOPPER:
      if (az_vwithin(proj->position, state->avatar_position,
                     AZ_AVATAR_RADIUS)) {
        state->avatar_velocity = az_vmul(state->avatar_velocity, 0.2);
        return true;
      }
      break;
  }

  return false;
}

void az_tick_projectiles(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(proj, state->projectiles) {
    if (proj->kind == AZ_PROJ_NOTHING) continue;
    if (tick_projectile(state, proj, time)) proj->kind = AZ_PROJ_NOTHING;
  }
}

/*===========================================================================*/
