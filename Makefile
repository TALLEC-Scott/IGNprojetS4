# Makefile

CPPFLAGS = -MMD
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 $(shell pkg-config --cflags sdl2 gtk+-3.0) -Iinclude
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 gtk+-3.0)

SRC = main.c Tools/tools.c MapProcessing/MapColorisation.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main:${OBJ}
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(SRC) -o main $(LDLIBS)

.PHONY: clean
clean:
	${RM} ${OBJ}   # remove object files
	${RM} ${DEP}   # remove dependency files
	${RM} main     # remove main program
	${RM} main.d

-include ${DEP}

# END
