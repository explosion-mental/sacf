# mage version
VERSION = 1.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
CFLAGS += -g -std=c99 -pedantic -Wall ${INCS} ${CPPFLAGS}
LDFLAGS += -g
#CFLAGS += -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#LDFLAGS +=

# compiler and linker
CC = cc
