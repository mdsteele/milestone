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

#include "milestone/util/misc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*===========================================================================*/

void _az_zero_array(void *ptr, size_t n, size_t size) {
  memset(ptr, 0, n * size);
}

void _az_fatal(const char *funcname, const char *format, ...) {
  va_list args;
  fprintf(stderr, "Fatal error in %s: ", funcname);
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

void *_az_alloc(const char *funcname, size_t n, size_t size) {
  if (n == 0) return NULL;
  void *ptr = calloc(n, size);
  if (ptr == NULL) {
    _az_fatal(funcname, "Out of memory.\n");
  }
  return ptr;
}

AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a) == 1);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b) == 2);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b,c) == 3);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b,c,d,e) == 5);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b,c,d,e,f,g,h) == 8);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b,c,d,e,f,g,h,i,j,k,l,m) == 13);
AZ_STATIC_ASSERT(AZ_COUNT_ARGS(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) == 15);

/*===========================================================================*/
