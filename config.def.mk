PREFIX?=/usr/local
INCLUDEDIR?=${PREFIX}/include
LIBDIR?=${PREFIX}/lib
DESTDIR?=
PKGCONFIGDIR?=${LIBDIR}/pkgconfig

CC?=gcc
CFLAGS?=-Og -g3
LDFLAGS?=
AR?=ar
ARFLAGS?=rv

ATOMICKIT_CFLAGS=${shell pkg-config --cflags atomickit}
ATOMICKIT_LIBS=${shell pkg-config --libs atomickit}
ATOMICKIT_STATIC=${shell pkg-config --static atomickit}
WITCH_CFLAGS=${shell pkg-config --cflags libwitch}
WITCH_LIBS=${shell pkg-config --libs libwitch}
WITCH_STATIC=${shell pkg-config --static libwitch}
JACK_CFLAGS=${shell pkg-config --cflags jack}
JACK_LIBS=${shell pkg-config --libs jack}
JACK_STATIC=${shell pkg-config --static jack}

CFLAGS+=${JACK_CFLAGS} ${WITCH_CFLAGS} ${ATOMICKIT_CFLAGS}
CFLAGS+=-Wall -Wextra -Wmissing-prototypes -Wredundant-decls \
        -Wdouble-promotion
CFLAGS+=-fplan9-extensions
CFLAGS+=-Iinclude

LIBS=${JACK_LIBS} ${WITCH_LIBS} ${ATOMICKIT_LIBS}
STATIC=${JACK_STATIC} ${WITCH_STATIC} ${ATOMICKIT_STATIC}
