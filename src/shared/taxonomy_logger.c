/* Implementasi logger — pengaturan level dan pencatatan pesan log. */
#include "shared/taxonomy_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*
 * Default level = LOG_FATAL.
 *
 * Aplikasi ini berjalan di mode TUI (ncurses). Logger menulis ke stderr,
 * dan tulisan bebas ke terminal akan merusak layar ncurses — makanya pesan
 * DEBUG/INFO/WARN/ERROR disembunyikan secara default. Hanya FATAL yang tampil.
 *
 * Tujuannya: saat `make run` jalan normal, tidak ada sampah log yang merusak
 * layar TUI. Pesan ERROR (mis. input user tak valid) sudah ditampilkan sendiri
 * di layar TUI, jadi logger tidak perlu meng-echo-nya ke stderr.
 *
 * Untuk debug, set env PENALTI_LOG=DEBUG (atau INFO/WARN/ERROR/FATAL).
 */
static LogLevel current_level = LOG_FATAL;

static const char *level_names[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

/* Inisialisasi level dari env PENALTI_LOG (dipanggil sekali). */
static LogLevel resolve_initial_level(void) {
    const char *env = getenv("PENALTI_LOG");
    if (env == NULL) return LOG_FATAL;
    if (strcmp(env, "DEBUG") == 0) return LOG_DEBUG;
    if (strcmp(env, "INFO")  == 0) return LOG_INFO;
    if (strcmp(env, "WARN")  == 0) return LOG_WARN;
    if (strcmp(env, "ERROR") == 0) return LOG_ERROR;
    if (strcmp(env, "FATAL") == 0) return LOG_FATAL;
    return LOG_FATAL; /* nilai env tak dikenal → fallback ke FATAL (senyap) */
}

void log_set_level(LogLevel level) {
    current_level = level;
}

LogLevel log_get_level(void) {
    return current_level;
}

void log_message(LogLevel level, const char *module, const char *message) {
    /* Lazy-init sekali saat pemanggilan pertama (setelah env tersedia). */
    static int initialized = 0;
    if (!initialized) {
        current_level = resolve_initial_level();
        initialized = 1;
    }

    if (level < current_level) return;
    fprintf(stderr, "[%s] %s: %s\n", level_names[level], module, message);
}
