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
            -I$(SRC_DIR)/tui -I$(SRC_DIR)/storage \
            -I$(SRC_DIR)/sanitizer -I$(SRC_DIR)/export

LDFLAGS  := -lncurses

# Test
TEST_DIR   := tests
TEST_SRCS  := $(wildcard $(TEST_DIR)/*.c)
TEST_INC   := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
              -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
              -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
              -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli -I$(SRC_DIR)/tui \
              -I$(SRC_DIR)/storage -I$(SRC_DIR)/sanitizer \
              -I$(SRC_DIR)/export
TEST_LIB_SRC := $(filter-out $(SRC_DIR)/root_cli_main_entry.c \
                  $(SRC_DIR)/cli/surfaces_menu_command.c \
                  $(SRC_DIR)/cli/surfaces_registration_command.c \
                  $(SRC_DIR)/cli/surfaces_scoring_command.c \
                  $(SRC_DIR)/cli/surfaces_ranking_command.c \
                  $(SRC_DIR)/cli/surfaces_search_command.c \
                  $(SRC_DIR)/cli/surfaces_recap_command.c \
                  $(SRC_DIR)/tui/infrastructure_tui_adapter.c, $(C_SOURCES))
TEST_BIN   := run_tests

PREFIX ?= /usr/local
BINDIR  = $(PREFIX)/bin

.PHONY: all clean run test lint format analyze metrics version valgrind install uninstall

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

# Lint (cppcheck)
lint:
	@if command -v cppcheck > /dev/null 2>&1; then \
		cppcheck --enable=all --suppress=missingIncludeSystem \
			--suppress=unusedFunction --suppress=normalCheckLevelMaxBranches \
			$(INCLUDES) src/ tests/; \
		echo "Linting complete"; \
	else \
		echo "cppcheck not installed, skipping"; \
	fi

# Format check (clang-format)
format:
	@if command -v clang-format > /dev/null 2>&1; then \
		find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "Formatting complete"; \
	else \
		echo "clang-format not installed, skipping"; \
	fi

# Static analysis (scan-build)
analyze:
	@if command -v scan-build > /dev/null 2>&1; then \
		scan-build --status-bugs make clean && scan-build --status-bugs make; \
		echo "Analysis complete"; \
	else \
		echo "scan-build not installed, skipping"; \
	fi

# Memory check (valgrind)
valgrind: $(TEST_BIN)
	@if command -v valgrind > /dev/null 2>&1; then \
		valgrind --leak-check=full --error-exitcode=1 ./$(TEST_BIN); \
	else \
		echo "valgrind not installed, skipping"; \
	fi

# Code metrics
metrics:
	@echo "=== Code Metrics ==="
	@echo "Source files: $$(find src/ -name '*.c' | wc -l)"
	@echo "Header files: $$(find src/ -name '*.h' | wc -l)"
	@echo "Source lines: $$(find src/ -name '*.c' -exec cat {} + | wc -l)"
	@echo "Header lines: $$(find src/ -name '*.h' -exec cat {} + | wc -l)"
	@echo "Test files:   $$(find tests/ -name '*.c' | wc -l)"
	@echo "Test lines:   $$(find tests/ -name '*.c' -exec cat {} + | wc -l)"

# Version
version:
	@echo "Version: 1.0.0"
	@echo "Build: $$(date +%Y%m%d)"

# Install
install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

# Uninstall
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)
