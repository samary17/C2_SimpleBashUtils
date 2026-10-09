CC      = gcc
CFLAGS  = -Wall -Werror -Wextra -std=c11 -pedantic
TARGET  = s21_cat
SRC     = s21_cat.c
HDR     = s21_cat.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

rebuild: clean all

test: $(TARGET)
	@bash ../tests/test_cat.sh

.PHONY: all clean rebuild test
