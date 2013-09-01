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

#include "milestone/system/resource.h"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>
#include <Foundation/Foundation.h>

/*===========================================================================*/

const char *az_get_app_data_directory(void) {
  static char path_buffer[PATH_MAX];
  if (path_buffer[0] == '\0') {
    // Get the path to the user's Application Support directory.
    NSArray *array =
      NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,
                                          NSUserDomainMask, YES);
    if ([array count] <= 0) return NULL;
    NSString *path_string = [[array objectAtIndex: 0]
                              stringByAppendingString: @"/Milestone"];
    // Create the directory (if it doesn't already exist).
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager createDirectoryAtPath: path_string
                      withIntermediateDirectories: YES
                      attributes: nil
                      error: nil]) {
      return NULL;
    }
    // Copy the directory path into the buffer.
    if (![path_string getCString: path_buffer
                      maxLength: sizeof(path_buffer)
                      encoding: NSUTF8StringEncoding]) {
      path_buffer[0] = '\0';
      return NULL;
    }
  }
  return path_buffer;
}

const char *az_get_resource_directory(void) {
  static unsigned char path[PATH_MAX];
  if (path[0] == '\0') {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
    if (!CFURLGetFileSystemRepresentation(url, YES, path, sizeof(path))) {
      path[0] = '\0';
      return NULL;
    }
  }
  return (char *)path;
}

/*===========================================================================*/
