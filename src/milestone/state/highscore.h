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
#ifndef MILESTONE_STATE_HIGHSCORE_H_
#define MILESTONE_STATE_HIGHSCORE_H_

#include <stdbool.h>
#include <stdint.h>

/*===========================================================================*/

typedef struct {
  char *name; // NUL-terminated, owned; NULL for no entry
  int64_t score;
  int wave;
} az_highscore_t;

typedef struct {
  az_highscore_t entries[12]; // first entry is lowest score
} az_highscore_list_t;

/*===========================================================================*/

bool az_load_highscore_list(const char *file_path,
                            az_highscore_list_t *highscore_list_out);

bool az_save_highscore_list(const az_highscore_list_t *highscore_list,
                            const char *file_path);

void az_clear_highscore_list(az_highscore_list_t *highscore_list);

// If the score is high enough to make it on the list, return a pointer to the
// entry.  Otherwise, return NULL.
az_highscore_t *az_submit_highscore(az_highscore_list_t *highscore_list,
                                    int64_t score, int wave);

/*===========================================================================*/

#endif // MILESTONE_STATE_HIGHSCORE_H_
