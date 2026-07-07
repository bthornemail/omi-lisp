/*
 * OMI Layer: test
 * Purpose: Verify the OMI-Lisp -> typed construction candidate handoff.
 * Authority: Non-authoritative declaration surface.
 *
 * Tests mapping of seed, symbol, and pair candidates into the neutral
 * downstream shape. No parser, no eval, no validation, no receipt.
 */

#include <stdio.h>
#include "omi_lisp.h"
#include "omi_candidate.h"

static int failures = 0;

#define CHECK(cond, msg)                                            \
    do {                                                            \
        if (cond) {                                                 \
            printf("PASS: %s\n", msg);                              \
        } else {                                                    \
            printf("FAIL: %s\n", msg);                              \
            failures++;                                             \
        }                                                           \
    } while (0)

int main(void)
{
    const OMI_LispNode* n = omi_lisp_null();

    OMI_CandidateArena arena;
    omi_candidate_arena_init(&arena);

    /* 1. seed candidate maps to PAIR whose car/cdr are OMI_CANDIDATE_NULL. */
    OMI_LispCandidate seed_lisp = omi_lisp_lower_seed();
    OMI_Candidate* seed = omi_candidate_from_lisp_into(&seed_lisp, &arena);
    CHECK(seed != NULL, "seed conversion succeeds");
    CHECK(seed->kind == OMI_CANDIDATE_PAIR, "seed maps to PAIR");
    CHECK(seed->car != NULL && seed->car->kind == OMI_CANDIDATE_NULL,
          "seed car maps to real OMI_CANDIDATE_NULL");
    CHECK(seed->cdr != NULL && seed->cdr->kind == OMI_CANDIDATE_NULL,
          "seed cdr maps to real OMI_CANDIDATE_NULL");

    /* 2. symbol candidate maps to SYMBOL. */
    omi_candidate_arena_init(&arena);
    OMI_LispCandidate sym_lisp = omi_lisp_lower_symbol("a", 1);
    OMI_Candidate* sym = omi_candidate_from_lisp_into(&sym_lisp, &arena);
    CHECK(sym != NULL, "symbol conversion succeeds");
    CHECK(sym->kind == OMI_CANDIDATE_SYMBOL, "symbol maps to SYMBOL");
    CHECK(sym->symbol != NULL, "symbol text present");
    CHECK(sym->car == NULL && sym->cdr == NULL, "symbol has no car/cdr");

    /* 3. pair candidate maps to PAIR whose car/cdr are OMI_CANDIDATE_NULL. */
    omi_candidate_arena_init(&arena);
    OMI_LispCandidate pair_lisp = omi_lisp_lower_pair(n, n, 1);
    OMI_Candidate* pair = omi_candidate_from_lisp_into(&pair_lisp, &arena);
    CHECK(pair != NULL, "pair conversion succeeds");
    CHECK(pair->kind == OMI_CANDIDATE_PAIR, "pair maps to PAIR");
    CHECK(pair->car != NULL && pair->car->kind == OMI_CANDIDATE_NULL,
          "pair car maps to real OMI_CANDIDATE_NULL");
    CHECK(pair->cdr != NULL && pair->cdr->kind == OMI_CANDIDATE_NULL,
          "pair cdr maps to real OMI_CANDIDATE_NULL");

    /* 4. pre-SP pair does not become authoritative (returns NULL). */
    omi_candidate_arena_init(&arena);
    OMI_LispCandidate pre_pair = omi_lisp_lower_pair(n, n, 0);
    OMI_Candidate* pre_pair_c = omi_candidate_from_lisp_into(&pre_pair, &arena);
    CHECK(pre_pair_c == NULL, "pre-SP pair returns NULL (non-authoritative failure)");
    CHECK(omi_candidate_is_authoritative(pre_pair_c) == 0,
          "pre-SP pair is not authoritative");

    /* 5. pre-SP symbol does not become authoritative. */
    omi_candidate_arena_init(&arena);
    OMI_LispCandidate pre_sym = omi_lisp_lower_symbol("a", 0);
    OMI_Candidate* pre_sym_c = omi_candidate_from_lisp_into(&pre_sym, &arena);
    CHECK(omi_candidate_is_authoritative(pre_sym_c) == 0,
          "pre-SP symbol is not authoritative");

    /* 6-8. never accepted / validated / receipted. */
    CHECK(seed->accepted == 0 && seed->validated == 0 && seed->receipted == 0,
          "seed accepted/validated/receipted == 0");

    /* 9. all nodes are non-authoritative. */
    CHECK(omi_candidate_is_authoritative(seed) == 0, "seed candidate not authoritative");
    CHECK(omi_candidate_is_authoritative(sym) == 0, "symbol candidate not authoritative");
    CHECK(omi_candidate_is_authoritative(pair) == 0, "pair candidate not authoritative");

    /* invalid input returns safe NULL candidate (non-authoritative). */
    CHECK(omi_candidate_from_lisp_into(NULL, &arena) == NULL,
          "NULL input returns safe NULL candidate");

    if (failures == 0) {
        printf("\nAll candidate handoff tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
