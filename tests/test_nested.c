/*
 * OMI Layer: test
 * Purpose: Verify nested pair grammar (structural nesting only).
 * Authority: Non-authoritative declaration surface.
 *
 * Nested structure is still declaration structure.
 * A nested pair is not evaluation, validation, or acceptance.
 */

#include <stdio.h>
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

int main(void)
{
    OMI_LispCandidate cand;
    OMI_ParseResult r;
    OMI_ParseArena pa;
    OMI_CandidateArena ca;

    /* 1. Right-nested: (a . (b . c)) → PAIR(SYMBOL a, PAIR(SYMBOL b, SYMBOL c)). */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . (b . c))", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "(a . (b . c)) parses");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "right-nested root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_SYMBOL, "right-nested car is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->car, "a") != 0,
          "right-nested car symbol 'a'");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_PAIR, "right-nested cdr is PAIR");
    CHECK(cand.root->cdr->car->kind == OMI_LISP_NODE_SYMBOL,
          "right-nested inner car is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr->car, "b") != 0,
          "right-nested inner car symbol 'b'");
    CHECK(cand.root->cdr->cdr->kind == OMI_LISP_NODE_SYMBOL,
          "right-nested inner cdr is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr->cdr, "c") != 0,
          "right-nested inner cdr symbol 'c'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "right-nested never accepted/validated/receipted");

    /* Spot-check atom spans on right-nested form. */
    CHECK(cand.root->car->span.ptr != NULL && cand.root->car->span.len == 1,
          "right-nested car atom span len == 1");
    CHECK(cand.root->cdr->car->span.ptr != NULL &&
          cand.root->cdr->car->span.len == 1,
          "right-nested inner car atom span len == 1");
    CHECK(cand.root->cdr->cdr->span.ptr != NULL &&
          cand.root->cdr->cdr->span.len == 1,
          "right-nested inner cdr atom span len == 1");

    /* Source-span checks on right-nested form:
     *   "(a . (b . c))" = 12 characters
     *   inner pair "(b . c)" = 7 characters
     *   all symbols: source_span == atom span (1 char each) */
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 13,
          "right-nested root source span len == 13");
    CHECK(cand.root->cdr->source_span.ptr != NULL &&
          cand.root->cdr->source_span.len == 7,
          "right-nested cdr (inner pair) source span len == 7");
    CHECK(cand.root->car->source_span.ptr != NULL &&
          cand.root->car->source_span.len == 1,
          "right-nested car source span == atom span (len 1)");

    /* 2. Left-nested: ((a . b) . c) → PAIR(PAIR(SYMBOL a, SYMBOL b), SYMBOL c). */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((a . b) . c)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "((a . b) . c) parses");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "left-nested root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_PAIR, "left-nested car is PAIR");
    CHECK(cand.root->car->car->kind == OMI_LISP_NODE_SYMBOL,
          "left-nested inner car is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->car->car, "a") != 0,
          "left-nested inner car symbol 'a'");
    CHECK(cand.root->car->cdr->kind == OMI_LISP_NODE_SYMBOL,
          "left-nested inner cdr is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->car->cdr, "b") != 0,
          "left-nested inner cdr symbol 'b'");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL,
          "left-nested cdr is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr, "c") != 0,
          "left-nested cdr symbol 'c'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "left-nested never accepted/validated/receipted");
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 13,
          "left-nested root source span len == 13 for \"((a . b) . c)\"");
    CHECK(cand.root->car->source_span.ptr != NULL &&
          cand.root->car->source_span.len == 7,
          "left-nested car (inner pair) source span len == 7");

    /* 3. Seed-nested: ((NULL . NULL) . a) → PAIR(PAIR(NULL, NULL), SYMBOL a). */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((NULL . NULL) . a)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "((NULL . NULL) . a) parses");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "seed-nested root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_PAIR,
          "seed-nested car is PAIR (seed)");
    CHECK(cand.root->car->car->kind == OMI_LISP_NODE_NULL,
          "seed-nested inner car is NULL");
    CHECK(cand.root->car->cdr->kind == OMI_LISP_NODE_NULL,
          "seed-nested inner cdr is NULL");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL,
          "seed-nested cdr is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr, "a") != 0,
          "seed-nested cdr symbol 'a'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "seed-nested never accepted/validated/receipted");

    /* 4. Balanced: ((a . b) . (c . d)) → PAIR(PAIR(a,b), PAIR(c,d)). */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((a . b) . (c . d))", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "((a . b) . (c . d)) parses");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "balanced root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_PAIR, "balanced car is PAIR");
    CHECK(omi_lisp_symbol_equals(cand.root->car->car, "a") != 0,
          "balanced car-car symbol 'a'");
    CHECK(omi_lisp_symbol_equals(cand.root->car->cdr, "b") != 0,
          "balanced car-cdr symbol 'b'");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_PAIR, "balanced cdr is PAIR");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr->car, "c") != 0,
          "balanced cdr-car symbol 'c'");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr->cdr, "d") != 0,
          "balanced cdr-cdr symbol 'd'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "balanced never accepted/validated/receipted");

    /* Spot-check atom spans on balanced form. */
    CHECK(cand.root->car->car->span.ptr != NULL &&
          cand.root->car->car->span.len == 1,
          "balanced car-car atom span len == 1");
    CHECK(cand.root->cdr->cdr->span.ptr != NULL &&
          cand.root->cdr->cdr->span.len == 1,
          "balanced cdr-cdr atom span len == 1");

    /* Source-span checks on balanced form:
     *   "((a . b) . (c . d))" = 18 characters
     *   car "(a . b)" = 7 chars, cdr "(c . d)" = 7 chars */
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 19,
          "balanced root source span len == 19");
    CHECK(cand.root->car->source_span.ptr != NULL &&
          cand.root->car->source_span.len == 7,
          "balanced car (inner pair) source span len == 7");
    CHECK(cand.root->cdr->source_span.ptr != NULL &&
          cand.root->cdr->source_span.len == 7,
          "balanced cdr (inner pair) source span len == 7");

    /* 5. Convert nested form into OMI_CandidateArena, preserving structure. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((a . b) . (c . d))", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "nested parse succeeds for conversion test");
    omi_candidate_arena_init(&ca);
    OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &ca);
    CHECK(cc != NULL, "nested conversion into arena succeeds");
    CHECK(cc->kind == OMI_CANDIDATE_PAIR, "converted root is PAIR");
    CHECK(cc->car->kind == OMI_CANDIDATE_PAIR, "converted car is PAIR ((a . b))");
    CHECK(cc->car->car->kind == OMI_CANDIDATE_SYMBOL, "converted car-car is SYMBOL");
    CHECK(cc->car->cdr->kind == OMI_CANDIDATE_SYMBOL, "converted car-cdr is SYMBOL");
    CHECK(cc->cdr->kind == OMI_CANDIDATE_PAIR, "converted cdr is PAIR ((c . d))");
    CHECK(cc->cdr->car->kind == OMI_CANDIDATE_SYMBOL, "converted cdr-car is SYMBOL");
    CHECK(cc->cdr->cdr->kind == OMI_CANDIDATE_SYMBOL, "converted cdr-cdr is SYMBOL");
    CHECK(cc->source_span.ptr != NULL && cc->source_span.len == 19,
          "converted root source span preserved (len == 19)");
    CHECK(cc->car->source_span.ptr != NULL && cc->car->source_span.len == 7,
          "converted car source span preserved (len == 7)");
    CHECK(cc->cdr->source_span.ptr != NULL && cc->cdr->source_span.len == 7,
          "converted cdr source span preserved (len == 7)");
    CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
          "converted nested never authoritative");

    /* 6. Depth overflow test: 17 nested pairs should exceed OMI_PARSE_MAX_DEPTH (16).
     *    Build (a . (a . (a . ... 17 deep ... (a . a)))). */
    {
        const char* deep =
            "(a . (a . (a . (a . (a . (a . (a . (a . "
            "(a . (a . (a . (a . (a . (a . (a . (a . "
            "(a . a))))))))))))))))";
        omi_parse_arena_init(&pa);
        r = omi_lisp_parse_candidate_into(deep, 1, &pa, &cand);
        CHECK(r == OMI_PARSE_ERR_DEPTH, "17-deep nesting returns ERR_DEPTH");
    }

    /* 7. Malformed nested forms fail. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((a . b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED,
          "malformed ((a . b) (missing outer paren) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . (b . c)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED,
          "malformed (a . (b . c) (missing inner paren) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("((a . b) . )", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED,
          "malformed ((a . b) . ) (missing cdr) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(. (a . b))", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED,
          "malformed (. (a . b)) (missing car) fails");

    /* 8. All candidates verified above have accepted=0, validated=0, receipted=0. */

    if (failures == 0) {
        printf("\nAll nested pair tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
