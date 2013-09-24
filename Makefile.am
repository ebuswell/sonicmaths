# Makefile.am
# 
# Copyright 2013 Evan Buswell
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

MAINTAINERCLEANFILES = aclocal.m4 config.guess config.sub configure depcomp install-sh ltmain.sh Makefile.in missing src/config.h.in~ compile
EXTRA_DIST = autogen.sh Doxyfile sonicmaths.pc.in
AUTOMAKE_OPTIONS = foreign serial-tests subdir-objects

lib_LTLIBRARIES = libsonicmaths.la
libsonicmaths_la_SOURCES = src/jbridge.c src/graph.c src/parameter.c src/buffer.c src/mtrand.c src/synth.c src/sine.c src/controller.c src/instrument.c src/envelope-generator.c src/key.c src/mixer.c src/noise.c src/filter.c src/distortion.c src/integrator.c src/sawtooth.c src/parabola.c src/impulse-train.c src/portamento.c src/second-order.c src/bandpass.c src/lowpass.c src/highpass.c src/notch.c src/square.c src/triangle.c src/midi.c src/modulator.c
# libsonicmaths_la_SOURCES = src/jbridge.c src/graph.c src/parameter.c src/synth.c src/sine.c src/envelope-generator.c src/controller.c src/instrument.c src/scheduler.c src/clock.c src/key.c src/jmidi.c src/mixer.c src/noise.c src/filter.c src/distortion.c src/impulse-train.c src/lowpass.c src/bandpass.c src/highpass.c src/notch.c src/integrator.c src/modulator.c src/portamento.c src/dsf.c src/sawtooth.c src/parabola.c src/triangle.c src/square.c
libsonicmaths_la_LIBADD = $(JACK_LIBS) $(SNDFILE_LIBS) $(ATOMICKIT_LIBS) $(GRAPHLINE_LIBS)
libsonicmaths_la_LDFLAGS = $(LDFLAGS) -L$(libdir) -export-dynamic -version_info @VERSION_INFO@
libsonicmaths_la_CFLAGS = $(CFLAGS) $(JACK_CFLAGS) $(SNDFILE_CFLAGS) $(ATOMICKIT_CFLAGS) $(GRAPHLINE_CFLAGS) -fplan9-extensions -Og -g3 -Wall -Wextra -Wmissing-prototypes -Wredundant-decls -Wdouble-promotion -Iinclude
libsonicmathsincludedir = $(pkgincludedir)
libsonicmathsinclude_HEADERS = include/sonicmaths/jbridge.h include/sonicmaths/graph.h include/sonicmaths/parameter.h include/sonicmaths/buffer.h include/sonicmaths/synth.h include/sonicmaths/sine.h include/sonicmaths/controller.h include/sonicmaths/instrument.h include/sonicmaths/envelope-generator.h include/sonicmaths/key.h include/sonicmaths/mixer.h include/sonicmaths/noise.h include/sonicmaths/filter.h include/sonicmaths/distortion.h include/sonicmaths/integrator.h include/sonicmaths/impulse-train.h include/sonicmaths/sawtooth.h include/sonicmaths/parabola.h include/sonicmaths/impulse-train.h include/sonicmaths/portamento.h include/sonicmaths/second-order.h include/sonicmaths/bandpass.h include/sonicmaths/lowpass.h include/sonicmaths/highpass.h include/sonicmaths/notch.h include/sonicmaths/square.h include/sonicmaths/triangle.h include/sonicmaths/midi.h include/sonicmaths/modulator.h
# libsonicmathsinclude_HEADERS = include/sonicmaths/jbridge.h include/sonicmaths/graph.h include/sonicmaths/parameter.h include/sonicmaths/synth.h include/sonicmaths/sine.h include/sonicmaths/envelope-generator.h include/sonicmaths/controller.h include/sonicmaths/instrument.h include/sonicmaths/scheduler.h include/sonicmaths/clock.h include/sonicmaths/key.h include/sonicmaths/jmidi.h include/sonicmaths/mixer.h include/sonicmaths/noise.h include/sonicmaths/filter.h include/sonicmaths/distortion.h include/sonicmaths/impulse-train.h include/sonicmaths/lowpass.h include/sonicmaths/bandpass.h include/sonicmaths/highpass.h include/sonicmaths/notch.h include/sonicmaths/integrator.h include/sonicmaths/modulator.h include/sonicmaths/portamento.h include/sonicmaths/dsf.h include/sonicmaths/sawtooth.h include/sonicmaths/parabola.h include/sonicmaths/triangle.h include/sonicmaths/square.h

pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = sonicmaths.pc

check_PROGRAMS = unittest
unittest_SOURCES = src/test.c
unittest_LDADD = $(JACK_LIBS) $(SNDFILE_LIBS) $(ATOMICKIT_LIBS) $(GRAPHLINE_LIBS) -lsonicmaths
unittest_LDFLAGS = $(LD_FLAGS) -rpath $(libdir)
unittest_CFLAGS = $(CFLAGS) $(JACK_CFLAGS) $(SNDFILE_CFLAGS) $(ATOMICKIT_CFLAGS) $(GRAPHLINE_CFLAGS) -fplan9-extensions -Og -g3 -Wall -Wextra -Wmissing-prototypes -Wredundant-decls -Wdouble-promotion -Iinclude
TESTS = unittest
