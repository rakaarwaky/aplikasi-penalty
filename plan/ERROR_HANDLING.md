# ERROR_HANDLING.md — Plan to Score 10/10

## Current State (7/10)

**What we have:**
- Error codes for each module
- Basic validation
- NULL pointer checks
- State validation

**What we're missing:**
- Structured logging
- Error recovery mechanisms
- Error context/metadata
- User-friendly error messages
- Error tracking/history

## Target State (10/10)

**Enterprise Error Handling Requirements:**
- Structured logging system
- Error recovery mechanisms
- Error context and metadata
- User-friendly error messages
- Error tracking and history
- Audit trail

## Action Items

### 1. Add Logging System (High Priority)

**File: `src/shared/infrastructure_logger.h`**

```c
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

typedef struct {
    LogLevel level;
    const char *module;
    const char *function;
    int line;
    const char *message;
    timestamp_t timestamp;
} LogEntry;

void log_message(LogLevel level, const char *module, const char *function, 
                 int line, const char *format, ...);
void log_set_level(LogLevel level);
LogLevel log_get_level(void);
```

**Implementation:**
```c
// File: src/shared/infrastructure_logger.c
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel current_level = LOG_INFO;

void log_message(LogLevel level, const char *module, const char *function,
                 int line, const char *format, ...) {
    if (level < current_level) return;
    
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

#define LOG(level, fmt, ...) log_message(level, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG(LOG_FATAL, fmt, ##__VA_ARGS__)
```

### 2. Add Error Context (High Priority)

**File: `src/shared/taxonomy_error_context.h`**

```c
typedef struct {
    ErrorType type;
    const char *module;
    const char *function;
    int line;
    char message[256];
    timestamp_t timestamp;
} ErrorContext;

typedef struct {
    ErrorContext errors[100];
    int count;
} ErrorHistory;

void error_context_init(ErrorContext *ctx, ErrorType type, 
                        const char *module, const char *function,
                        int line, const char *message);
void error_history_add(ErrorHistory *history, const ErrorContext *ctx);
void error_history_print(const ErrorHistory *history);
```

### 3. Add Error Recovery (Medium Priority)

**File: `src/shared/infrastructure_recovery.h`**

```c
typedef enum {
    RECOVERY_NONE,
    RECOVERY_RETRY,
    RECOVERY_FALLBACK,
    RECOVERY_ABORT
} RecoveryAction;

typedef struct {
    RecoveryAction action;
    int max_retries;
    int retry_count;
    void (*fallback)(void *context);
} RecoveryStrategy;

RecoveryAction error_handle(ErrorType type, const RecoveryStrategy *strategy);
```

**Implementation:**
```c
// Example: Retry logic for file operations
RecoveryStrategy file_recovery = {
    .action = RECOVERY_RETRY,
    .max_retries = 3,
    .retry_count = 0,
    .fallback = NULL
};

RecoveryAction error_handle(ErrorType type, const RecoveryStrategy *strategy) {
    if (strategy->action == RECOVERY_RETRY) {
        if (strategy->retry_count < strategy->max_retries) {
            LOG_WARN("Retrying operation (attempt %d/%d)", 
                     strategy->retry_count + 1, strategy->max_retries);
            return RECOVERY_RETRY;
        }
    }
    return strategy->action;
}
```

### 4. Add User-Friendly Messages (Medium Priority)

**File: `src/shared/taxonomy_error_messages.h`**

```c
typedef struct {
    ErrorType code;
    const char *user_message;
    const char *technical_message;
    const char *suggestion;
} ErrorMessage;

static const ErrorMessage error_messages[] = {
    {ERR_INVALID_NAME, 
     "Nama peserta tidak valid",
     "Name contains invalid characters",
     "Gunakan huruf dan spasi saja (1-30 karakter)"},
    {ERR_DUPLICATE_NAME,
     "Nama peserta sudah ada",
     "Participant name already exists",
     "Gunakan nama yang berbeda"},
    {ERR_INVALID_ZONE,
     "Zona tendangan tidak valid",
     "Zone value out of range",
     "Masukkan zona 0-5"},
    {ERR_PARTICIPANT_NOT_FOUND,
     "Peserta tidak ditemukan",
     "Participant not found in system",
     "Periksa ejaan nama peserta"},
    // ... more messages
};

const char *error_get_user_message(ErrorType type);
const char *error_get_suggestion(ErrorType type);
```

### 5. Add Audit Trail (Low Priority)

**File: `src/shared/infrastructure_audit.h`**

```c
typedef struct {
    timestamp_t timestamp;
    const char *action;
    const char *user;
    const char *details;
    ErrorType result;
} AuditEntry;

typedef struct {
    AuditEntry entries[1000];
    int count;
} AuditTrail;

void audit_log(AuditTrail *trail, const char *action, 
               const char *user, const char *details,
               ErrorType result);
void audit_print(const AuditTrail *trail);
void audit_export_csv(const AuditTrail *trail, const char *filename);
```

### 6. Add Error Statistics (Low Priority)

**File: `src/shared/infrastructure_error_stats.h`**

```c
typedef struct {
    ErrorType type;
    int count;
    timestamp_t last_occurrence;
} ErrorStats;

typedef struct {
    ErrorStats stats[100];
    int count;
} ErrorStatistics;

void error_stats_record(ErrorStatistics *stats, ErrorType type);
void error_stats_print(const ErrorStatistics *stats);
ErrorType error_stats_most_common(const ErrorStatistics *stats);
```

## Implementation Steps

1. **Week 1:** Add logging system
2. **Week 2:** Add error context and messages
3. **Week 3:** Add error recovery
4. **Week 4:** Add audit trail and statistics

## Success Criteria

- [ ] All errors logged with context
- [ ] User-friendly messages for all errors
- [ ] Error recovery mechanisms working
- [ ] Audit trail complete
- [ ] Error statistics available
