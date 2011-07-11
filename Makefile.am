# Makefile.am
# 
# Copyright 2010 Evan Buswell
# 
# This file is part of Sonic Maths.
# 
# Sonic Maths is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 2.
# 
# Sonic Maths is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.

MAINTAINERCLEANFILES = aclocal.m4 config.guess config.sub configure depcomp install-sh ltmain.sh Makefile.in missing src/config.h.in~
EXTRA_DIST = autogen.sh Doxyfile
AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libsonicmaths.la
libsonicmaths_la_SOURCES = src/jbridge.c src/graph.c src/parameter.c src/synth.c src/sine.c src/envelope-generator.c src/controller.c src/instrument.c src/scheduler.c src/clock.c src/key.c
libsonicmaths_la_LIBADD = $(JACK_LIBS) $(SNDFILE_LIBS) $(ATOMICKIT_LIBS) $(GRAPHLINE_LIBS)
libsonicmaths_la_LDFLAGS = $(LDFLAGS) -L$(libdir) -export-dynamic -version_info @VERSION_INFO@
libsonicmaths_la_CFLAGS = $(CFLAGS) $(JACK_CFLAGS) $(SNDFILE_CFLAGS) $(ATOMICKIT_CFLAGS) $(GRAPHLINE_CFLAGS) -Wall -Wextra -Iinclude
libsonicmathsincludedir = $(pkgincludedir)
libsonicmathsinclude_HEADERS = include/sonicmaths/jbridge.h include/sonicmaths/graph.h include/sonicmaths/parameter.h include/sonicmaths/synth.h include/sonicmaths/sine.h include/sonicmaths/envelope-generator.h include/sonicmaths/controller.h include/sonicmaths/instrument.h include/sonicmaths/scheduler.h include/sonicmaths/clock.h include/sonicmaths/key.h

pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = sonicmaths.pc

check_PROGRAMS = test
test_SOURCES = src/test.c
test_LDADD = $(JACK_LIBS) $(SNDFILE_LIBS) $(ATOMICKIT_LIBS) $(GRAPHLINE_LIBS) -lsonicmaths
test_LDFLAGS = $(LD_FLAGS) -rpath $(libdir)
test_CFLAGS = $(CFLAGS) $(JACK_CFLAGS) $(SNDFILE_CFLAGS) $(ATOMICKIT_CFLAGS) $(GRAPHLINE_CFLAGS) -Wall -Wextra -Iinclude
TESTS = test
