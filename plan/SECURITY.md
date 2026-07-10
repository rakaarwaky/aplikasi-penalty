# SECURITY.md — Plan to Score 10/10

## Current State (5/10)

**What we have:**
- Basic input validation
- NULL pointer checks
- State validation

**What we're missing:**
- Input sanitization
- Buffer overflow protection
- Format string protection
- Integer overflow protection
- Security auditing

## Target State (10/10)

**Enterprise Security Requirements:**
- Input sanitization
- Buffer overflow protection
- Format string protection
- Integer overflow protection
- Security auditing
- Penetration testing

## Action Items

### 1. Add Input Sanitization (High Priority)

**File: `src/shared/infrastructure_sanitizer.h`**

```c
#include <ctype.h>
#include <string.h>

typedef enum {
    SANITIZE_OK,
    SANITIZE_ERROR_NULL_INPUT,
    SANITIZE_ERROR_TOO_LONG,
    SANITIZE_ERROR_INVALID_CHARS,
    SANITIZE_ERROR_BUFFER_OVERFLOW
} SanitizeResult;

// Sanitize string input
SanitizeResult sanitize_string(char *output, size_t output_size,
                               const char *input, size_t max_length) {
    if (input == NULL || output == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    size_t input_len = strlen(input);
    if (input_len >= output_size) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;
    }
    
    if (input_len > max_length) {
        return SANITIZE_ERROR_TOO_LONG;
    }
    
    // Copy and sanitize
    for (size_t i = 0; i <= input_len; i++) {
        if (!isprint((unsigned char)input[i]) && input[i] != '\0') {
            return SANITIZE_ERROR_INVALID_CHARS;
        }
        output[i] = input[i];
    }
    
    return SANITIZE_OK;
}

// Sanitize integer input
SanitizeResult sanitize_int(int *output, const char *input,
                            int min_val, int max_val) {
    if (input == NULL || output == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    // Check for format string attacks
    if (strchr(input, '%') != NULL) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    
    // Parse integer
    char *endptr;
    long val = strtol(input, &endptr, 10);
    
    if (*endptr != '\0' || endptr == input) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    
    if (val < min_val || val > max_val) {
        return SANITIZE_ERROR_TOO_LONG;
    }
    
    *output = (int)val;
    return SANITIZE_OK;
}
```

### 2. Add Buffer Overflow Protection (High Priority)

**File: `src/shared/infrastructure_buffer.h`**

```c
#include <string.h>

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} SafeBuffer;

// Initialize safe buffer
void buffer_init(SafeBuffer *buf, char *data, size_t capacity) {
    buf->data = data;
    buf->size = 0;
    buf->capacity = capacity;
    buf->data[0] = '\0';
}

// Safe string copy
SanitizeResult buffer_copy(SafeBuffer *buf, const char *src) {
    if (buf == NULL || src == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    size_t src_len = strlen(src);
    if (src_len >= buf->capacity) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(buf->data, src, src_len + 1);
    buf->size = src_len;
    return SANITIZE_OK;
}

// Safe string concatenation
SanitizeResult buffer_append(SafeBuffer *buf, const char *src) {
    if (buf == NULL || src == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    size_t src_len = strlen(src);
    if (buf->size + src_len >= buf->capacity) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;
    }
    
    memcpy(buf->data + buf->size, src, src_len + 1);
    buf->size += src_len;
    return SANITIZE_OK;
}

// Safe formatted output
SanitizeResult buffer_printf(SafeBuffer *buf, const char *format, ...) {
    if (buf == NULL || format == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    va_list args;
    va_start(args, format);
    int written = vsnprintf(buf->data + buf->size, 
                           buf->capacity - buf->size,
                           format, args);
    va_end(args);
    
    if (written < 0) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    
    if ((size_t)written >= buf->capacity - buf->size) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;
    }
    
    buf->size += written;
    return SANITIZE_OK;
}
```

### 3. Add Format String Protection (High Priority)

**File: `src/shared/infrastructure_format_protect.h`**

```c
#include <string.h>

// Check for format string attacks
SanitizeResult validate_format_string(const char *input) {
    if (input == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    // Check for format specifiers
    const char *dangerous[] = {"%s", "%n", "%x", "%p", "%d", "%f", "%c", "%u", NULL};
    
    for (int i = 0; dangerous[i] != NULL; i++) {
        if (strstr(input, dangerous[i]) != NULL) {
            return SANITIZE_ERROR_INVALID_CHARS;
        }
    }
    
    return SANITIZE_OK;
}

// Safe printf wrapper
void safe_printf(const char *format, ...) {
    // Validate format string
    if (validate_format_string(format) != SANITIZE_OK) {
        fprintf(stderr, "Error: Invalid format string\n");
        return;
    }
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
```

### 4. Add Integer Overflow Protection (Medium Priority)

**File: `src/shared/infrastructure_int_safe.h`**

```c
#include <limits.h>
#include <errno.h>

// Safe integer addition
SanitizeResult safe_add(int *result, int a, int b) {
    if (result == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    if (a > 0 && b > INT_MAX - a) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;  // Overflow
    }
    if (a < 0 && b < INT_MIN - a) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;  // Underflow
    }
    
    *result = a + b;
    return SANITIZE_OK;
}

// Safe integer multiplication
SanitizeResult safe_multiply(int *result, int a, int b) {
    if (result == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    if (a != 0 && b > INT_MAX / a) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;  // Overflow
    }
    if (a != 0 && b < INT_MIN / a) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;  // Underflow
    }
    
    *result = a * b;
    return SANITIZE_OK;
}

// Safe string to integer conversion
SanitizeResult safe_strtoi(int *result, const char *str) {
    if (result == NULL || str == NULL) {
        return SANITIZE_ERROR_NULL_INPUT;
    }
    
    // Check for format string attacks
    if (validate_format_string(str) != SANITIZE_OK) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        return SANITIZE_ERROR_BUFFER_OVERFLOW;
    }
    
    if (*endptr != '\0' || endptr == str) {
        return SANITIZE_ERROR_INVALID_CHARS;
    }
    
    *result = (int)val;
    return SANITIZE_OK;
}
```

### 5. Add Security Auditing (Medium Priority)

**File: `src/shared/infrastructure_security_audit.h`**

```c
typedef enum {
    SECURITY_OK,
    SECURITY_WARNING,
    SECURITY_CRITICAL
} SecurityLevel;

typedef struct {
    SecurityLevel level;
    const char *category;
    const char *message;
    const char *location;
    timestamp_t timestamp;
} SecurityEvent;

typedef struct {
    SecurityEvent events[1000];
    int count;
} SecurityAudit;

// Log security event
void security_audit_log(SecurityAudit *audit, SecurityLevel level,
                       const char *category, const char *message,
                       const char *location) {
    if (audit == NULL || audit->count >= 1000) {
        return;
    }
    
    SecurityEvent *event = &audit->events[audit->count];
    event->level = level;
    event->category = category;
    event->message = message;
    event->location = location;
    event->timestamp = time(NULL);
    
    audit->count++;
    
    // Log to stderr for critical events
    if (level == SECURITY_CRITICAL) {
        fprintf(stderr, "[SECURITY CRITICAL] %s: %s at %s\n",
                category, message, location);
    }
}

// Check for common vulnerabilities
void security_check_input(const char *input, const char *location,
                          SecurityAudit *audit) {
    if (input == NULL) {
        security_audit_log(audit, SECURITY_WARNING, "NULL_INPUT",
                         "NULL pointer passed as input", location);
        return;
    }
    
    // Check for format string
    if (validate_format_string(input) != SANITIZE_OK) {
        security_audit_log(audit, SECURITY_CRITICAL, "FORMAT_STRING",
                         "Format string vulnerability detected", location);
    }
    
    // Check for buffer overflow attempts
    if (strlen(input) > 1000) {
        security_audit_log(audit, SECURITY_WARNING, "OVERFLOW_ATTEMPT",
                         "Suspiciously long input", location);
    }
}
```

### 6. Add Penetration Testing (Low Priority)

**File: `tests/test_security.c`**

```c
void test_format_string_attack(void) {
    CompetitionState state = {0};
    ParticipantNameVO name = {{0}};
    
    // Try format string attack
    char malicious_name[] = "%s%s%s%s%s%s%s%s%s%s";
    memcpy(name.value, malicious_name, sizeof(malicious_name));
    
    SanitizeResult result = sanitize_string(name.value, sizeof(name.value),
                                           malicious_name, 30);
    assert(result == SANITIZE_ERROR_INVALID_CHARS);
    printf("  [PASS] test_format_string_attack\n");
}

void test_buffer_overflow_attempt(void) {
    CompetitionState state = {0};
    ParticipantNameVO name = {{0}};
    
    // Try buffer overflow
    char long_name[100];
    memset(long_name, 'A', 99);
    long_name[99] = '\0';
    
    SanitizeResult result = sanitize_string(name.value, sizeof(name.value),
                                           long_name, 30);
    assert(result == SANITIZE_ERROR_TOO_LONG);
    printf("  [PASS] test_buffer_overflow_attempt\n");
}

void test_integer_overflow(void) {
    int result;
    
    // Try integer overflow
    SanitizeResult res = safe_add(&result, INT_MAX, 1);
    assert(res == SANITIZE_ERROR_BUFFER_OVERFLOW);
    printf("  [PASS] test_integer_overflow\n");
}
```

## Security Checklist

- [ ] All input sanitized
- [ ] Buffer overflow protection
- [ ] Format string protection
- [ ] Integer overflow protection
- [ ] Security auditing enabled
- [ ] Penetration tests passing

## Implementation Steps

1. **Week 1:** Add input sanitization and buffer protection
2. **Week 2:** Add format string and integer overflow protection
3. **Week 3:** Add security auditing
4. **Week 4:** Add penetration tests

## Success Criteria

- [ ] All security tests passing
- [ ] No vulnerabilities detected
- [ ] Security audit log complete
- [ ] Penetration tests passing
