# Makefile untuk Aplikasi Perhitungan Penalty (C99, AES v3.0)
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
                  $(SRC_DIR)/tui/infrastructure_tui_adapter.c \
                  $(SRC_DIR)/tui/root_display_container.c, $(C_SOURCES))
TEST_BIN   := run_tests

PREFIX ?= /usr/local
BINDIR  = $(PREFIX)/bin

.PHONY: all clean run test lint format analyze metrics version valgrind install uninstall win

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

# Windows cross-compile (MinGW)
WIN_CC      := x86_64-w64-mingw32-gcc
WIN_TARGET  := aplikasi_perhitungan_penalty.exe
WIN_LDFLAGS := -lpdcurses -lmingw32

win:
	$(WIN_CC) $(CFLAGS) $(INCLUDES) \
		$(SRC_DIR)/root_cli_main_entry.c \
		$(SRC_DIR)/cli/surfaces_menu_command.c \
		$(SRC_DIR)/cli/surfaces_registration_command.c \
		$(SRC_DIR)/cli/surfaces_scoring_command.c \
		$(SRC_DIR)/cli/surfaces_ranking_command.c \
		$(SRC_DIR)/cli/surfaces_search_command.c \
		$(SRC_DIR)/cli/surfaces_recap_command.c \
		$(SRC_DIR)/tui/infrastructure_tui_adapter.c \
		$(SRC_DIR)/registration/capabilities_registration_validator.c \
		$(SRC_DIR)/registration/capabilities_registration_appender.c \
		$(SRC_DIR)/registration/agent_registration_orchestrator.c \
		$(SRC_DIR)/registration/root_registration_container.c \
		$(SRC_DIR)/scoring/capabilities_scoring_zone_validator.c \
		$(SRC_DIR)/scoring/capabilities_scoring_score_calculator.c \
		$(SRC_DIR)/scoring/agent_scoring_orchestrator.c \
		$(SRC_DIR)/scoring/root_scoring_container.c \
		$(SRC_DIR)/ranking/capabilities_ranking_calculator.c \
		$(SRC_DIR)/ranking/agent_ranking_orchestrator.c \
		$(SRC_DIR)/ranking/root_ranking_container.c \
		$(SRC_DIR)/search/capabilities_search_resolver.c \
		$(SRC_DIR)/search/agent_search_orchestrator.c \
		$(SRC_DIR)/search/root_search_container.c \
		$(SRC_DIR)/recap/capabilities_recap_formatter.c \
		$(SRC_DIR)/recap/agent_recap_orchestrator.c \
		$(SRC_DIR)/recap/root_recap_container.c \
		$(SRC_DIR)/storage/infrastructure_storage_adapter.c \
		$(SRC_DIR)/storage/agent_storage_orchestrator.c \
		$(SRC_DIR)/storage/root_storage_container.c \
		$(SRC_DIR)/sanitizer/capabilities_sanitizer_validator.c \
		$(SRC_DIR)/export/infrastructure_export_adapter.c \
		$(SRC_DIR)/export/agent_export_orchestrator.c \
		$(SRC_DIR)/export/root_export_container.c \
		$(SRC_DIR)/shared/taxonomy_logger.c \
		-o $(WIN_TARGET) $(WIN_LDFLAGS)
	@echo "Windows exe: $(WIN_TARGET)"
