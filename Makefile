# Makefile untuk C-AES Penalty Shootout (C99, AES v3.0)
CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2 -g
TARGET  := penalty_shootout

SRC_DIR := src
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)

INCLUDES := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
            -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
            -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
            -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(C_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(C_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(C_OBJECTS) $(TARGET)
