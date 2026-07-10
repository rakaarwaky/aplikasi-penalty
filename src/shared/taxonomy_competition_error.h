#ifndef SHARED_TAXONOMY_COMPETITION_ERROR_H
#define SHARED_TAXONOMY_COMPETITION_ERROR_H

/* Error enum PER FITUR (bukan satu enum raksasa). */
typedef enum {
    REG_OK = 0, REG_NAME_EMPTY, REG_NAME_TOO_LONG,
    REG_NAME_INVALID_CHAR, REG_NAME_DUPLICATE, REG_FULL, REG_TOO_FEW
} RegistrationError;

typedef enum {
    SC_OK = 0, SC_INVALID_ZONE, SC_NOT_REGISTERED,
    SC_ALREADY_DONE, SC_PARTICIPANT_NOT_FOUND
} ScoringError;

typedef enum {
    RK_OK = 0, RK_NOT_READY, RK_NO_PARTICIPANT
} RankingError;

typedef enum {
    SR_OK = 0, SR_NOT_FOUND, SR_EMPTY_QUERY
} SearchError;

typedef enum {
    RC_OK = 0, RC_NOT_READY
} RecapError;

typedef enum {
    ST_OK = 0, ST_ERROR_FILE_NOT_FOUND, ST_ERROR_PERMISSION,
    ST_ERROR_CORRUPT, ST_ERROR_FULL
} StorageError;

#endif /* SHARED_TAXONOMY_COMPETITION_ERROR_H */
