# Makefile untuk C-AES Penalty Shootout (C99, AES v3.0)
CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2 -g
TARGET  := aplikasi_perhitungan_penalty

SRC_DIR   := src
BUILD_DIR := build

C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

INCLUDES := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
            -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
            -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
            -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli \
            -I$(SRC_DIR)/tui -I$(SRC_DIR)/storage -I$(SRC_DIR)/sanitizer

LDFLAGS  := -lncurses

# Test
TEST_DIR   := tests
TEST_SRCS  := $(wildcard $(TEST_DIR)/*.c)
TEST_INC   := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
              -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
              -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
              -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli -I$(SRC_DIR)/tui \
              -I$(SRC_DIR)/storage -I$(SRC_DIR)/sanitizer
TEST_LIB_SRC := $(filter-out $(SRC_DIR)/root_cli_main_entry.c \
                  $(SRC_DIR)/cli/surfaces_menu_command.c \
                  $(SRC_DIR)/cli/surfaces_registration_command.c \
                  $(SRC_DIR)/cli/surfaces_scoring_command.c \
                  $(SRC_DIR)/cli/surfaces_ranking_command.c \
                  $(SRC_DIR)/cli/surfaces_search_command.c \
                  $(SRC_DIR)/cli/surfaces_recap_command.c \
                  $(SRC_DIR)/tui/infrastructure_tui_adapter.c, $(C_SOURCES))
TEST_BIN   := run_tests

.PHONY: all clean run test lint format check-format analyze valgrind

all: $(TARGET)

$(TARGET): $(C_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(C_OBJECTS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_LIB_SRC) $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_INC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_BIN)

# Lint (cppcheck jika tersedia)
lint:
	@if command -v cppcheck > /dev/null 2>&1; then \
		cppcheck --enable=all --suppress=missingIncludeSystem src/ tests/; \
		echo "Lint complete"; \
	else \
		echo "cppcheck not installed, skipping lint"; \
	fi

# Format check (clang-format jika tersedia)
format-check:
	@if command -v clang-format > /dev/null 2>&1; then \
		find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror; \
		echo "Format check complete"; \
	else \
		echo "clang-format not installed, skipping format check"; \
	fi

# Static analysis via GCC warnings (always available)
analyze:
	$(CC) $(CFLAGS) -fanalyzer $(INCLUDES) -fsyntax-only $(C_SOURCES) 2>&1 || true
	@echo "Static analysis complete"

# Linting
lint:
	cppcheck --enable=all --suppress=missingIncludeSystem \
		-I$(SRC_DIR) -I$(SRC_DIR)/shared \
		-I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
		-I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
		-I$(SRC_DIR)/recap -I$(SRC_DIR)/cli \
		-I$(SRC_DIR)/tui -I$(SRC_DIR)/storage -I$(SRC_DIR)/sanitizer \
		src/ tests/
	@echo "Linting complete"

# Formatting
format:
	find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "Formatting complete"

check-format:
	find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror
	@echo "Format check complete"

# Static analysis
analyze:
	scan-build --status-bugs make
	@echo "Analysis complete"

# Memory check
valgrind: $(TEST_BIN)
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_BIN)
