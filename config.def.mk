PREFIX?=/usr/local
INCLUDEDIR?=${PREFIX}/include
LIBDIR?=${PREFIX}/lib
DESTDIR?=
PKGCONFIGDIR?=${LIBDIR}/pkgconfig
SHAREDSUFFIX?=.so
STATICSUFFIX?=.a
VERSIONPOS=suffix

CC=cc
CFLAGS?=-fPIC -O3 -ffast-math -freciprocal-math -fno-trapping-math \
	-mfpmath=sse,387 -mpc32
LDFLAGS?=
AR?=ar
ARFLAGS?=rv

CFLAGS+=-Wall -Wextra -Wmissing-prototypes -Wredundant-decls \
	-Wdouble-promotion
CFLAGS+=-Iinclude

LIBS=-lm
STATIC=-lm
