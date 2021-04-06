# Makefile

CPPFLAGS = -Iinclude #-MMD
CC = gcc
CFLAGS = -g -fsanitize=address -Wall -Wextra -std=c99 $(shell pkg-config --cflags sdl2 gtk+-3.0)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 gtk+-3.0)

SRC = ${wildcard MapProcessing/*.c Tools/*.c  *.c}
OBJ = ${SRC:.c=.o}
#DEP = ${SRC:.c=.d}

all: main

main:${OBJ}
	$(CC) $(SRC) $(CFLAGS) $(CPPFLAGS) -o main $(LDLIBS) $(LDFLAGS)

.PHONY: clean
clean:
	${RM} ${OBJ}   # remove object files
#	${RM} ${DEP}   # remove dependency files
	${RM} main     # remove main program
#	${RM} main.d

#-include ${DEP}

# END
