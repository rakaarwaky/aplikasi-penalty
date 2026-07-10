#ifndef SHARED_MODULE_SHARED_H
#define SHARED_MODULE_SHARED_H

/* AES_BYPASS: module declaration (barrel) — 1 header untuk seluruh folder shared.
   Analog mod.rs/index.ts: satu titik masuk publik untuk semua taxonomy shared. */

#include <stddef.h> /* NULL, size_t untuk seluruh modul */

/* ============================================================
   GAME CONSTANT
   AES401: HANYA #define, tanpa fungsi/extern/logic.
   ============================================================ */
#define MIN_PARTICIPANTS 5
#define MAX_PARTICIPANTS 7
#define TOTAL_KICKS 7
#define MIN_ZONE 0
#define MAX_ZONE 5
#define MAX_NAME_LENGTH 30

#define MENU_EXIT 0
#define MENU_REGISTRATION 1
#define MENU_SCORING 2
#define MENU_RANKING 3
#define MENU_SEARCH 4
#define MENU_RECAP 5

/* ============================================================
   PARTICIPANT NAME VO (AES402: VO nama peserta)
   ============================================================ */
typedef struct {
    char value[MAX_NAME_LENGTH + 1];
} ParticipantNameVO;

/* ============================================================
   ZONE VO (batas domain scoring, AES402)
   ============================================================ */
typedef struct {
    int value;
} ZoneVO;

/* ============================================================
   KICK VO (satu tendangan: zona == poin, PRD 2.2)
   ============================================================ */
typedef struct {
    int zone;
    int points;
} KickVO;

/* ============================================================
   PARTICIPANT ENTITY (AES401: pakai VO, bukan primitif nama)
   ============================================================ */
typedef struct {
    int id;
    ParticipantNameVO name;
    int kicks[TOTAL_KICKS];
    int total_score;
    int zone_freq[MAX_ZONE + 1];
    int kick_count;
} ParticipantEntity;

/* ============================================================
   COMPETITION STATE (di-pass via pointer ke semua layer, tanpa global)
   ============================================================ */
typedef enum {
    STATE_INIT = 0,
    STATE_REGISTERED = 1,
    STATE_COMPLETED = 2
} CompetitionStateKind;

typedef struct {
    ParticipantEntity participants[MAX_PARTICIPANTS];
    int participant_count;
    CompetitionStateKind state;
} CompetitionState;

/* ============================================================
   RANK VO (dipakai ranking + recap)
   ============================================================ */
typedef struct {
    int participant_id;
    int total_score;
    int zone_freq[MAX_ZONE + 1];
    int rank;
} RankingEntryVO;

/* ============================================================
   SEARCH RESULT VO
   ============================================================ */
typedef struct {
    int found;
    int participant_id;
    char name[MAX_NAME_LENGTH + 1];
    int total_score;
    int kicks[TOTAL_KICKS];
    int zone_freq[MAX_ZONE + 1];
} SearchResultVO;

/* ============================================================
   ERROR ENUM PER FITUR (bukan satu enum raksasa)
   ============================================================ */
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

#endif /* SHARED_MODULE_SHARED_H */
