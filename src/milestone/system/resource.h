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
#ifndef MILESTONE_SYSTEM_RESOURCE_H_
#define MILESTONE_SYSTEM_RESOURCE_H_

/*===========================================================================*/

// Get the path to the user-specific directory for storing persistent data for
// this application (e.g. preferences or save files) as a NUL-terminated string
// (without the trailing slash).  If the directory doesn't already exist,
// create it.  If anything fails, this will return a NULL pointer.
const char *az_get_app_data_directory(void);

// Get the path to the directory containing the game's resource files as a
// NUL-terminated string (without the trailing slash).  If this fails, it will
// return a NULL pointer.
const char *az_get_resource_directory(void);

/*===========================================================================*/

#endif // MILESTONE_SYSTEM_RESOURCE_H_
