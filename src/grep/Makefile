CC      = gcc
CFLAGS  = -Wall -Werror -Wextra -std=c11 -pedantic
TARGET  = s21_grep
SRC     = s21_grep.c
HDR     = s21_grep.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

rebuild: clean all

test: $(TARGET)
	@bash ../tests/test_grep.sh

.PHONY: all clean rebuild test
