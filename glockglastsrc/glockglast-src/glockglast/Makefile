CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -O2
SRC := main.c board.c pieces.c game.c
BIN := glockglast

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
else ifneq (,$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
	CFLAGS += -Iinclude
	LDFLAGS := lib/libraylib.a -lopengl32 -lgdi32 -lwinmm
else
	LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

.PHONY: all run clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)
