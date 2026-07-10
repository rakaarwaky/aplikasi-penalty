/* SANITIZER — Contract (Protocol) */
#ifndef CONTRACT_SANITIZE_PROTOCOL_H
#define CONTRACT_SANITIZE_PROTOCOL_H

#include "shared/taxonomy_sanitize_error.h"
#include <stddef.h>

typedef struct {
    SanitizeError (*validate_string)(const char *input, size_t max_length);
    SanitizeError (*validate_int)(const char *input, int min_val, int max_val);
} SanitizeProtocol;

#endif /* CONTRACT_SANITIZE_PROTOCOL_H */
