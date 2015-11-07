.PHONY: shared static all install-headers install-pkgconfig install-shared \
	install-static install-static-strip install-shared-strip \
	install-all-static install-all-shared install-all-static-strip \
	install-all-shared-strip install install-strip uninstall clean \
        check-shared check-static check

.SUFFIXES: .o

include config.mk

VERSION=0.3

SRCS=src/clock.c src/cosine.c src/delay.c src/differentiator.c \
     src/envelope-generator.c src/filter.c src/fdmodulator.c \
     src/impulse-train.c src/integrator.c src/key.c src/lag.c src/limit.c \
     src/oscillator.c src/quantize.c src/random.c src/reverb.c \
     src/sample-and-hold.c

TESTSRCS=

HEADERS=sonicmaths/clock.h sonicmaths/cosine.h sonicmaths/delay.h \
	sonicmaths/differentiator.h sonicmaths/envelope-generator.h \
	sonicmaths/fdmodulator.h sonicmaths/filter.h \
	sonicmaths/impulse-train.h sonicmaths/integrator.h sonicmaths/key.h \
	sonicmaths/lag.h sonicmaths/limit.h sonicmaths/math.h \
	sonicmaths/oscillator.h sonicmaths/quantize.h sonicmaths/random.h \
	sonicmaths/reverb.h sonicmaths/sample-and-hold.h \
	sonicmaths.h

OBJS=${SRCS:.c=.o}
TESTOBJS=${TESTSRCS:.c=.o}

MAJOR=${shell echo ${VERSION}|cut -d . -f 1}

VERSIONSUFFIX=${shell echo 'prefix${SHAREDSUFFIX}suffix' \
	        | sed -e 's/${VERSIONPOS}/.${VERSION}/' -e 's/prefix//g' -e 's/suffix//g'}
MAJORSUFFIX=${shell echo 'prefix${SHAREDSUFFIX}suffix' \
	      | sed -e 's/${VERSIONPOS}/.${MAJOR}/' -e 's/prefix//g' -e 's/suffix//g'}

all: shared sonicmaths.pc

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

libsonicmaths${SHAREDSUFFIX}: ${OBJS}
	${CC} ${CFLAGS} -fPIC ${LDFLAGS} -shared ${OBJS} ${LIBS} \
	      -o libsonicmaths${SHAREDSUFFIX}

libsonicmaths${STATICSUFFIX}: ${OBJS}
	rm -f libsonicmaths${STATICSUFFIX}
	${AR} ${ARFLAGS}c libsonicmaths${STATICSUFFIX} ${OBJS}
	ranlib libsonicmaths${STATICSUFFIX}

unittest-shared: libsonicmaths${SHAREDSUFFIX} ${TESTOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -L`pwd` -Wl,-rpath,`pwd` \
	      ${TESTOBJS} ${LIBS} -lsonicmaths -o unittest-shared

unittest-static: libsonicmaths${STATICSUFFIX} ${TESTOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -static -L`pwd` \
	      ${TESTOBJS} ${STATIC} -lsonicmaths -o unittest-static

sonicmaths.pc: sonicmaths.pc.in config.mk Makefile
	sed -e 's!@prefix@!${PREFIX}!g' \
	    -e 's!@libdir@!${LIBDIR}!g' \
	    -e 's!@includedir@!${INCLUDEDIR}!g' \
	    -e 's!@version@!${VERSION}!g' \
	    sonicmaths.pc.in >sonicmaths.pc

shared: libsonicmaths${SHAREDSUFFIX}

static: libsonicmaths${STATICSUFFIX}

install-headers:
	(umask 022; mkdir -p ${DESTDIR}${INCLUDEDIR}/sonicmaths)
	for HEADER in ${HEADERS}; do \
		install -m 644 include/$$HEADER \
			${DESTDIR}${INCLUDEDIR}/$$HEADER; \
	done

install-pkgconfig: sonicmaths.pc
	(umask 022; mkdir -p ${DESTDIR}${PKGCONFIGDIR})
	install -m 644 sonicmaths.pc ${DESTDIR}${PKGCONFIGDIR}/sonicmaths.pc

install-shared: shared
	(umask 022; mkdir -p ${DESTDIR}${LIBDIR})
	install -m 755 libsonicmaths${SHAREDSUFFIX} \
	        ${DESTDIR}${LIBDIR}/libsonicmaths${VERSIONSUFFIX}
	(cd ${DESTDIR}${LIBDIR}; \
	 ln -fs libsonicmaths${VERSIONSUFFIX} \
	        libsonicmaths${MAJORSUFFIX}; \
	 ln -fs libsonicmaths${VERSIONSUFFIX} \
	        libsonicmaths${SHAREDSUFFIX})

install-static: static
	(umask 022; mkdir -p ${DESTDIR}${LIBDIR})
	install -m 644 libsonicmaths${STATICSUFFIX} \
		${DESTDIR}${LIBDIR}/libsonicmaths${STATICSUFFIX}

install-shared-strip: install-shared
	strip --strip-unneeded ${DESTDIR}${LIBDIR}/libsonicmaths${VERSIONSUFFIX}

install-static-strip: install-static
	strip --strip-unneeded ${DESTDIR}${LIBDIR}/libsonicmaths${STATICSUFFIX}

install-all-static: static sonicmaths.pc install-static install-headers \
		    install-pkgconfig

install-all-shared: shared sonicmaths.pc install-shared install-headers \
                    install-pkgconfig

install-all-shared-strip: install-all-shared install-shared-strip

install-all-static-strip: install-all-static install-static-strip

install: install-all-shared

install-strip: install-all-shared-strip

uninstall: 
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths${VERSIONSUFFIX}
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths${MAJORSUFFIX}
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths${SHAREDSUFFIX}
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths${STATICSUFFIX}
	rm -f ${DESTDIR}${PKGCONFIGDIR}/sonicmaths.pc
	rm -rf ${DESTDIR}${INCLUDEDIR}/sonicmaths
	for HEADER in ${HEADERS}; do \
		rm -f ${DESTDIR}${INCLUDEDIR}/$$HEADER; \
	done

clean:
	rm -f sonicmaths.pc
	rm -f libsonicmaths${SHAREDSUFFIX}
	rm -f libsonicmaths${STATICSUFFIX}
	rm -f ${OBJS}
	rm -f ${TESTOBJS}
	rm -f unittest-shared
	rm -f unittest-static

check-shared: unittest-shared
	./unittest-shared

check-static: unittest-static
	./unittest-static

check: check-shared
