# scaf - simple auto cpu freq
# See LICENSE file for copyright and license details.

include config.mk

SRC = sacf.c util.c
OBJ = ${SRC:.c=.o}

# detect if the user chose GCC or Clang
ifeq ($(CC),gcc)

	CC  	= gcc
        CFLAGS += -Wextra -flto
	LINKER 	= gcc

ifeq ($(DEBUG),true)
	# gcc-specific security/debug flags
	CFLAGS += -fanalyzer -ggdb

endif #debug

else ifeq ($(CC),clang)

	CC  	= clang
	CFLAGS += -Weverything -flto
	LINKER 	= clang

ifeq ($(DEBUG),true)
	# clang-specific security/debug flags
        CFLAGS += -fsanitize=undefined,signed-integer-overflow,null,alignment,address,leak,cfi \
	-fsanitize-undefined-trap-on-error -ftrivial-auto-var-init=pattern \
	-fvisibility=hidden  
	
	LDFLAGS  += -fsanitize=address
endif #debug

endif #compiler
ifeq ($(DEBUG),true)
        CFLAGS +=        -fno-omit-frame-pointer -fstack-clash-protection -D_FORTIFY_SOURCE=2 \
			  -fcf-protection -fstack-protector-all -fexceptions -fasynchronous-unwind-tables \
			  -Werror=format-security -D_DEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin
	LDFLAGS += -flto -fPIE -pie -Wl,-z,relro -Wl,--as-needed -Wl,-z,now \
			  -Wl,-z,noexecstack
endif

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
