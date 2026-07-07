/*
 * OMI Layer: test
 * Purpose: Verify adapter shape stub: OMI_Candidate -> OMI_AdapterNode handoff.
 *          No validation, no acceptance, no receipts, no projection.
 * Authority: Non-authoritative declaration surface.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omi_lisp.h"
#include "omi_parse.h"
#include "omi_candidate.h"
#include "omi_adapter_contract.h"

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
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 0) {
        fclose(f);
        return NULL;
    }
    char* buf = (char*)malloc((size_t)len + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    size_t n = fread(buf, 1, (size_t)len, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

int main(void)
{
    OMI_ParseArena pa;
    OMI_LispCandidate cand;
    OMI_CandidateArena ca;
    OMI_AdapterArena aa;

    /* 1. Parse nested-balanced.omi from disk. */
    char* content = read_file("tests/fixtures/nested-balanced.omi");
    CHECK(content != NULL, "nested-balanced.omi read from disk");
    if (content == NULL) {
        printf("\n%d test(s) failed.\n", failures);
        return 1;
    }

    omi_parse_arena_init(&pa);
    int r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "nested-balanced.omi parses");
    CHECK(cand.is_candidate == 1, "nested-balanced candidate is_candidate");

    /* 2. Convert to OMI_CandidateArena. */
    omi_candidate_arena_init(&ca);
    OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &ca);
    CHECK(cc != NULL, "nested-balanced converts to OMI_CandidateArena");

    /* 3. Convert to OMI_AdapterArena. */
    omi_adapter_arena_init(&aa);
    OMI_AdapterNode* ac = omi_adapter_from_candidate(cc, &aa);
    CHECK(ac != NULL, "nested-balanced converts to OMI_AdapterArena");

    /* 4. Verify root PAIR. */
    CHECK(ac->kind == OMI_ADAPTER_KIND_PAIR, "adapter root is PAIR");

    /* 5. Verify car/cdr are nested PAIRs. */
    CHECK(ac->car != NULL && ac->car->kind == OMI_ADAPTER_KIND_PAIR,
          "adapter car is PAIR");
    CHECK(ac->cdr != NULL && ac->cdr->kind == OMI_ADAPTER_KIND_PAIR,
          "adapter cdr is PAIR");

    /* 6. Verify car-car and car-cdr are SYMBOLs. */
    CHECK(ac->car->car != NULL && ac->car->car->kind == OMI_ADAPTER_KIND_SYMBOL,
          "adapter car-car is SYMBOL");
    CHECK(ac->car->cdr != NULL && ac->car->cdr->kind == OMI_ADAPTER_KIND_SYMBOL,
          "adapter car-cdr is SYMBOL");

    /* 7. Verify cdr-car and cdr-cdr are SYMBOLs. */
    CHECK(ac->cdr->car != NULL && ac->cdr->car->kind == OMI_ADAPTER_KIND_SYMBOL,
          "adapter cdr-car is SYMBOL");
    CHECK(ac->cdr->cdr != NULL && ac->cdr->cdr->kind == OMI_ADAPTER_KIND_SYMBOL,
          "adapter cdr-cdr is SYMBOL");

    /* 8. Verify symbol atom_span preserved. */
    CHECK(ac->car->car->atom_span.ptr != NULL && ac->car->car->atom_span.len == 1,
          "adapter car-car atom_span len == 1");
    CHECK(ac->car->cdr->atom_span.ptr != NULL && ac->car->cdr->atom_span.len == 1,
          "adapter car-cdr atom_span len == 1");
    CHECK(ac->cdr->car->atom_span.ptr != NULL && ac->cdr->car->atom_span.len == 1,
          "adapter cdr-car atom_span len == 1");
    CHECK(ac->cdr->cdr->atom_span.ptr != NULL && ac->cdr->cdr->atom_span.len == 1,
          "adapter cdr-cdr atom_span len == 1");

    /* 9. Verify source_span preserved on all nodes. */
    CHECK(ac->source_span.ptr != NULL && ac->source_span.len == 19,
          "adapter root source_span len == 19");
    CHECK(ac->car->source_span.ptr != NULL && ac->car->source_span.len == 7,
          "adapter car source_span len == 7");
    CHECK(ac->cdr->source_span.ptr != NULL && ac->cdr->source_span.len == 7,
          "adapter cdr source_span len == 7");
    CHECK(ac->car->car->source_span.ptr != NULL &&
          ac->car->car->source_span.len == 1,
          "adapter car-car source_span len == 1");
    CHECK(ac->cdr->cdr->source_span.ptr != NULL &&
          ac->cdr->cdr->source_span.len == 1,
          "adapter cdr-cdr source_span len == 1");

    /* 10. Verify accepted/validated/receipted remain 0. */
    CHECK(ac->accepted == 0 && ac->validated == 0 && ac->receipted == 0,
          "adapter root accepted/validated/receipted == 0");
    CHECK(ac->car->accepted == 0 && ac->car->validated == 0 &&
          ac->car->receipted == 0,
          "adapter car accepted/validated/receipted == 0");
    CHECK(ac->cdr->accepted == 0 && ac->cdr->validated == 0 &&
          ac->cdr->receipted == 0,
          "adapter cdr accepted/validated/receipted == 0");
    CHECK(ac->car->car->accepted == 0 && ac->car->car->validated == 0 &&
          ac->car->car->receipted == 0,
          "adapter car-car accepted/validated/receipted == 0");

    /* 11. Verify authoritative check returns 0. */
    CHECK(omi_adapter_is_authoritative(ac) == 0,
          "adapter_is_authoritative(root) == 0");
    CHECK(omi_adapter_is_authoritative(ac->car) == 0,
          "adapter_is_authoritative(car) == 0");
    CHECK(omi_adapter_is_authoritative(NULL) == 0,
          "adapter_is_authoritative(NULL) == 0");

    /* 12. Verify seed conversion (NULL node). */
    OMI_LispCandidate seed_cand;
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("NULL", 1, &pa, &seed_cand);
    CHECK(r == OMI_PARSE_OK, "\"NULL\" parses for seed conversion");
    omi_candidate_arena_init(&ca);
    OMI_Candidate* seed_cc = omi_candidate_from_lisp_into(&seed_cand, &ca);
    CHECK(seed_cc != NULL, "seed converts to OMI_CandidateArena");
    omi_adapter_arena_init(&aa);
    OMI_AdapterNode* seed_ac = omi_adapter_from_candidate(seed_cc, &aa);
    CHECK(seed_ac != NULL, "seed converts to OMI_AdapterArena");
    CHECK(seed_ac->kind == OMI_ADAPTER_KIND_PAIR,
          "seed adapter root is PAIR");
    CHECK(seed_ac->car != NULL && seed_ac->car->kind == OMI_ADAPTER_KIND_NULL,
          "seed adapter car is NULL");
    CHECK(seed_ac->cdr != NULL && seed_ac->cdr->kind == OMI_ADAPTER_KIND_NULL,
          "seed adapter cdr is NULL");
    CHECK(seed_ac->car->accepted == 0 && seed_ac->car->validated == 0 &&
          seed_ac->car->receipted == 0,
          "seed adapter NULL node never authoritative");
    CHECK(omi_adapter_is_authoritative(seed_ac) == 0,
          "seed adapter_is_authoritative == 0");

    /* 13. Verify NULL candidate returns NULL. */
    omi_adapter_arena_init(&aa);
    OMI_AdapterNode* null_ac = omi_adapter_from_candidate(NULL, &aa);
    CHECK(null_ac == NULL, "adapter_from_candidate(NULL) returns NULL");

    /* 14. Verify NULL arena returns NULL. */
    omi_adapter_arena_init(&aa);
    null_ac = omi_adapter_from_candidate(cc, NULL);
    CHECK(null_ac == NULL, "adapter_from_candidate(c, NULL) returns NULL");

    free(content);

    if (failures == 0) {
        printf("\nAll adapter contract tests passed.\n");
    } else {
        printf("\n%d test(s) failed.\n", failures);
    }
    return failures > 0 ? 1 : 0;
}
