CC=gcc
CFLAGS=`pkg-config --cflags gtk+-3.0` -g -Wall
LDFLAGS=`pkg-config --libs gtk+-3.0`
SRC=main.c snake.c food.c level.c game.c leaderboard.c
OBJ=$(SRC:.c=.o)
TARGET=snakes

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
