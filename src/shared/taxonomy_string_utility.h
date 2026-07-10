#ifndef TAXONOMY_STRING_UTILITY_H
#define TAXONOMY_STRING_UTILITY_H

#include <ctype.h>

/* Case-insensitive string comparison (C99-safe, no strcasecmp). */
static inline int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

#endif /* TAXONOMY_STRING_UTILITY_H */
