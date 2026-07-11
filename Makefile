# Makefile untuk Aplikasi Perhitungan Penalti (C99, AES v3.0)
CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Wpedantic -O2 -g
TARGET  := aplikasi_perhitungan_penalti

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

LDFLAGS  := -lncursesw

# Test
TEST_DIR   := tests
TEST_SRCS  := $(filter-out $(TEST_DIR)/screenshooter.c, $(wildcard $(TEST_DIR)/*.c))
TEST_INC   := -I$(SRC_DIR) -I$(SRC_DIR)/shared \
              -I$(SRC_DIR)/registration -I$(SRC_DIR)/scoring \
              -I$(SRC_DIR)/ranking -I$(SRC_DIR)/search \
              -I$(SRC_DIR)/recap -I$(SRC_DIR)/cli -I$(SRC_DIR)/tui \
              -I$(SRC_DIR)/storage -I$(SRC_DIR)/sanitizer \
              -I$(SRC_DIR)/export
TEST_LIB_SRC := $(filter-out $(SRC_DIR)/root_cli_main_entry.c, $(C_SOURCES))
# fake ncurses (headless) untuk coverage CLI/TUI tanpa terminal
TEST_FAKE_SRC := tests/fake_ncurses.c
TEST_BIN   := run_tests

# Test build TIDAK link -lncurses (pakai stub di TEST_FAKE_SRC).
TEST_LDFLAGS :=

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

# Coverage: bangun test dengan instrumentasi --coverage, jalankan, lalu
# pindahkan semua artefak (.gcno/.gcda) ke folder coverage/ agar tidak
# berserakan di root. jalankan gcov dari dalam coverage/ untuk ringkasan.
COV_DIR := coverage
coverage:
	@mkdir -p $(COV_DIR)
	@rm -f run_tests-*.gcno run_tests-*.gcda $(COV_DIR)/run_tests-*.gcno $(COV_DIR)/run_tests-*.gcda
	$(MAKE) $(TEST_BIN) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage"
	./$(TEST_BIN)
	@mv -f run_tests-*.gcno run_tests-*.gcda $(COV_DIR)/ 2>/dev/null || true
	@cd $(COV_DIR) && shopt -s nullglob; for f in run_tests-*.gcno; do gcov -n "$$f" >/dev/null 2>&1; done; shopt -u nullglob
	@echo "Coverage artefacts in ./$(COV_DIR)/ — jalankan: cd $(COV_DIR) && gcov run_tests-*.gcno"

$(TEST_BIN): $(TEST_LIB_SRC) $(TEST_SRCS)
	$(CC) $(CFLAGS) $(TEST_INC) -o $@ $^ $(TEST_LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_BIN) $(COV_DIR)

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

# Windows cross-compile (MinGW + PDCurses).
# PDCurses (curses.h + wincon/pdcurses.a) must be available. The default
# WIN_PDCURSES path is where it was built on this machine; override if yours
# lives elsewhere, e.g.:
#   make win WIN_PDCURSES=/path/to/PDCurses-3.9
WIN_CC       := x86_64-w64-mingw32-gcc
WIN_TARGET   := aplikasi_perhitungan_penalti.exe
WIN_PDCURSES ?= /tmp/opencode/PDCurses-3.9
WIN_SOURCES  := $(shell find $(SRC_DIR) -name '*.c')
WIN_LDFLAGS  := $(WIN_PDCURSES)/wincon/pdcurses.a -lmingw32

win:
	$(WIN_CC) $(CFLAGS) $(INCLUDES) -I$(WIN_PDCURSES) \
		$(WIN_SOURCES) \
		-o $(WIN_TARGET) $(WIN_LDFLAGS)
	@echo "Windows exe: $(WIN_TARGET)"
