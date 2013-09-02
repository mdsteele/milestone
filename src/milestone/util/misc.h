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
#ifndef MILESTONE_UTIL_MISC_H_
#define MILESTONE_UTIL_MISC_H_

#include <stddef.h> // for size_t

/*===========================================================================*/

// Get the length of a statically-sized array, as a compile-time constant.
#define AZ_ARRAY_SIZE(a) \
  ((int)((sizeof(a) / sizeof(*(a))) / !(sizeof(a) % sizeof(*(a)))))

// Loop over a statically-sized array.
#define AZ_ARRAY_LOOP(var_name, array) \
  for (__typeof__(&*(array)) var_name = (array); \
       var_name != (array) + AZ_ARRAY_SIZE(array); ++var_name)

// Zero the memory of a statically-sized array.
#define AZ_ZERO_ARRAY(array) \
  _az_zero_array(array, AZ_ARRAY_SIZE(array), sizeof(array[0]))
void _az_zero_array(void *ptr, size_t n, size_t size);

// Signal a fatal error and exit the program.
#define AZ_FATAL(...) _az_fatal(__func__, __VA_ARGS__)
void _az_fatal(const char *funcname, const char *format, ...)
  __attribute__((__format__(__printf__,2,3),__noreturn__));

// Allocate a block of memory large enough to fit a type[n] array.  If you only
// want to allocate a single object rather than an array, simply use n=1.  If
// memory allocation fails, this will signal a fatal error and exit the
// program.  Returns NULL for n=0.
#define AZ_ALLOC(n, type) ((type *)_az_alloc(__func__, (n), sizeof(type)))
void *_az_alloc(const char *funcname, size_t n, size_t size)
  __attribute__((__malloc__));

// Duplicate a string, using AZ_ALLOC for allocation.  Returns NULL if passed
// NULL.
char *az_strdup(const char *string);

// Use this macro to indicate that this point in the code should never be
// reached at runtime.  If it is reached anyway (presumably due to a bug), it
// will terminate the program with a fatal error.
#define AZ_ASSERT_UNREACHABLE() AZ_FATAL("line %d: unreachable\n", __LINE__)

// Use this macro to check at compile time that a (compile-time-constant)
// expression is true.  It is legal at top-level or within a function.
#define AZ_STATIC_ASSERT(condition) \
  typedef int AZ_JOIN(az_static_assertion_, __LINE__)[(condition) ? 1 : -1]

// Join two tokens.  This is handy when one of the two tokens is __LINE__;
// using AZ_JOIN(foo_, __LINE__) will produce e.g. foo_123, whereas
// foo_##__LINE__ would result in foo___LINE__.
#define AZ_JOIN(a, b) AZ_JOIN2(a, b)
#define AZ_JOIN2(a, b) a##b

// AZ_COUNT_ARGS(...) expands to the number of arguments passed to it (as long
// as that number is between 1 and 15, inclusive).  Beware: it may silently
// give the wrong result if given 0 arguments or more than 15 arguments.
#define AZ_COUNT_ARGS(...) \
  AZ_SELECT_16TH(__VA_ARGS__, 15, 14, 13, 12, 11, 10, \
                 9, 8, 7, 6, 5, 4, 3, 2, 1, unused)
#define AZ_SELECT_16TH(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, ...) p

/*===========================================================================*/

#endif // MILESTONE_UTIL_MISC_H_
