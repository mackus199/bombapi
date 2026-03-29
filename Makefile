CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags libulfius)
LDFLAGS = $(shell pkg-config --libs libulfius) -ljansson

SRC = main.c endpoints.c
OUT = bombapi

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
