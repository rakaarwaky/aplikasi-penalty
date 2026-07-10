/* TESTS — Invarian & Konsistensi (gate yang menangkap regresi logika) */
#include "registration/module.registration.h"
#include "scoring/module.scoring.h"
#include "ranking/module.ranking.h"
#include "search/module.search.h"
#include "recap/module.recap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Bangun state SELESAI dengan skor per-peserta yang diberikan. */
static CompetitionState build_completed(int scores[], int n) {
    CompetitionState state = {0};
    state.state = STATE_REGISTERED;
    int i;
    for (i = 0; i < n; i++) {
        ParticipantNameVO name = {{0}};
        snprintf(name.value, 20, "P%d", i);
        capabilities_registration_append(&state, &name);
    }
    int p, k;
    for (p = 0; p < n; p++) {
        for (k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {scores[p]};
            capabilities_scoring_record_kick(&state, p, z);
        }
    }
    assert(state.state == STATE_COMPLETED);
    return state;
}

/* INVARIAN 1: setiap entry ranking harus konsisten dengan state.
   entry.total_score HARUS == state.participants[entry.participant_id].total_score
   dan entry.participant_id HARUS valid. Bug yang membalik urutan/id
   (tapi score kebetulan benar) akan ketahuan di sini. */
static void test_ranking_id_score_consistency(void) {
    int scores[] = {2, 5, 1, 4, 3};
    CompetitionState state = build_completed(scores, 5);
    RankingEntryVO entries[5];
    assert(capabilities_ranking_compute(&state, entries, 5) == RK_OK);

    int i;
    for (i = 0; i < 5; i++) {
        int pid = entries[i].participant_id;
        assert(pid >= 0 && pid < 5);
        int expected = state.participants[pid].total_score.value;
        assert(entries[i].total_score == expected);
    }
    /* Tidak boleh ada dua entry dengan participant_id sama. */
    for (i = 0; i < 5; i++) {
        int j;
        for (j = 0; j < 5; j++)
            if (i != j) assert(entries[i].participant_id != entries[j].participant_id);
    }
    printf("  [PASS] test_ranking_id_score_consistency\n");
}

/* INVARIAN 2: urut menurun & urutan rank valid (1,1,3,... untuk seri). */
static void test_ranking_order_and_ranks(void) {
    int scores[] = {3, 5, 3, 5, 1};
    CompetitionState state = build_completed(scores, 5);
    RankingEntryVO entries[5];
    capabilities_ranking_compute(&state, entries, 5);

    int i;
    for (i = 1; i < 5; i++)
        assert(entries[i - 1].total_score >= entries[i].total_score);
    /* rank monotonic non-decreasing */
    for (i = 1; i < 5; i++)
        assert(entries[i].rank >= entries[i - 1].rank);
    /* dua skor sama (P1&P3 = 35) harus rank sama; berikutnya loncat ke 3 */
    assert(entries[0].rank == 1);
    assert(entries[1].rank == 1);
    assert(entries[2].rank == 3);
    printf("  [PASS] test_ranking_order_and_ranks\n");
}

/* INVARIAN 3: total skor == Σ zona (property scoring). */
static void test_scoring_total_equals_sum_zones(void) {
    int patterns[][7] = {
        {0,0,0,0,0,0,0},
        {5,5,5,5,5,5,5},
        {1,2,3,4,5,0,1},
        {5,3,0,2,4,1,5},
        {2,2,2,2,2,2,2}
    };
    int p;
    for (p = 0; p < 5; p++) {
        CompetitionState state = {0};
        state.state = STATE_REGISTERED;
        ParticipantNameVO n = {{0}};
        strcpy(n.value, "X");
        capabilities_registration_append(&state, &n);
        int sum = 0, k;
        for (k = 0; k < TOTAL_KICKS; k++) {
            ZoneVO z = {patterns[p][k]};
            sum += patterns[p][k];
            capabilities_scoring_record_kick(&state, 0, z);
        }
        assert(state.participants[0].total_score.value == sum);
    }
    printf("  [PASS] test_scoring_total_equals_sum_zones\n");
}

/* INVARIAN 4: recap details total == ranking total per peserta. */
static void test_recap_matches_ranking(void) {
    int scores[] = {3, 5, 1, 4, 2};
    CompetitionState state = build_completed(scores, 5);
    RankingEntryVO rankings[5];
    capabilities_ranking_compute(&state, rankings, 5);
    SearchResultVO details[5];
    assert(capabilities_recap_prepare_details(&state, details, 5) == RC_OK);
    int i;
    for (i = 0; i < 5; i++) {
        int pid = rankings[i].participant_id;
        assert(details[pid].total_score == rankings[i].total_score);
        assert(details[pid].found == 1);
    }
    printf("  [PASS] test_recap_matches_ranking\n");
}

/* INVARIAN 5: batas jumlah peserta — tepat MIN, tepat MAX, dan over-MAX ditolak. */
static void test_participant_boundaries(void) {
    /* tepat MIN peserta -> state COMPLETED setelah kick */
    CompetitionState min = {0}; min.state = STATE_INIT;
    int i;
    for (i = 0; i < MIN_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "M%d", i);
        assert(capabilities_registration_append(&min, &n) == REG_OK);
    }
    for (i = 0; i < MIN_PARTICIPANTS; i++) {
        int k;
        for (k = 0; k < TOTAL_KICKS; k++)
            capabilities_scoring_record_kick(&min, i, (ZoneVO){1});
    }
    assert(min.state == STATE_COMPLETED);

    /* over-MAX -> REG_FULL */
    CompetitionState over = {0}; over.state = STATE_INIT;
    for (i = 0; i < MAX_PARTICIPANTS; i++) {
        ParticipantNameVO n = {{0}};
        snprintf(n.value, 10, "O%d", i);
        capabilities_registration_append(&over, &n);
    }
    ParticipantNameVO extra = {{0}};
    strcpy(extra.value, "Extra");
    assert(capabilities_registration_append(&over, &extra) == REG_FULL);
    assert(over.participant_count == MAX_PARTICIPANTS);
    printf("  [PASS] test_participant_boundaries\n");
}

/* INVARIAN 6: determinisme — ranking 2x menghasilkan urutan identik. */
static void test_ranking_deterministic(void) {
    int scores[] = {3, 5, 3, 5, 1, 4, 2};
    CompetitionState state = build_completed(scores, 7);
    RankingEntryVO a[7], b[7];
    capabilities_ranking_compute(&state, a, 7);
    capabilities_ranking_compute(&state, b, 7);
    int i;
    for (i = 0; i < 7; i++) {
        assert(a[i].participant_id == b[i].participant_id);
        assert(a[i].rank == b[i].rank);
        assert(a[i].total_score == b[i].total_score);
    }
    printf("  [PASS] test_ranking_deterministic\n");
}

void run_invariant_tests(void) {
    printf("=== Invariant Tests ===\n");
    test_ranking_id_score_consistency();
    test_ranking_order_and_ranks();
    test_scoring_total_equals_sum_zones();
    test_recap_matches_ranking();
    test_participant_boundaries();
    test_ranking_deterministic();
    printf("  All invariant tests passed!\n\n");
}
