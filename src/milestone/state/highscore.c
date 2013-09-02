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

#include "milestone/state/highscore.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "milestone/util/misc.h"

/*===========================================================================*/

static bool try_read_highscore_list(
    FILE *file, az_highscore_list_t *highscore_list) {
  int64_t last_score = 0;
  AZ_ARRAY_LOOP(entry, highscore_list->entries) {
    int wave;
    int64_t score;
    char name[21];
    if (fscanf(file, "@%d,%"SCNd64",'%20[-a-zA-Z0-9_ .,?/!@#$%^&*()|~]'\n",
               &wave, &score, name) < 3) return false;
    if (wave < 0) return false;
    if (score < last_score) return false;
    entry->wave = wave;
    entry->score = score;
    if (wave > 0) entry->name = az_strdup(name);
    last_score = score;
  }
  return true;
}

bool az_load_highscore_list(
    const char *file_path, az_highscore_list_t *highscore_list_out) {
  assert(highscore_list_out != NULL);
  az_clear_highscore_list(highscore_list_out);
  FILE *file = fopen(file_path, "r");
  if (file == NULL) return false;
  const bool success = try_read_highscore_list(file, highscore_list_out);
  fclose(file);
  return success;
}

/*===========================================================================*/

static bool try_write_highscore_list(
    const az_highscore_list_t *highscore_list, FILE *file) {
  AZ_ARRAY_LOOP(entry, highscore_list->entries) {
    if (fprintf(file, "@%d,%"PRId64",'%s'\n", entry->wave, entry->score,
                (entry->name == NULL ? "-" : entry->name)) < 0) {
      return false;
    }
  }
  return true;
}

bool az_save_highscore_list(
    const az_highscore_list_t *highscore_list, const char *file_path) {
  FILE *file = fopen(file_path, "w");
  if (file == NULL) return false;
  const bool success = try_write_highscore_list(highscore_list, file);
  fclose(file);
  return success;
}

/*===========================================================================*/

void az_clear_highscore_list(az_highscore_list_t *highscore_list) {
  AZ_ARRAY_LOOP(entry, highscore_list->entries) {
    free(entry->name);
    memset(entry, 0, sizeof(*entry));
  }
}

/*===========================================================================*/

az_highscore_t *az_submit_highscore(az_highscore_list_t *highscore_list,
                                    int64_t score, int wave) {
  az_highscore_t *last_entry = NULL;
  AZ_ARRAY_LOOP(entry, highscore_list->entries) {
    if (entry->score >= score) break;
    if (last_entry == NULL) {
      free(entry->name);
    } else {
      assert(last_entry->name == NULL);
      *last_entry = *entry;
    }
    entry->name = NULL;
    last_entry = entry;
  }
  if (last_entry != NULL) {
    assert(last_entry->name == NULL);
    last_entry->score = score;
    last_entry->wave = wave;
  }
  return last_entry;
}

/*===========================================================================*/
