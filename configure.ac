# configure.ac
# 
# Process this file with autoconf to produce a configure script.
#
# Copyright 2013 Evan Buswell
# 
# This file is part of Sonic Maths.
# 
# Sonic Maths is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# Sonic Maths is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.

# define version information
m4_define(SONICMATHS_VERSION, [0.1])

m4_define(SONICMATHS_INTERFACE_REVISION, 0)
m4_define(SONICMATHS_INTERFACE_BACK_COMPATABILITY, 0)
m4_define(SONICMATHS_SOURCE_REVISION, 0)


AC_PREREQ([2.65])
AC_INIT([sonicmaths], SONICMATHS_VERSION, [ebuswell@gmail.com])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR([src/graph.c])
AC_CONFIG_HEADERS([src/config.h])

VERSION_INFO=SONICMATHS_INTERFACE_REVISION:SONICMATHS_SOURCE_REVISION:SONICMATHS_INTERFACE_BACK_COMPATABILITY
AC_SUBST(VERSION_INFO)

[SONICMATHS_VERSION]=SONICMATHS_VERSION
AC_SUBST([SONICMATHS_VERSION])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([ATOMICKIT], [atomickit], [HAVE_ATOMICKIT=true], [AC_MSG_ERROR([atomickit is required to build AC_PACKAGE_NAME])])
PKG_CHECK_MODULES([GRAPHLINE], [graphline], [HAVE_GRAPHLINE=true], [AC_MSG_ERROR([graphline is required to build AC_PACKAGE_NAME])])
PKG_CHECK_MODULES([JACK], [jack], [HAVE_JACK=true], [AC_MSG_ERROR([libjack is required to build AC_PACKAGE_NAME])])
PKG_CHECK_MODULES([SNDFILE], [sndfile >= 1.0.2], [HAVE_SNDFILE=true], [AC_MSG_ERROR([libsndfile [(>= 1.0.2)] is required to build AC_PACKAGE_NAME])])

# Checks for language features.
AC_LANG([C])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile sonicmaths.pc])

AC_OUTPUT
