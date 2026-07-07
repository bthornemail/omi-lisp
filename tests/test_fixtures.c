/*
 * OMI Layer: test
 * Purpose: Verify the tiny parser reads repo-level .omi fixture files from disk.
 * Authority: Non-authoritative declaration surface.
 *
 * Fixtures are declaration fixtures only — parsed, never validated or receipted.
 * No new grammar, no evaluation, no validation, no receipt, no authority.
 *
 * Uses caller-owned OMI_ParseArena for all parses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omi_lisp.h"
#include "omi_parse.h"
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

/* Reads a file into a heap-allocated buffer. Caller must free. */
static char* read_file(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    char* buf = (char*)malloc((size_t)size + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    size_t nread = fread(buf, 1, (size_t)size, f);
    fclose(f);
    buf[nread] = '\0';
    /* Strip trailing whitespace (fixtures have a newline). */
    while (nread > 0 && (buf[nread - 1] == '\n' || buf[nread - 1] == '\r' ||
                         buf[nread - 1] == ' ' || buf[nread - 1] == '\t')) {
        buf[--nread] = '\0';
    }
    return buf;
}

int main(void)
{
    OMI_LispCandidate cand;
    OMI_ParseResult r;
    OMI_CandidateArena arena;
    OMI_ParseArena pa;
    char* content;

    /* 1. seed.omi: "(NULL . NULL)" → PAIR(NULL, NULL). */
    content = read_file("tests/fixtures/seed.omi");
    CHECK(content != NULL, "seed.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "seed.omi parses");
        CHECK(cand.is_candidate == 1, "seed.omi candidate is_candidate");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "seed.omi root is PAIR");
        CHECK(cand.root->car->kind == OMI_LISP_NODE_NULL, "seed.omi car is NULL");
        CHECK(cand.root->cdr->kind == OMI_LISP_NODE_NULL, "seed.omi cdr is NULL");
        CHECK(omi_lisp_candidate_is_seed(&cand), "seed.omi matches seed invariant");
        CHECK(cand.accepted == 0, "seed.omi accepted == 0");
        CHECK(cand.validated == 0, "seed.omi validated == 0");
        CHECK(cand.receipted == 0, "seed.omi receipted == 0");
        free(content);
    }

    /* 2. pair.omi: "(a . b)" → PAIR(SYMBOL a, SYMBOL b). */
    content = read_file("tests/fixtures/pair.omi");
    CHECK(content != NULL, "pair.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "pair.omi parses");
        CHECK(cand.is_candidate == 1, "pair.omi candidate is_candidate");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "pair.omi root is PAIR");
        CHECK(cand.root->car->kind == OMI_LISP_NODE_SYMBOL, "pair.omi car is SYMBOL");
        CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL, "pair.omi cdr is SYMBOL");
        CHECK(omi_lisp_symbol_equals(cand.root->car, "a") != 0,
              "pair.omi car symbol equals 'a'");
        CHECK(cand.root->car->span.ptr != NULL && cand.root->car->span.len == 1,
              "pair.omi car atom span len == 1");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr, "b") != 0,
              "pair.omi cdr symbol equals 'b'");
        CHECK(cand.root->cdr->span.ptr != NULL && cand.root->cdr->span.len == 1,
              "pair.omi cdr atom span len == 1");
        CHECK(cand.accepted == 0, "pair.omi accepted == 0");
        CHECK(cand.validated == 0, "pair.omi validated == 0");
        CHECK(cand.receipted == 0, "pair.omi receipted == 0");
        free(content);
    }

    /* 3. symbol.omi: "a" → SYMBOL a. */
    content = read_file("tests/fixtures/symbol.omi");
    CHECK(content != NULL, "symbol.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "symbol.omi parses");
        CHECK(cand.is_candidate == 1, "symbol.omi candidate is_candidate");
        CHECK(cand.root->kind == OMI_LISP_NODE_SYMBOL, "symbol.omi root is SYMBOL");
        CHECK(omi_lisp_symbol_equals(cand.root, "a") != 0,
              "symbol.omi symbol text is 'a' via symbol_equals");
        CHECK(cand.root->span.ptr != NULL && cand.root->span.len == 1,
              "symbol.omi atom span len == 1");
        CHECK(cand.accepted == 0, "symbol.omi accepted == 0");
        CHECK(cand.validated == 0, "symbol.omi validated == 0");
        CHECK(cand.receipted == 0, "symbol.omi receipted == 0");
        free(content);
    }

    /* 4. Separate arenas for seed.omi and pair.omi preserve both results. */
    {
        char* seed_src = read_file("tests/fixtures/seed.omi");
        char* pair_src = read_file("tests/fixtures/pair.omi");
        CHECK(seed_src != NULL, "seed.omi re-read for concurrent arena test");
        CHECK(pair_src != NULL, "pair.omi re-read for concurrent arena test");

        if (seed_src != NULL && pair_src != NULL) {
            OMI_ParseArena seed_pa, pair_pa;
            OMI_LispCandidate seed_cand, pair_cand;

            omi_parse_arena_init(&seed_pa);
            r = omi_lisp_parse_candidate_into(seed_src, 1, &seed_pa, &seed_cand);
            CHECK(r == OMI_PARSE_OK, "seed.omi parses into its own arena");

            omi_parse_arena_init(&pair_pa);
            r = omi_lisp_parse_candidate_into(pair_src, 1, &pair_pa, &pair_cand);
            CHECK(r == OMI_PARSE_OK, "pair.omi parses into its own arena");

            /* Verify both results independently. */
            CHECK(omi_lisp_candidate_is_seed(&seed_cand),
                  "seed.omi still matches seed invariant after concurrent parse");
            CHECK(pair_cand.root->kind == OMI_LISP_NODE_PAIR,
                  "pair.omi root is still PAIR after concurrent parse");
            CHECK(omi_lisp_symbol_equals(pair_cand.root->car, "a") != 0,
                  "pair.omi car still 'a' after concurrent parse");
            CHECK(omi_lisp_symbol_equals(pair_cand.root->cdr, "b") != 0,
                  "pair.omi cdr still 'b' after concurrent parse");
        }

        free(seed_src);
        free(pair_src);
    }

    /* 5. All parsed candidates convert into OMI_CandidateArena. */
    content = read_file("tests/fixtures/pair.omi");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "pair.omi parses for conversion test");
        omi_candidate_arena_init(&arena);
        OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
        CHECK(cc != NULL, "pair.omi converts into arena");
        CHECK(cc->kind == OMI_CANDIDATE_PAIR, "pair.omi converted is PAIR");
        CHECK(cc->car != NULL && cc->car->kind == OMI_CANDIDATE_SYMBOL,
              "pair.omi converted car is SYMBOL");
        CHECK(cc->cdr != NULL && cc->cdr->kind == OMI_CANDIDATE_SYMBOL,
              "pair.omi converted cdr is SYMBOL");
        CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
              "pair.omi converted never authoritative");
        free(content);
    }

    /* seed.omi — also converts. */
    content = read_file("tests/fixtures/seed.omi");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "seed.omi parses for conversion test");
        omi_candidate_arena_init(&arena);
        OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
        CHECK(cc != NULL, "seed.omi converts into arena");
        CHECK(cc->kind == OMI_CANDIDATE_PAIR, "seed.omi converted is PAIR");
        CHECK(cc->car != NULL && cc->car->kind == OMI_CANDIDATE_NULL,
              "seed.omi converted car is NULL");
        CHECK(cc->cdr != NULL && cc->cdr->kind == OMI_CANDIDATE_NULL,
              "seed.omi converted cdr is NULL");
        CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
              "seed.omi converted never authoritative");
        free(content);
    }

    /* symbol.omi — also converts. */
    content = read_file("tests/fixtures/symbol.omi");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "symbol.omi parses for conversion test");
        omi_candidate_arena_init(&arena);
        OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
        CHECK(cc != NULL, "symbol.omi converts into arena");
        CHECK(cc->kind == OMI_CANDIDATE_SYMBOL, "symbol.omi converted is SYMBOL");
        CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
              "symbol.omi converted never authoritative");
        free(content);
    }

    /* 6. Nested fixtures parse correctly from disk. */
    content = read_file("tests/fixtures/nested-right.omi");
    CHECK(content != NULL, "nested-right.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "nested-right.omi parses");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR,
              "nested-right.omi root is PAIR");
        CHECK(cand.root->cdr->kind == OMI_LISP_NODE_PAIR,
              "nested-right.omi cdr is PAIR");
        CHECK(omi_lisp_symbol_equals(cand.root->car, "a") != 0,
              "nested-right.omi car symbol 'a'");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr->car, "b") != 0,
              "nested-right.omi inner car symbol 'b'");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr->cdr, "c") != 0,
              "nested-right.omi inner cdr symbol 'c'");
        CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
              "nested-right.omi never accepted/validated/receipted");
        free(content);
    }

    content = read_file("tests/fixtures/nested-left.omi");
    CHECK(content != NULL, "nested-left.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "nested-left.omi parses");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR,
              "nested-left.omi root is PAIR");
        CHECK(cand.root->car->kind == OMI_LISP_NODE_PAIR,
              "nested-left.omi car is PAIR");
        CHECK(omi_lisp_symbol_equals(cand.root->car->car, "a") != 0,
              "nested-left.omi inner car symbol 'a'");
        CHECK(omi_lisp_symbol_equals(cand.root->car->cdr, "b") != 0,
              "nested-left.omi inner cdr symbol 'b'");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr, "c") != 0,
              "nested-left.omi cdr symbol 'c'");
        CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
              "nested-left.omi never accepted/validated/receipted");
        free(content);
    }

    content = read_file("tests/fixtures/nested-balanced.omi");
    CHECK(content != NULL, "nested-balanced.omi read from disk");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "nested-balanced.omi parses");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR,
              "nested-balanced.omi root is PAIR");
        CHECK(cand.root->car->kind == OMI_LISP_NODE_PAIR,
              "nested-balanced.omi car is PAIR");
        CHECK(cand.root->cdr->kind == OMI_LISP_NODE_PAIR,
              "nested-balanced.omi cdr is PAIR");
        CHECK(omi_lisp_symbol_equals(cand.root->car->car, "a") != 0,
              "nested-balanced.omi car-car symbol 'a'");
        CHECK(omi_lisp_symbol_equals(cand.root->car->cdr, "b") != 0,
              "nested-balanced.omi car-cdr symbol 'b'");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr->car, "c") != 0,
              "nested-balanced.omi cdr-car symbol 'c'");
        CHECK(omi_lisp_symbol_equals(cand.root->cdr->cdr, "d") != 0,
              "nested-balanced.omi cdr-cdr symbol 'd'");
        CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
              "nested-balanced.omi never accepted/validated/receipted");
        free(content);
    }

    /* 7. Nested fixtures convert into OMI_CandidateArena. */
    content = read_file("tests/fixtures/nested-balanced.omi");
    if (content != NULL) {
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(content, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_OK, "nested-balanced.omi parses for conversion");
        omi_candidate_arena_init(&arena);
        OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
        CHECK(cc != NULL, "nested-balanced.omi converts into arena");
        CHECK(cc->kind == OMI_CANDIDATE_PAIR, "nested-balanced converted root PAIR");
        CHECK(cc->car->kind == OMI_CANDIDATE_PAIR,
              "nested-balanced converted car PAIR (a . b)");
        CHECK(cc->car->car->kind == OMI_CANDIDATE_SYMBOL,
              "nested-balanced converted car-car SYMBOL 'a'");
        CHECK(cc->car->cdr->kind == OMI_CANDIDATE_SYMBOL,
              "nested-balanced converted car-cdr SYMBOL 'b'");
        CHECK(cc->cdr->kind == OMI_CANDIDATE_PAIR,
              "nested-balanced converted cdr PAIR (c . d)");
        CHECK(cc->cdr->car->kind == OMI_CANDIDATE_SYMBOL,
              "nested-balanced converted cdr-car SYMBOL 'c'");
        CHECK(cc->cdr->cdr->kind == OMI_CANDIDATE_SYMBOL,
              "nested-balanced converted cdr-cdr SYMBOL 'd'");
        CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
              "nested-balanced converted never authoritative");
        free(content);
    }

    /* 8. All remain accepted=0, validated=0, receipted=0 (verified per-fixture above). */

    if (failures == 0) {
        printf("\nAll fixture tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
