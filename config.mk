# sacf version
VERSION = 1.6

# sacf git exact commit version
SRCVERSION = $$(git describe --tags --dirty=[modified] 2>/dev/null || echo ${VERSION}-nogit)

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -DVERSION=\"${SRCVERSION}\"
CFLAGS  = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra -O3 ${INCS} ${CPPFLAGS} -flto -fsanitize=address,undefined,leak
LDFLAGS =
#LDFLAGS = -g ${CFLAGS}

# compiler and linker
CC = cc
