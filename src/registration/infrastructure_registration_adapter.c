#include "registration/module.registration.h"

#include <stdio.h>
#include <string.h>

static void infra_display_header(void) {
    printf("\n=== PENDAFTARAN PESERTA ===\n");
    printf("Kuota: %d - %d peserta. Kosongkan nama lalu Enter untuk selesai.\n",
           MIN_PARTICIPANTS, MAX_PARTICIPANTS);
}

static void infra_display_prompt(int number) {
    printf("Nama peserta #%d: ", number);
    fflush(stdout);
}

static RegistrationError infra_read_name(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) return REG_NAME_EMPTY;
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        buffer[--len] = '\0';
    if (len == 0) return REG_NAME_EMPTY;
    return REG_OK;
}

static void infra_display_success(const char *name) {
    printf("  [OK] %s terdaftar.\n", name);
}

static void infra_display_error(RegistrationError e) {
    switch (e) {
        case REG_NAME_EMPTY:        printf("  [GAGAL] Nama tidak boleh kosong.\n"); break;
        case REG_NAME_TOO_LONG:     printf("  [GAGAL] Nama melebihi 30 karakter.\n"); break;
        case REG_NAME_INVALID_CHAR: printf("  [GAGAL] Nama hanya huruf dan spasi.\n"); break;
        case REG_NAME_DUPLICATE:    printf("  [GAGAL] Nama sudah terdaftar (case-insensitive).\n"); break;
        case REG_FULL:              printf("  [GAGAL] Kuota penuh.\n"); break;
        case REG_TOO_FEW:           printf("  [GAGAL] Minimal %d peserta.\n", MIN_PARTICIPANTS); break;
        default:                    printf("  [GAGAL] Kesalahan pendaftaran.\n");
    }
}

static void infra_display_wait(void) {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar();
}

static void infra_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

RegistrationPort *create_registration_port(void) {
    static RegistrationPort port;
    port.display_header  = infra_display_header;
    port.display_prompt  = infra_display_prompt;
    port.read_name       = infra_read_name;
    port.display_success = infra_display_success;
    port.display_error   = infra_display_error;
    port.display_wait    = infra_display_wait;
    port.clear_buffer    = infra_clear_buffer;
    return &port;
}
