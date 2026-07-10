/* Utilitas logger — pengaturan level dan pemancaran pesan log. */
#ifndef SHARED_TAXONOMY_LOGGER_H
#define SHARED_TAXONOMY_LOGGER_H

#include "shared/taxonomy_log_level.h"

void log_set_level(LogLevel level);
LogLevel log_get_level(void);
void log_message(LogLevel level, const char *module, const char *message);

/* Makro pendukung untuk pemanggilan log yang lebih ringkas. */
#define LOG_DEBUG(msg) log_message(LOG_DEBUG, __func__, msg)
#define LOG_INFO(msg)  log_message(LOG_INFO, __func__, msg)
#define LOG_WARN(msg)  log_message(LOG_WARN, __func__, msg)
#define LOG_ERROR(msg) log_message(LOG_ERROR, __func__, msg)
#define LOG_FATAL(msg) log_message(LOG_FATAL, __func__, msg)

#endif /* SHARED_TAXONOMY_LOGGER_H */
