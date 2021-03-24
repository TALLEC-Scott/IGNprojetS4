CFLAGS = `pkg-config --cflags gtk+-3.0` -rdynamic -Wall -Wextra -std=c99
LDLIBS = `pkg-config --libs gtk+-3.0` -lSDL_gfx -lSDL -lm -Wno-stringop-overflow
EXE = app
SRC = app.c

all: $(SRC)
	gcc $(SRC) -o $(EXE) $(CFLAGS) $(LDLIBS)

clean:
	rm -f $(EXE)
