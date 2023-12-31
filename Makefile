# Makefile

CPPFLAGS = -Iinclude #-MMD
CC = gcc
CFLAGS = $(shell pkg-config --cflags sdl2 gtk+-3.0) -rdynamic -Wall -Wextra -Wno-deprecated-declarations -std=c99 -g
LDFLAGS =
LDLIBS = -lm -lGL -lGLU -lglut -Wno-stringop-overflow $(shell pkg-config --libs sdl2 gtk+-3.0) -llept -ltesseract

EXE = 223D
SRC = ${wildcard Application/app.c MapProcessing/*.c Modelling/*.c Tools/*.c} 
OBJ = ${SRC:.c=.o}

all: app

app:${OBJ}
	$(CC) $(SRC) $(CFLAGS) $(CPPFLAGS) -o $(EXE) $(LDLIBS) $(LDFLAGS)

.PHONY:
	clean

clean:
	$(RM) $(OBJ)
	$(RM) $(EXE)

# END
