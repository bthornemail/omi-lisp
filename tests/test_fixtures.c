/*
 * OMI Layer: test
 * Purpose: Verify the tiny parser reads repo-level .omi fixture files from disk.
 * Authority: Non-authoritative declaration surface.
 *
 * Fixtures are declaration fixtures only — parsed, never validated or receipted.
 * No new grammar, no evaluation, no validation, no receipt, no authority.
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
    char* content;

    /* 1. seed.omi: "(NULL . NULL)" → PAIR(NULL, NULL). */
    content = read_file("tests/fixtures/seed.omi");
    CHECK(content != NULL, "seed.omi read from disk");
    if (content != NULL) {
        r = omi_lisp_parse_candidate(content, 1, &cand);
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
        r = omi_lisp_parse_candidate(content, 1, &cand);
        CHECK(r == OMI_PARSE_OK, "pair.omi parses");
        CHECK(cand.is_candidate == 1, "pair.omi candidate is_candidate");
        CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "pair.omi root is PAIR");
        CHECK(cand.root->car->kind == OMI_LISP_NODE_SYMBOL, "pair.omi car is SYMBOL");
        CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL, "pair.omi cdr is SYMBOL");
        CHECK(cand.root->car->symbol != NULL &&
              cand.root->car->symbol[0] == 'a',
              "pair.omi car symbol starts with 'a'");
        CHECK(cand.root->cdr->symbol != NULL &&
              cand.root->cdr->symbol[0] == 'b',
              "pair.omi cdr symbol starts with 'b'");
        CHECK(cand.accepted == 0, "pair.omi accepted == 0");
        CHECK(cand.validated == 0, "pair.omi validated == 0");
        CHECK(cand.receipted == 0, "pair.omi receipted == 0");
        free(content);
    }

    /* 3. symbol.omi: "a" → SYMBOL a. */
    content = read_file("tests/fixtures/symbol.omi");
    CHECK(content != NULL, "symbol.omi read from disk");
    if (content != NULL) {
        r = omi_lisp_parse_candidate(content, 1, &cand);
        CHECK(r == OMI_PARSE_OK, "symbol.omi parses");
        CHECK(cand.is_candidate == 1, "symbol.omi candidate is_candidate");
        CHECK(cand.root->kind == OMI_LISP_NODE_SYMBOL, "symbol.omi root is SYMBOL");
        CHECK(cand.root->symbol != NULL &&
              strcmp(cand.root->symbol, "a") == 0,
              "symbol.omi symbol text is 'a'");
        CHECK(cand.accepted == 0, "symbol.omi accepted == 0");
        CHECK(cand.validated == 0, "symbol.omi validated == 0");
        CHECK(cand.receipted == 0, "symbol.omi receipted == 0");
        free(content);
    }

    /* 4. All parsed candidates convert into OMI_CandidateArena. */
    content = read_file("tests/fixtures/pair.omi");
    if (content != NULL) {
        r = omi_lisp_parse_candidate(content, 1, &cand);
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
        r = omi_lisp_parse_candidate(content, 1, &cand);
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
        r = omi_lisp_parse_candidate(content, 1, &cand);
        CHECK(r == OMI_PARSE_OK, "symbol.omi parses for conversion test");
        omi_candidate_arena_init(&arena);
        OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
        CHECK(cc != NULL, "symbol.omi converts into arena");
        CHECK(cc->kind == OMI_CANDIDATE_SYMBOL, "symbol.omi converted is SYMBOL");
        CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
              "symbol.omi converted never authoritative");
        free(content);
    }

    /* 5. All remain accepted=0, validated=0, receipted=0 (verified per-fixture above). */

    if (failures == 0) {
        printf("\nAll fixture tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
