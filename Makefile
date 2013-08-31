#=============================================================================#
# Copyright 2013 Matthew D. Steele <mdsteele@alum.mit.edu>                    #
#                                                                             #
# This file is part of Milestone.                                             #
#                                                                             #
# Milestone is free software: you can redistribute it and/or modify it under  #
# the terms of the GNU General Public License as published by the Free        #
# Software Foundation, either version 3 of the License, or (at your option)   #
# any later version.                                                          #
#                                                                             #
# Milestone is distributed in the hope that it will be useful, but WITHOUT    #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       #
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   #
# more details.                                                               #
#                                                                             #
# You should have received a copy of the GNU General Public License along     #
# with Milestone.  If not, see <http://www.gnu.org/licenses/>.                #
#=============================================================================#

SRCDIR = src
DATADIR = data
OUTDIR = out
OBJDIR = $(OUTDIR)/obj
BINDIR = $(OUTDIR)/bin

CFLAGS = -Wall -Wempty-body -Winline -Wmissing-field-initializers \
         -Wold-style-definition -Woverride-init -Wshadow -Wsign-compare \
         -Wstrict-prototypes -Wundef -Werror -O1 -I$(SRCDIR)
C99FLAGS = -std=c99 -pedantic $(CFLAGS)

define compile-sys
	@echo "Compiling $@"
	@mkdir -p $(@D)
	@gcc -o $@ -c $< $(CFLAGS)
endef
define compile-c99
	@echo "Compiling $@"
	@mkdir -p $(@D)
	@gcc -o $@ -c $< $(C99FLAGS)
endef

#=============================================================================#
# Determine what OS we're on and what targets we're building.

OS_NAME := $(shell uname)
ifeq "$(OS_NAME)" "Darwin"
  CFLAGS += -I$(SRCDIR)/macosx
  MAIN_LIBFLAGS = -framework Cocoa -framework OpenGL \
                  -framework SDL -framework SDL_mixer
  SYSTEM_OBJFILES = $(OBJDIR)/macosx/SDLMain.o
else
  MAIN_LIBFLAGS = -lGL -lSDL -lSDL_mixer
  SYSTEM_OBJFILES =
endif

#=============================================================================#
# Find all of the source files:

AZ_GUI_HEADERS := $(shell find $(SRCDIR)/milestone/gui -name '*.h')
AZ_UTIL_HEADERS := $(shell find $(SRCDIR)/milestone/util -name '*.h') \
                   $(SRCDIR)/milestone/constants.h
AZ_STATE_HEADERS := $(shell find $(SRCDIR)/milestone/state -name '*.h')
AZ_TICK_HEADERS := $(shell find $(SRCDIR)/milestone/tick -name '*.h')
AZ_VIEW_HEADERS := $(shell find $(SRCDIR)/milestone/view -name '*.h')

AZ_GUI_C99FILES := $(shell find $(SRCDIR)/milestone/gui -name '*.c')
AZ_UTIL_C99FILES := $(shell find $(SRCDIR)/milestone/util -name '*.c')
AZ_STATE_C99FILES := $(shell find $(SRCDIR)/milestone/state -name '*.c')
AZ_TICK_C99FILES := $(shell find $(SRCDIR)/milestone/tick -name '*.c')
AZ_VIEW_C99FILES := $(shell find $(SRCDIR)/milestone/view -name '*.c')

MAIN_C99FILES := $(AZ_UTIL_C99FILES) $(AZ_STATE_C99FILES) $(AZ_TICK_C99FILES) \
	         $(AZ_GUI_C99FILES) $(AZ_VIEW_C99FILES) \
                 $(SRCDIR)/milestone/main.c

MAIN_OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(MAIN_C99FILES)) \
                 $(SYSTEM_OBJFILES)

#=============================================================================#
# Build rules for linking the executable:

$(BINDIR)/milestone: $(MAIN_OBJFILES)
	@echo "Linking $@"
	@mkdir -p $(@D)
	@gcc -o $@ $^ $(CFLAGS) $(MAIN_LIBFLAGS)

#=============================================================================#
# Build rules for compiling system-specific code:

$(OBJDIR)/macosx/SDLMain.o: $(SRCDIR)/macosx/SDLMain.m \
                            $(SRCDIR)/macosx/SDLMain.h
	$(compile-sys)

#=============================================================================#
# Build rules for compiling non-system-specific code:

$(OBJDIR)/milestone/util/%.o: $(SRCDIR)/milestone/util/%.c $(AZ_UTIL_HEADERS)
	$(compile-c99)

$(OBJDIR)/milestone/state/%.o: $(SRCDIR)/milestone/state/%.c \
    $(AZ_UTIL_HEADERS) $(AZ_STATE_HEADERS)
	$(compile-c99)

$(OBJDIR)/milestone/tick/%.o: $(SRCDIR)/milestone/tick/%.c \
    $(AZ_UTIL_HEADERS) $(AZ_STATE_HEADERS) $(AZ_TICK_HEADERS)
	$(compile-c99)

$(OBJDIR)/milestone/gui/%.o: $(SRCDIR)/milestone/gui/%.c \
    $(AZ_UTIL_HEADERS) $(AZ_GUI_HEADERS)
	$(compile-c99)

$(OBJDIR)/milestone/view/%.o: $(SRCDIR)/milestone/view/%.c \
    $(AZ_UTIL_HEADERS) $(AZ_STATE_HEADERS) $(AZ_GUI_HEADERS) $(AZ_VIEW_HEADERS)
	$(compile-c99)

$(OBJDIR)/milestone/main.o: $(SRCDIR)/milestone/main.c \
    $(AZ_UTIL_HEADERS) $(AZ_STATE_HEADERS) $(AZ_TICK_HEADERS) \
    $(AZ_GUI_HEADERS) $(AZ_VIEW_HEADERS)
	$(compile-c99)

#=============================================================================#
# Convenience build targets:

.PHONY: run
run: $(BINDIR)/milestone
	$(BINDIR)/milestone

.PHONY: clean
clean:
	rm -rf $(OUTDIR)

.PHONY: tidy
tidy:
	find $(SRCDIR) -name '*~' -print0 | xargs -0 rm

.PHONY: wc
wc:
	find $(SRCDIR) \( -name '*.c' -or -name '*.h' \) -print0 | \
	    xargs -0 wc -l

#=============================================================================#
