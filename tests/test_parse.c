/*
 * OMI Layer: test
 * Purpose: Verify the tiny OMI-Lisp parser with caller-owned arena.
 * Authority: Non-authoritative declaration surface.
 *
 * Parsing creates candidates only.
 * Parsing does not evaluate.
 * Parsing does not validate.
 * Parsing does not accept.
 * Parsing does not receipt.
 */

#include <stdio.h>
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
    OMI_CandidateArena arena;
    OMI_ParseArena pa;

    /* 1. NULL input fails. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into(NULL, 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_NULL_INPUT, "NULL input fails");

    /* 2. pre-SP parse fails. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("NULL", 0, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_PRE_SP, "pre-SP parse fails");

    /* 3. "NULL" parses after SP. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("NULL", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "\"NULL\" parses after SP");
    CHECK(cand.is_candidate == 1, "NULL candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "NULL parses as seed pair");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_NULL, "NULL car is NULL node");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_NULL, "NULL cdr is NULL node");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "NULL candidate never accepted/validated/receipted");
    CHECK(cand.root->source_span.ptr == NULL && cand.root->source_span.len == 0,
          "NULL seed source span is zero (static node)");

    /* 4. "a" parses after SP as SYMBOL. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("a", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "\"a\" parses after SP");
    CHECK(cand.is_candidate == 1, "symbol candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_SYMBOL, "symbol candidate root is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root, "a") != 0,
          "symbol text is 'a' via symbol_equals");
    CHECK(cand.root->span.ptr != NULL && cand.root->span.len == 1,
          "parsed symbol span len == 1");
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 1,
          "parsed symbol source span len == 1");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "symbol candidate never accepted/validated/receipted");

    /* 5. "(a . b)" parses after SP as PAIR. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "\"(a . b)\" parses after SP");
    CHECK(cand.is_candidate == 1, "pair candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "pair candidate root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_SYMBOL, "pair car is SYMBOL");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL, "pair cdr is SYMBOL");
    CHECK(omi_lisp_symbol_equals(cand.root->car, "a") != 0,
          "car symbol is 'a' via symbol_equals");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr, "b") != 0,
          "cdr symbol is 'b' via symbol_equals");
    CHECK(cand.root->car->span.ptr != NULL && cand.root->car->span.len == 1,
          "car atom span len == 1");
    CHECK(cand.root->cdr->span.ptr != NULL && cand.root->cdr->span.len == 1,
          "cdr atom span len == 1");
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 7,
          "pair root source span len == 7");
    CHECK(cand.root->car->source_span.ptr != NULL &&
          cand.root->car->source_span.len == 1,
          "pair car source span len == 1 (equals atom span)");
    CHECK(cand.root->cdr->source_span.ptr != NULL &&
          cand.root->cdr->source_span.len == 1,
          "pair cdr source span len == 1");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "pair candidate never accepted/validated/receipted");

    /* 6. "(NULL . NULL)" parses after SP as seed pair. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(NULL . NULL)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "\"(NULL . NULL)\" parses after SP");
    CHECK(cand.is_candidate == 1, "seed candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "seed candidate root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_NULL, "seed car is NULL node");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_NULL, "seed cdr is NULL node");
    CHECK(omi_lisp_candidate_is_seed(&cand),
          "parsed seed matches seed invariant");
    CHECK(cand.root->source_span.ptr != NULL && cand.root->source_span.len == 13,
          "seed pair source span len == 13 for \"(NULL . NULL)\"");

    /* 7. trailing input fails. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("a extra", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_TRAILING, "trailing input fails");

    /* 8. malformed pair fails. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing dot) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a .)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing cdr) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(. b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing car) fails");
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . b", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing paren) fails");

    /* 9-11. accepted == 0, validated == 0, receipted == 0 (verified in cases 3-6). */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("a", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "re-parse succeeds for final flags check");
    CHECK(cand.accepted == 0, "accepted == 0");
    CHECK(cand.validated == 0, "validated == 0");
    CHECK(cand.receipted == 0, "receipted == 0");

    /* 12. parsed candidate converts into OMI_CandidateArena. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "parse succeeds for conversion test");
    omi_candidate_arena_init(&arena);
    OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
    CHECK(cc != NULL, "conversion into arena succeeds");
    CHECK(cc->kind == OMI_CANDIDATE_PAIR, "converted candidate is PAIR");
    CHECK(cc->car != NULL && cc->car->kind == OMI_CANDIDATE_SYMBOL,
          "converted car is SYMBOL");
    CHECK(cc->cdr != NULL && cc->cdr->kind == OMI_CANDIDATE_SYMBOL,
          "converted cdr is SYMBOL");
    CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
          "converted candidate never authoritative");

    /* 13. Two separate arenas hold two separate parse results at once. */
    OMI_ParseArena pa1, pa2;
    omi_parse_arena_init(&pa1);
    omi_parse_arena_init(&pa2);

    OMI_LispCandidate c1, c2;
    r = omi_lisp_parse_candidate_into("a", 1, &pa1, &c1);
    CHECK(r == OMI_PARSE_OK, "arena1 parses 'a'");
    r = omi_lisp_parse_candidate_into("b", 1, &pa2, &c2);
    CHECK(r == OMI_PARSE_OK, "arena2 parses 'b'");

    CHECK(c1.root->kind == OMI_LISP_NODE_SYMBOL, "arena1 root is SYMBOL");
    CHECK(c2.root->kind == OMI_LISP_NODE_SYMBOL, "arena2 root is SYMBOL");
    CHECK(omi_lisp_symbol_equals(c1.root, "a") != 0,
          "arena1 symbol is 'a'");
    CHECK(omi_lisp_symbol_equals(c2.root, "b") != 0,
          "arena2 symbol is 'b'");

    /* Verify c1 result stays valid after c2 parse (independent arenas). */
    CHECK(omi_lisp_symbol_equals(c1.root, "a") != 0,
          "arena1 symbol still 'a' after separate arena parse");

    /* 14. Parse output does not depend on global static pool.
     *     (Verified implicitly: no static globals used for node storage.)
     *     Explicitly: reuse same arena for two different inputs and confirm
     *     that re-init discards prior result cleanly. */
    omi_parse_arena_init(&pa);
    r = omi_lisp_parse_candidate_into("(a . b)", 1, &pa, &cand);
    CHECK(r == OMI_PARSE_OK, "reused arena parses '(a . b)'");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "reused arena root is PAIR");
    CHECK(omi_lisp_symbol_equals(cand.root->car, "a") != 0,
          "reused arena car is 'a'");
    CHECK(omi_lisp_symbol_equals(cand.root->cdr, "b") != 0,
          "reused arena cdr is 'b'");

    if (failures == 0) {
        printf("\nAll parse tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
