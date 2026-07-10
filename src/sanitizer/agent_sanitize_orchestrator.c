/**
 * @file agent_sanitize_orchestrator.c
 * @brief Orkestrasi validasi input: pastikan input valid sebelum diproses.
 */

/* SANITIZER — Agent Orkestrator */
#include "module.sanitizer.h"
#include "shared/taxonomy_logger.h"

/**
 * Validasi string input melalui protocol.
 *
 * @param agg   Penghubung ke fungsi sanitasi.
 * @param input String yang akan divalidasi.
 * @param max_length Panjang maksimum yang diizinkan.
 * @return      SANITIZE_OK bila sah, atau error bila tidak.
 */
SanitizeError agent_sanitize_validate_string(const SanitizeAggregate *agg,
                                             const char *input,
                                             size_t max_length) {
    LOG_INFO("sanitize_string: memulai validasi");
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("sanitize_string: aggregate/protocol NULL");
        return SANITIZE_ERROR_NULL_INPUT;
    }
    if (agg->protocol->validate_string == NULL) {
        LOG_ERROR("sanitize_string: fungsi validate_string NULL");
        return SANITIZE_ERROR_NULL_INPUT;
    }
    SanitizeError result = agg->protocol->validate_string(input, max_length);
    if (result == SANITIZE_OK)
        LOG_INFO("sanitize_string: valid");
    else
        LOG_ERROR("sanitize_string: tidak valid");
    return result;
}

/**
 * Validasi angka input melalui protocol.
 *
 * @param agg   Penghubung ke fungsi sanitasi.
 * @param input String angka yang akan divalidasi.
 * @param min_val Nilai minimum yang diizinkan.
 * @param max_val Nilai maksimum yang diizinkan.
 * @return      SANITIZE_OK bila sah, atau error bila tidak.
 */
SanitizeError agent_sanitize_validate_int(const SanitizeAggregate *agg,
                                          const char *input,
                                          int min_val, int max_val) {
    LOG_INFO("sanitize_int: memulai validasi");
    if (agg == NULL || agg->protocol == NULL) {
        LOG_ERROR("sanitize_int: aggregate/protocol NULL");
        return SANITIZE_ERROR_NULL_INPUT;
    }
    if (agg->protocol->validate_int == NULL) {
        LOG_ERROR("sanitize_int: fungsi validate_int NULL");
        return SANITIZE_ERROR_NULL_INPUT;
    }
    SanitizeError result = agg->protocol->validate_int(input, min_val, max_val);
    if (result == SANITIZE_OK)
        LOG_INFO("sanitize_int: valid");
    else
        LOG_ERROR("sanitize_int: tidak valid");
    return result;
}
