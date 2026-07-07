/*
 * OMI Layer: test
 * Purpose: Verify the tiny OMI-Lisp parser.
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

    /* 1. NULL input fails. */
    r = omi_lisp_parse_candidate(NULL, 1, &cand);
    CHECK(r == OMI_PARSE_ERR_NULL_INPUT, "NULL input fails");

    /* 2. pre-SP parse fails. */
    r = omi_lisp_parse_candidate("NULL", 0, &cand);
    CHECK(r == OMI_PARSE_ERR_PRE_SP, "pre-SP parse fails");

    /* 3. "NULL" parses after SP. */
    r = omi_lisp_parse_candidate("NULL", 1, &cand);
    CHECK(r == OMI_PARSE_OK, "\"NULL\" parses after SP");
    CHECK(cand.is_candidate == 1, "NULL candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "NULL parses as seed pair");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_NULL, "NULL car is NULL node");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_NULL, "NULL cdr is NULL node");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "NULL candidate never accepted/validated/receipted");

    /* 4. "a" parses after SP as SYMBOL. */
    r = omi_lisp_parse_candidate("a", 1, &cand);
    CHECK(r == OMI_PARSE_OK, "\"a\" parses after SP");
    CHECK(cand.is_candidate == 1, "symbol candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_SYMBOL, "symbol candidate root is SYMBOL");
    CHECK(cand.root->symbol != NULL && cand.root->symbol[0] == 'a',
          "symbol text is 'a'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "symbol candidate never accepted/validated/receipted");

    /* 5. "(a . b)" parses after SP as PAIR. */
    r = omi_lisp_parse_candidate("(a . b)", 1, &cand);
    CHECK(r == OMI_PARSE_OK, "\"(a . b)\" parses after SP");
    CHECK(cand.is_candidate == 1, "pair candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "pair candidate root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_SYMBOL, "pair car is SYMBOL");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_SYMBOL, "pair cdr is SYMBOL");
    CHECK(cand.root->car->symbol != NULL && cand.root->car->symbol[0] == 'a',
          "car symbol is 'a'");
    CHECK(cand.root->cdr->symbol != NULL && cand.root->cdr->symbol[0] == 'b',
          "cdr symbol is 'b'");
    CHECK(cand.accepted == 0 && cand.validated == 0 && cand.receipted == 0,
          "pair candidate never accepted/validated/receipted");

    /* 6. "(NULL . NULL)" parses after SP as seed pair. */
    r = omi_lisp_parse_candidate("(NULL . NULL)", 1, &cand);
    CHECK(r == OMI_PARSE_OK, "\"(NULL . NULL)\" parses after SP");
    CHECK(cand.is_candidate == 1, "seed candidate is_candidate");
    CHECK(cand.root->kind == OMI_LISP_NODE_PAIR, "seed candidate root is PAIR");
    CHECK(cand.root->car->kind == OMI_LISP_NODE_NULL, "seed car is NULL node");
    CHECK(cand.root->cdr->kind == OMI_LISP_NODE_NULL, "seed cdr is NULL node");
    CHECK(omi_lisp_candidate_is_seed(&cand),
          "parsed seed matches seed invariant");

    /* 7. trailing input fails. */
    r = omi_lisp_parse_candidate("a extra", 1, &cand);
    CHECK(r == OMI_PARSE_ERR_TRAILING, "trailing input fails");

    /* 8. malformed pair fails. */
    r = omi_lisp_parse_candidate("(a b)", 1, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing dot) fails");
    r = omi_lisp_parse_candidate("(a .)", 1, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing cdr) fails");
    r = omi_lisp_parse_candidate("(. b)", 1, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing car) fails");
    r = omi_lisp_parse_candidate("(a . b", 1, &cand);
    CHECK(r == OMI_PARSE_ERR_UNEXPECTED, "malformed pair (missing paren) fails");

    /* 9-11. accepted == 0, validated == 0, receipted == 0 (verified in cases 3-6). */
    CHECK(cand.accepted == 0, "accepted == 0");
    CHECK(cand.validated == 0, "validated == 0");
    CHECK(cand.receipted == 0, "receipted == 0");

    /* 12. parsed candidate converts into OMI_CandidateArena. */
    omi_candidate_arena_init(&arena);
    r = omi_lisp_parse_candidate("(a . b)", 1, &cand);
    OMI_Candidate* cc = omi_candidate_from_lisp_into(&cand, &arena);
    CHECK(r == OMI_PARSE_OK, "parse succeeds for conversion test");
    CHECK(cc != NULL, "conversion into arena succeeds");
    CHECK(cc->kind == OMI_CANDIDATE_PAIR, "converted candidate is PAIR");
    CHECK(cc->car != NULL && cc->car->kind == OMI_CANDIDATE_SYMBOL,
          "converted car is SYMBOL");
    CHECK(cc->cdr != NULL && cc->cdr->kind == OMI_CANDIDATE_SYMBOL,
          "converted cdr is SYMBOL");
    CHECK(cc->accepted == 0 && cc->validated == 0 && cc->receipted == 0,
          "converted candidate never authoritative");

    if (failures == 0) {
        printf("\nAll parse tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}