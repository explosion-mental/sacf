# scaf - simple auto cpu freq
# See LICENSE file for copyright and license details.

include config.mk

SRC = sacf.c util.c
OBJ = ${SRC:.c=.o}

all: options sacf

options:
	@echo sacf build options:
	@echo "VERSION  = ${VERSION}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

config.h:
	cp config.def.h config.h

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

sacf: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f sacf ${OBJ} sacf-${VERSION}.tar.gz

dist: clean
	mkdir -p sacf-${VERSION}
	cp -R LICENSE Makefile README.md config.mk config.def.h ${SRC} sacf.1 util.h sacf-${VERSION}
	tar -cf sacf-${VERSION}.tar sacf-${VERSION}
	gzip sacf-${VERSION}.tar
	rm -rf sacf-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f sacf ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/sacf
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < sacf.1 > ${DESTDIR}${MANPREFIX}/man1/sacf.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/sacf.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/sacf \
		${DESTDIR}${MANPREFIX}/man1/sacf.1

.PHONY: all options clean dist install uninstall
