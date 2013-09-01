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

#include "milestone/tick/baddie.h"

#include <assert.h>
#include <math.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

#define FLARE_TIME 1.0

// Return true if baddie should disappear.
static bool tick_baddie(az_play_state_t *state, az_baddie_t *baddie,
                        double time) {
  assert(baddie->kind != AZ_BAD_NOTHING);

  // Move the baddie:
  az_vpluseq(&baddie->position,
             az_vmul(baddie->velocity,
                     time * (baddie->stun > 0.0 ? 0.5 : 1.0)));
  az_bounce_off_edges(&baddie->position, &baddie->velocity, AZ_BADDIE_RADIUS);

  // Cool down timers:
  baddie->cooldown = fmax(0.0, baddie->cooldown - time);
  baddie->stun = fmax(0.0, baddie->stun - time);
  baddie->flare = fmax(0.0, baddie->flare - time / FLARE_TIME);

  // Apply special logic for the baddie kind:
  switch (baddie->kind) {
    case AZ_BAD_NOTHING: AZ_ASSERT_UNREACHABLE();
    case AZ_BAD_TANK: {
      // Seek towards the avatar, but away from other nearby baddies.
      const az_vector_t delta =
        az_vsub(state->avatar_position, baddie->position);
      baddie->velocity = az_vwithlen(delta, 70.0);
      AZ_ARRAY_LOOP(other, state->baddies) {
        if (other->kind == AZ_BAD_NOTHING) continue;
        if (other == baddie) continue;
        if (az_vwithin(baddie->position, other->position, 100.0)) {
          az_vpluseq(&baddie->velocity, az_vwithlen(
              az_vsub(baddie->position, other->position), 20.0));
        }
      }
      // Fire tank shells at the avatar.
      if (baddie->cooldown <= 0.0 && baddie->stun <= 0.0) {
        az_add_projectile(state, AZ_PROJ_TANK_SHELL, baddie->position,
                          az_vwithlen(delta, 600.0));
        az_play_sound(&state->soundboard, AZ_SND_FIRE_TANK_SHELL);
        baddie->cooldown = 2.0;
      }
    } break;
    case AZ_BAD_GUARD: {
      // Seek to the closest target that doesn't have another baddie near it.
      double best_dist = INFINITY;
      az_vector_t goal = baddie->position;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind == AZ_TARG_NOTHING) continue;
        if (target->kind == AZ_TARG_BONUS) continue;
        if (target->kind == AZ_TARG_FAKE) continue;
        if (target->kind == AZ_TARG_REBEL) continue;
        if (target->presence < 0.5) continue;
        bool alone = true;
        AZ_ARRAY_LOOP(other, state->baddies) {
          if (other->kind == AZ_BAD_NOTHING) continue;
          if (other == baddie) continue;
          if (az_vwithin(other->position, target->position, 100.0)) {
            alone = false;
            break;
          }
        }
        if (alone) {
          const double dist = az_vdist(target->position, baddie->position);
          if (dist < best_dist) {
            best_dist = dist;
            goal = target->position;
          }
        }
      }
      const az_vector_t goal_delta = az_vsub(goal, baddie->position);
      baddie->velocity =
        az_vwithlen(goal_delta, fmin(100.0, 2.0 * az_vnorm(goal_delta)));
      // Fire at the avatar, firing faster when the avatar is nearer.
      if (baddie->cooldown <= 0.0 && baddie->stun <= 0.0) {
        const az_vector_t avatar_delta =
          az_vsub(state->avatar_position, baddie->position);
        az_add_projectile(state, AZ_PROJ_TANK_SHELL, baddie->position,
                          az_vwithlen(avatar_delta, 400.0));
        az_play_sound(&state->soundboard, AZ_SND_FIRE_TANK_SHELL);
        baddie->cooldown = 0.5 + fmin(1.5, az_vnorm(avatar_delta) / 400.0);
      }
    } break;
    case AZ_BAD_BASILISK: {
      // Seek towards the avatar, but away from other nearby baddies.
      const az_vector_t delta =
        az_vsub(state->avatar_position, baddie->position);
      if (!az_vwithin(baddie->position, state->avatar_position, 100.0)) {
        az_vpluseq(&baddie->velocity,
                   az_vmul(delta, time * 150.0 / az_vnorm(delta)));
      }
      AZ_ARRAY_LOOP(other, state->baddies) {
        if (other->kind == AZ_BAD_NOTHING) continue;
        if (other == baddie) continue;
        if (az_vwithin(baddie->position, other->position, 100.0)) {
          az_vpluseq(&baddie->velocity, az_vwithlen(
              az_vsub(baddie->position, other->position), 20.0));
        }
      }
      // Fire stoppers at the avatar.
      if (baddie->cooldown <= 0.0 && baddie->stun <= 0.0) {
        const double proj_speed = 500.0;
        const az_vector_t rel_velocity =
          az_vsub(state->avatar_velocity, baddie->velocity);
        az_vector_t rel_impact;
        if (az_lead_target(delta, rel_velocity, proj_speed, &rel_impact)) {
          az_add_projectile(
              state, AZ_PROJ_STOPPER, baddie->position,
              az_vadd(baddie->velocity, az_vwithlen(rel_impact, proj_speed)));
          az_play_sound(&state->soundboard, AZ_SND_FIRE_STOPPER);
          baddie->cooldown = 1.5;
        }
      }
    } break;
    case AZ_BAD_FAKER: {
      // Move away from the avatar, and away from nearby baddies and targets.
      const az_vector_t avatar = state->avatar_position;
      baddie->velocity = AZ_VZERO;
      if (az_vwithin(avatar, baddie->position, 200.0)) {
        baddie->velocity =
          az_vwithlen(az_vsub(baddie->position, avatar), 100.0);
      }
      const az_vector_t goal = {
        (state->avatar_position.x >= AZ_BOARD_CENTER_X ?
         (AZ_BOARD_CENTER_X + AZ_BOARD_MIN_X) / 2 :
         (AZ_BOARD_CENTER_X + AZ_BOARD_MAX_X) / 2),
        (state->avatar_position.y >= AZ_BOARD_CENTER_Y ?
         (AZ_BOARD_CENTER_Y + AZ_BOARD_MIN_Y) / 2 :
         (AZ_BOARD_CENTER_Y + AZ_BOARD_MAX_Y) / 2)
      };
      if (!az_vwithin(baddie->position, goal, 50.0)) {
        az_vpluseq(&baddie->velocity,
                   az_vwithlen(az_vsub(goal, baddie->position), 150.0));
      }
      AZ_ARRAY_LOOP(other, state->baddies) {
        if (other->kind == AZ_BAD_NOTHING) continue;
        if (other == baddie) continue;
        if (az_vwithin(baddie->position, other->position, 50.0)) {
          az_vpluseq(&baddie->velocity, az_vwithlen(
              az_vsub(baddie->position, other->position), 20.0));
        }
      }
      bool nearby = false;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind == AZ_TARG_NOTHING) continue;
        if (target->presence < 0.5) continue;
        if (az_vwithin(target->position, baddie->position,
                       AZ_BADDIE_RADIUS)) {
          nearby = true;
          az_vpluseq(&baddie->velocity,
                     az_vwithlen(az_vsub(baddie->position,
                                         target->position), 30.0));
        }
      }
      // Drop fake targets.
      if (!nearby && baddie->cooldown <= 0.0 && baddie->stun <= 0.0) {
        az_add_target(state, AZ_TARG_FAKE, state->current_wave,
                      baddie->position);
        baddie->cooldown = 3.0;
        az_play_sound(&state->soundboard, AZ_SND_DROP_FAKE_TARGET);
      }
    } break;
    case AZ_BAD_GHOST: {
      // Make all nearby targets invisible.  Seek towards the target nearest
      // the avatar that doesn't have a nearby baddie (including this one).
      double best_dist = INFINITY;
      az_vector_t goal = baddie->position;
      AZ_ARRAY_LOOP(target, state->targets) {
        if (target->kind == AZ_TARG_NOTHING) continue;
        if (az_vwithin(target->position, baddie->position,
                       AZ_GHOST_HIDE_RADIUS)) {
          target->is_invisible = true;
        }
        if (target->presence < 0.5 || target->kind == AZ_TARG_FAKE) continue;
        bool alone = true;
        AZ_ARRAY_LOOP(other, state->baddies) {
          if (other->kind == AZ_BAD_NOTHING) continue;
          if (az_vwithin(other->position, target->position, 150.0)) {
            alone = false;
            break;
          }
        }
        if (alone) {
          const double dist =
            az_vdist(target->position, state->avatar_position);
          if (dist < best_dist) {
            best_dist = dist;
            goal = target->position;
          }
        }
      }
      const az_vector_t goal_delta = az_vsub(goal, baddie->position);
      az_vpluseq(&baddie->velocity, az_vmul(goal_delta, time * 0.1));
    } break;
  }

  return false;
}

void az_tick_baddies(az_play_state_t *state, double time) {
  AZ_ARRAY_LOOP(baddie, state->baddies) {
    if (baddie->kind == AZ_BAD_NOTHING) continue;
    tick_baddie(state, baddie, time);
  }
}

/*===========================================================================*/
