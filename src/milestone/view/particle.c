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

#include "milestone/view/particle.h"

#include <GL/gl.h>

#include "milestone/constants.h"
#include "milestone/state/play.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

void az_draw_particles(const az_play_state_t *state) {
  glBegin(GL_POINTS); {
    AZ_ARRAY_LOOP(particle, state->particles) {
      if (particle->kind == AZ_PAR_NOTHING) continue;
      const az_color_t color = particle->color;
      glColor4ub(color.r, color.g, color.b,
                 color.a * (1.0 - particle->age / particle->lifetime));
      glVertex2d(particle->position.x, particle->position.y);
    }
  } glEnd();
}

/*===========================================================================*/
