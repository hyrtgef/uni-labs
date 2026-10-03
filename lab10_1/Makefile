CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O2
SRC =src/main.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(OBJ) -o $@

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJ) $(TARGET)