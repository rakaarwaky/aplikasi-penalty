#include "cli/module.cli.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* utility inline (milik surface ini): banding string case-insensitive. */
static int ci_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

static void display_error(RegistrationError e) {
    switch (e) {
        case REG_NAME_EMPTY:        printf("  [GAGAL] Nama tidak boleh kosong.\n"); break;
        case REG_NAME_TOO_LONG:     printf("  [GAGAL] Nama melebihi %d karakter.\n", MAX_NAME_LENGTH); break;
        case REG_NAME_INVALID_CHAR: printf("  [GAGAL] Nama hanya huruf dan spasi.\n"); break;
        case REG_NAME_DUPLICATE:    printf("  [GAGAL] Nama sudah terdaftar (case-insensitive).\n"); break;
        case REG_FULL:              printf("  [GAGAL] Kuota penuh.\n"); break;
        case REG_TOO_FEW:           printf("  [GAGAL] Minimal %d peserta.\n", MIN_PARTICIPANTS); break;
        default:                    printf("  [GAGAL] Kesalahan pendaftaran.\n");
    }
}

void cli_surfaces_registration_execute(RegistrationAggregate *agg, CompetitionState *state) {
    if (agg == NULL || state == NULL) return;

    printf("\n=== PENDAFTARAN PESERTA ===\n");
    printf("Kuota: %d - %d peserta. Kosongkan nama / ketik SELESAI untuk berhenti.\n",
           MIN_PARTICIPANTS, MAX_PARTICIPANTS);
    printf("Peserta terdaftar: %d\n\n", state->participant_count);

    char buffer[MAX_NAME_LENGTH + 2];
    while (state->participant_count < MAX_PARTICIPANTS) {
        printf("Nama peserta #%d: ", state->participant_count + 1);
        fflush(stdout);

        if (fgets(buffer, sizeof buffer, stdin) == NULL) break;
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';

        if (len == 0 || ci_equal(buffer, "selesai")) {
            if (state->participant_count >= MIN_PARTICIPANTS) break;
            display_error(REG_TOO_FEW);
            continue;
        }

        ParticipantNameVO name;
        strncpy(name.value, buffer, MAX_NAME_LENGTH);
        name.value[MAX_NAME_LENGTH] = '\0';

        RegistrationError e = agent_registration_add(agg, state, &name);
        if (e == REG_OK) printf("  [OK] %s terdaftar.\n", name.value);
        else display_error(e);
    }

    printf("\nTotal peserta: %d\n", state->participant_count);
    printf("Tekan Enter untuk melanjutkan...");
    getchar();
}
