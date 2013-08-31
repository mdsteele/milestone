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

#include "milestone/gui/screen.h"

#include <assert.h>
#include <stdbool.h>

#include <GL/gl.h>
#include <SDL/SDL.h>

#include "milestone/constants.h"
#include "milestone/gui/audio.h"
#include "milestone/util/misc.h"

/*===========================================================================*/

#ifdef __APPLE__
// The UTF-8 sequence for the Apple command key symbol (U+2318):
#define CMD_KEY_NAME "\xE2\x8C\x98"
#else
#define CMD_KEY_NAME "Ctrl"
#endif

#define VIDEO_DEPTH 32
#define VIDEO_FLAGS (SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)

static bool sdl_initialized = false;
static bool display_initialized = false;
static bool currently_fullscreen;
static int num_gl_init_funcs = 0;
static az_init_func_t gl_init_funcs[8];

void az_register_gl_init_func(az_init_func_t func) {
  assert(!sdl_initialized);
  if (num_gl_init_funcs >= AZ_ARRAY_SIZE(gl_init_funcs)) {
    AZ_FATAL("gl_init_funcs array is full.\n");
  } else {
    gl_init_funcs[num_gl_init_funcs++] = func;
  }
}

void az_init_gui(bool fullscreen) {
  assert(!sdl_initialized);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    AZ_FATAL("SDL_Init failed.\n");
  }
  atexit(SDL_Quit);
  az_init_audio_mixer();
  SDL_WM_SetCaption("Milestone (press " CMD_KEY_NAME "-M to run full-screen)",
                    "Milestone");
  sdl_initialized = true;
  az_set_fullscreen(fullscreen);
  SDL_ShowCursor(SDL_DISABLE);
}

bool az_is_fullscreen(void) {
  assert(sdl_initialized);
  assert(display_initialized);
  return currently_fullscreen;
}

void az_set_fullscreen(bool fullscreen) {
  assert(sdl_initialized);
  if (display_initialized && fullscreen == currently_fullscreen) return;
  currently_fullscreen = fullscreen;
  az_pause_all_audio();
  // Enable OpenGL double-buffering:
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // Enable vsync:
#if SDL_VERSION_ATLEAST(1,3,0)
  SDL_GL_SetSwapInterval(1);
#else
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#endif
  // Enable antialiasing:
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

  // Init the display:
  int x = 0, y = 0;
  if (display_initialized) {
    SDL_GetMouseState(&x, &y);
  }
  if (!SDL_SetVideoMode(AZ_SCREEN_WIDTH, AZ_SCREEN_HEIGHT, VIDEO_DEPTH,
                        VIDEO_FLAGS | (fullscreen ? SDL_FULLSCREEN : 0))) {
    AZ_FATAL("SDL_SetVideoMode failed.\n");
  }
  if (display_initialized) {
    SDL_WarpMouse(x, y);
  }

  // Enable unicode values for keyboard events:
  SDL_EnableUNICODE(1);

  // Turn off the depth buffer:
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  // Enable alpha blending:
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Enable antialiasing:
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  // Set the view:
  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0f);
  glViewport(0, 0, AZ_SCREEN_WIDTH, AZ_SCREEN_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, AZ_SCREEN_WIDTH, AZ_SCREEN_HEIGHT, 0, 1, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Run GL init functions:
  for (int i = 0; i < num_gl_init_funcs; ++i) {
    gl_init_funcs[i]();
  }

  display_initialized = true;
  az_unpause_all_audio();
}

void az_start_screen_redraw(void) {
  assert(sdl_initialized);
  assert(display_initialized);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
}

void az_finish_screen_redraw(void) {
  assert(sdl_initialized);
  assert(display_initialized);
  glFlush(); // Are the flush and finish at all necessary?  I'm not sure.
  glFinish();
  SDL_GL_SwapBuffers();
}

/*===========================================================================*/
