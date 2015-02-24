PREFIX?=/usr/local
INCLUDEDIR?=${PREFIX}/include
LIBDIR?=${PREFIX}/lib
DESTDIR?=
PKGCONFIGDIR?=${LIBDIR}/pkgconfig

CC=gcc-4.9
CFLAGS?=-O2
LDFLAGS?=
AR?=ar
ARFLAGS?=rv

CFLAGS+=-Wall -Wextra -Wmissing-prototypes -Wredundant-decls \
        -Wdouble-promotion
CFLAGS+=-Iinclude

LIBS=-lfftw3f -lm
STATIC=-lfftw3f -lm
