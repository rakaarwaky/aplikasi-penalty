# Makefile untuk C-AES Penalty Shootout (C99, AES v3.0)
CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2 -g
TARGET  := penalty_shootout

SRC_DIR   := src
BUILD_DIR := build

C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

INCLUDES := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
            -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
            -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
            -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(C_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(C_OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
