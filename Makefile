.PHONY: shared static all install-headers install-pkgconfig install-shared \
	install-static install-static-strip install-shared-strip \
	install-all-static install-all-shared install-all-static-strip \
	install-all-shared-strip install install-strip uninstall clean \
        check-shared check-static check

.SUFFIXES: .o .pic.o

include config.mk

VERSION=0.2

SRCS=src/clock.c src/delay.c src/envelope-generator.c src/fdmodulator.c \
     src/integrator.c src/key.c src/lag.c src/math.c src/oscillator.c \
     src/random.c src/reverb.c src/sample-and-hold.c src/second-order.c \
     src/sequence.c src/sequence-gram.c src/sequence-lex.c src/shifter.c

GENSRCS=src/sequence-gram.c src/sequence-lex.c src/sequence-gram.h

TESTSRCS=

HEADERS=include/sonicmaths/bandpass.h include/sonicmaths/clock.h \
        include/sonicmaths/cosine.h include/sonicmaths/delay.h \
	include/sonicmaths/envelope-generator.h \
	include/sonicmaths/fdmodulator.h include/sonicmaths/highpass.h \
	include/sonicmaths/impulse-train.h include/sonicmaths/integrator.h \
	include/sonicmaths/key.h include/sonicmaths/lag.h \
	include/sonicmaths/limit.h include/sonicmaths/lowpass.h \
	include/sonicmaths/math.h include/sonicmaths/notch.h \
	include/sonicmaths/oscillator.h include/sonicmaths/quantize.h \
	include/sonicmaths/random.h include/sonicmaths/reverb.h \
	include/sonicmaths/sample-and-hold.h \
	include/sonicmaths/second-order.h include/sonicmaths/sequence.h \
	include/sonicmaths/shifter.h

OBJS=${SRCS:.c=.o}
PICOBJS=${SRCS:.c=.pic.o}
TESTOBJS=${TESTSRCS:.c=.o}

MAJOR=${shell echo ${VERSION}|cut -d . -f 1}

.SECONDARY: ${GENSRCS}

all: shared sonicmaths.pc

src/sequence-lex.c: src/sequence-lex.l src/sequence-gram.h
	${LEX} ${LEXFLAGS} -P smseq_ -o $@ $<

src/sequence-gram.c: src/sequence-gram.y
	${YACC} ${YACCFLAGS} -p smseq_ -d -o $@ $<

src/sequence-gram.h: src/sequence-gram.y
	${YACC} ${YACCFLAGS} -p smseq_ -d -o $@ $<

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

.c.pic.o:
	${CC} ${CFLAGS} -fPIC -c $< -o $@

libsonicmaths.so: ${PICOBJS}
	${CC} ${CFLAGS} -fPIC ${LDFLAGS} -shared ${PICOBJS} ${LIBS} \
	      -o libsonicmaths.so

libsonicmaths.a: ${OBJS}
	rm -f libsonicmaths.a
	${AR} ${ARFLAGS}c libsonicmaths.a ${OBJS}

unittest-shared: libsonicmaths.so ${TESTOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -L`pwd` -Wl,-rpath,`pwd` \
	      ${TESTOBJS} ${LIBS} -lsonicmaths -o unittest-shared

unittest-static: libsonicmaths.a ${TESTOBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -static -L`pwd` \
	      ${TESTOBJS} ${STATIC} -lsonicmaths -o unittest-static

sonicmaths.pc: sonicmaths.pc.in config.mk Makefile
	sed -e 's!@prefix@!${PREFIX}!g' \
	    -e 's!@libdir@!${LIBDIR}!g' \
	    -e 's!@includedir@!${INCLUDEDIR}!g' \
	    -e 's!@version@!${VERSION}!g' \
	    sonicmaths.pc.in >sonicmaths.pc

shared: libsonicmaths.so

static: libsonicmaths.a

install-headers:
	(umask 022; mkdir -p ${DESTDIR}${INCLUDEDIR}/sonicmaths)
	install -m 644 -t ${DESTDIR}${INCLUDEDIR}/sonicmaths ${HEADERS}

install-pkgconfig: sonicmaths.pc
	(umask 022; mkdir -p ${DESTDIR}${PKGCONFIGDIR})
	install -m 644 sonicmaths.pc ${DESTDIR}${PKGCONFIGDIR}/sonicmaths.pc

install-shared: shared
	(umask 022; mkdir -p ${DESTDIR}${LIBDIR})
	install -m 755 libsonicmaths.so \
	        ${DESTDIR}${LIBDIR}/libsonicmaths.so.${VERSION}
	ln -frs ${DESTDIR}${LIBDIR}/libsonicmaths.so.${VERSION} \
	        ${DESTDIR}${LIBDIR}/libsonicmaths.so.${MAJOR}
	ln -frs ${DESTDIR}${LIBDIR}/libsonicmaths.so.${VERSION} \
	        ${DESTDIR}${LIBDIR}/libsonicmaths.so

install-static: static
	(umask 022; mkdir -p ${DESTDIR}${LIBDIR})
	install -m 644 libsonicmaths.a ${DESTDIR}${LIBDIR}/libsonicmaths.a

install-shared-strip: install-shared
	strip --strip-unneeded ${DESTDIR}${LIBDIR}/libsonicmaths.so.${VERSION}

install-static-strip: install-static
	strip --strip-unneeded ${DESTDIR}${LIBDIR}/libsonicmaths.a

install-all-static: static sonicmaths.pc install-static install-headers \
                    install-pkgconfig

install-all-shared: shared sonicmaths.pc install-shared install-headers \
                    install-pkgconfig

install-all-shared-strip: install-all-shared install-shared-strip

install-all-static-strip: install-all-static install-static-strip

install: install-all-shared

install-strip: install-all-shared-strip

uninstall: 
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths.so.${VERSION}
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths.so.${MAJOR}
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths.so
	rm -f ${DESTDIR}${LIBDIR}/libsonicmaths.a
	rm -f ${DESTDIR}${PKGCONFIGDIR}/sonicmaths.pc
	rm -rf ${DESTDIR}${INCLUDEDIR}/sonicmaths

clean:
	rm -f sonicmaths.pc
	rm -f libsonicmaths.so
	rm -f libsonicmaths.a
	rm -f ${GENSRCS}
	rm -f ${OBJS}
	rm -f ${PICOBJS}
	rm -f ${TESTOBJS}
	rm -f unittest-shared
	rm -f unittest-static

check-shared: unittest-shared
	./unittest-shared

check-static: unittest-static
	./unittest-static

check: check-shared
