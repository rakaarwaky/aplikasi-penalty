/* Implementasi logger — pengaturan level dan pencatatan pesan log. */
#include "shared/taxonomy_logger.h"
#include <stdio.h>
#include <stdarg.h>

static LogLevel current_level = LOG_INFO;

static const char *level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

void log_set_level(LogLevel level) {
    current_level = level;
}

LogLevel log_get_level(void) {
    return current_level;
}

void log_message(LogLevel level, const char *module, const char *message) {
    if (level < current_level) return;
    fprintf(stderr, "[%s] %s: %s\n", level_names[level], module, message);
}
