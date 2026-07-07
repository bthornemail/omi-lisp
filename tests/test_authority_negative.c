/*
 * OMI Layer: test
 * Purpose: Prove no layer currently permits accepted/validated/receipted
 *          to become authority. Even manual flag mutation cannot accidentally
 *          confer authority before real integration.
 * Authority: Non-authoritative declaration surface.
 *
 * This test parses a real fixture, converts through every non-authoritative
 * layer, and at each boundary verifies that is_authoritative returns 0
 * regardless of flag state. The stub implementations are hardcoded to 0;
 * this test proves they stay that way.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omi_lisp.h"
#include "omi_parse.h"
#include "omi_candidate.h"
#include "omi_adapter_contract.h"
#include "omi_canvas_boundary.h"

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

static char* read_file(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 0) { fclose(f); return NULL; }
    char* buf = (char*)malloc((size_t)len + 1);
    if (buf == NULL) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

int main(void)
{
    char* content = read_file("tests/fixtures/nested-balanced.omi");
    CHECK(content != NULL, "nested-balanced.omi read from disk");
    if (content == NULL) {
        printf("\n%d test(s) failed.\n", failures);
        return 1;
    }

    OMI_ParseArena pa;
    OMI_LispCandidate cand;
    OMI_CandidateArena ca;
    OMI_AdapterArena aa;

    /* === Layer 1: Parse === */
    omi_parse_arena_init(&pa);
    int r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "parse succeeds");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "OMI_LispCandidate authority flags start at 0");

    /* === Layer 2: OMI_Candidate === */
    omi_candidate_arena_init(&ca);
    OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &ca);
    CHECK(cc != NULL, "candidate conversion succeeds");
    CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
          "OMI_Candidate authority flags start at 0");
    CHECK(omi_candidate_is_authoritative(cc) == 0,
          "omi_candidate_is_authoritative returns 0 on clean node");
    CHECK(omi_candidate_is_authoritative(NULL) == 0,
          "omi_candidate_is_authoritative(NULL) returns 0");

    /* Attempt manual flag mutation on a separate copy arena.
     * Even with flags set, the stub currently returns 0 because it is
     * hardcoded: (accepted==0 && validated==0 && receipted==0) ? 0 : 0.
     * After real integration this will change; this test proves the
     * current stub behavior. */
    omi_candidate_arena_init(&ca);
    OMI_Candidate* mutated_cc = omi_candidate_from_lisp_into(&cand, &ca);
    CHECK(mutated_cc != NULL, "separate arena copy succeeded");
    mutated_cc->accepted = 1;
    mutated_cc->validated = 1;
    mutated_cc->receipted = 1;
    CHECK(mutated_cc->accepted == 1 && mutated_cc->validated == 1 &&
          mutated_cc->receipted == 1,
          "manual flag mutation on candidate succeeded");
    CHECK(omi_candidate_is_authoritative(mutated_cc) == 0,
          "omi_candidate_is_authoritative returns 0 even after flag mutation "
          "(stub hardcoded to 0)");

    /* === Layer 3: OMI_AdapterNode === */
    omi_adapter_arena_init(&aa);
    OMI_AdapterNode* ac = omi_adapter_from_candidate(cc, &aa);
    CHECK(ac != NULL, "adapter conversion succeeds");
    CHECK(ac->accepted == 0 && ac->validated == 0 && ac->receipted == 0,
          "OMI_AdapterNode authority flags start at 0");
    CHECK(omi_adapter_is_authoritative(ac) == 0,
          "omi_adapter_is_authoritative returns 0 on clean node");
    CHECK(omi_adapter_is_authoritative(NULL) == 0,
          "omi_adapter_is_authoritative(NULL) returns 0");

    /* Mutate adapter flags on a separate copy. */
    omi_adapter_arena_init(&aa);
    OMI_AdapterNode* mutated_ac = omi_adapter_from_candidate(cc, &aa);
    CHECK(mutated_ac != NULL, "separate adapter arena copy succeeded");
    mutated_ac->accepted = 1;
    mutated_ac->validated = 1;
    mutated_ac->receipted = 1;
    CHECK(mutated_ac->accepted == 1 && mutated_ac->validated == 1 &&
          mutated_ac->receipted == 1,
          "manual flag mutation on adapter node succeeded");
    CHECK(omi_adapter_is_authoritative(mutated_ac) == 0,
          "omi_adapter_is_authoritative returns 0 even after flag mutation "
          "(stub hardcoded to 0)");

    /* === Layer 4: OMI_CanvasBoundaryInput === */
    OMI_CanvasBoundaryInput input_clean = {ac, 0, 0, 0};
    CHECK(input_clean.accepted == 0 && input_clean.validated == 0 &&
          input_clean.receipted == 0,
          "OMI_CanvasBoundaryInput flags start at 0");

    OMI_CanvasBoundaryInput input_mutated = {ac, 1, 1, 1};
    CHECK(input_mutated.accepted == 1 && input_mutated.validated == 1 &&
          input_mutated.receipted == 1,
          "OMI_CanvasBoundaryInput flags can be set to 1");

    /* The boundary function is declared but not implemented.
     * Verify the enum guard exists. */
    CHECK(OMI_CANVAS_BOUNDARY_NOT_AUTHORITY == 3,
          "OMI_CANVAS_BOUNDARY_NOT_AUTHORITY == 3 (explicit guard)");

    /* Function pointer proves the declaration is linkable. */
    OMI_CanvasBoundaryResult (*fp)(const OMI_CanvasBoundaryInput*) =
        omi_canvas_boundary_check_shape;
    CHECK(fp != NULL, "omi_canvas_boundary_check_shape declared");

    /* === Layer 0: OMI_LispCandidate flags are struct fields, not functions.
     * But the canary is that after every conversion they remain 0. */
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "OMI_LispCandidate flags still 0 after all conversions");

    /* === Cross-layer invariant === */
    CHECK(omi_candidate_is_authoritative(cc) == 0,
          "cross-layer: candidate authoritative still 0");
    CHECK(omi_adapter_is_authoritative(ac) == 0,
          "cross-layer: adapter authoritative still 0");
    CHECK(omi_candidate_is_authoritative(mutated_cc) == 0,
          "cross-layer: mutated candidate authoritative still 0 (stub)");
    CHECK(omi_adapter_is_authoritative(mutated_ac) == 0,
          "cross-layer: mutated adapter authoritative still 0 (stub)");

    free(content);

    if (failures == 0) {
        printf("\nAll authority negative tests passed.\n");
    } else {
        printf("\n%d test(s) failed.\n", failures);
    }
    return failures > 0 ? 1 : 0;
}
