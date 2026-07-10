/* INFRASTRUCTURE — Logging Implementation */
#include "infrastructure/infrastructure_logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel current_level = LOG_INFO;

void log_set_level(LogLevel level) {
    current_level = level;
}

LogLevel log_get_level(void) {
    return current_level;
}

void log_message(LogLevel level, const char *module, const char *function,
                 int line, const char *format, ...) {
    if (level < current_level) {
        return;
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    
    fprintf(stderr, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [%s:%s:%d] ",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            level_str[level], module, function, line);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}
