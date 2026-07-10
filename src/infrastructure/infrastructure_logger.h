/* INFRASTRUCTURE — Logging System */
#ifndef INFRASTRUCTURE_LOGGER_H
#define INFRASTRUCTURE_LOGGER_H

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
} LogLevel;

void log_message(LogLevel level, const char *module, const char *function,
                 int line, const char *format, ...);
void log_set_level(LogLevel level);
LogLevel log_get_level(void);

#define LOG_DEBUG(fmt, ...) log_message(LOG_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_message(LOG_INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) log_message(LOG_WARN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_message(LOG_FATAL, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* INFRASTRUCTURE_LOGGER_H */
