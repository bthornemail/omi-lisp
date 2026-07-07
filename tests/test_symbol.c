/*
 * OMI Layer: test
 * Purpose: Verify the post-SP symbol lowering contract.
 * Authority: Non-authoritative declaration surface.
 *
 * Tests the SP gate and non-empty rule for symbol declarations.
 * No parser, no eval, no validation, no receipt involved.
 */

#include <stdio.h>
#include <string.h>
#include "omi_lisp.h"

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
    /* 1. symbol before SP is not a candidate. */
    OMI_LispCandidate before = omi_lisp_lower_symbol("X", 0);
    CHECK(before.is_candidate == 0, "symbol lowering before SP is not a candidate");

    /* 2. symbol after SP is a candidate. */
    OMI_LispCandidate after = omi_lisp_lower_symbol("X", 1);
    CHECK(after.is_candidate == 1, "symbol lowering after SP is a candidate");

    /* 3. root kind is symbol. */
    CHECK(after.root != NULL, "candidate has a root");
    CHECK(after.root->kind == OMI_LISP_NODE_SYMBOL, "root kind is OMI_LISP_NODE_SYMBOL");

    /* 4. root symbol equals input (backward compat null-terminated path). */
    CHECK(after.root->symbol != NULL, "symbol text present");
    CHECK(strcmp(after.root->symbol, "X") == 0, "root symbol equals input string");

    /* 4b. span set correctly on lowered symbol. */
    CHECK(after.root->span.ptr != NULL && after.root->span.len == 1,
          "lowered symbol span len == 1");
    CHECK(omi_lisp_symbol_equals(after.root, "X") != 0,
          "omi_lisp_symbol_equals matches lowered symbol");

    /* 5. empty symbol fails. */
    OMI_LispCandidate empty = omi_lisp_lower_symbol("", 1);
    CHECK(empty.is_candidate == 0, "empty symbol fails to lower");

    /* 6. NULL symbol fails. */
    OMI_LispCandidate nul = omi_lisp_lower_symbol(NULL, 1);
    CHECK(nul.is_candidate == 0, "NULL symbol fails to lower");

    /* 7-9. never accepted / validated / receipted. */
    CHECK(after.accepted == 0, "accepted == false");
    CHECK(after.validated == 0, "validated == false");
    CHECK(after.receipted == 0, "receipted == false");

    /* 10. seed lowering still passes. */
    OMI_LispCandidate seed = omi_lisp_lower_seed();
    CHECK(omi_lisp_candidate_is_seed(&seed) != 0, "seed lowering still passes");

    /* 11. pair lowering still passes (after SP). */
    const OMI_LispNode* n = omi_lisp_null();
    OMI_LispCandidate pair = omi_lisp_lower_pair(n, n, 1);
    CHECK(pair.is_candidate == 1 && pair.root->kind == OMI_LISP_NODE_PAIR,
          "pair lowering still passes");

    /* 12. omi_lisp_symbol_span creates symbol node with explicit span. */
    OMI_LispNode span_node = omi_lisp_symbol_span("hello", 5);
    CHECK(span_node.kind == OMI_LISP_NODE_SYMBOL, "span_node kind is SYMBOL");
    CHECK(span_node.span.ptr != NULL && span_node.span.len == 5,
          "span_node atom span len == 5");

    /* 13. omi_lisp_symbol_equals matches same content. */
    CHECK(omi_lisp_symbol_equals(&span_node, "hello") != 0,
          "symbol_equals matches same content");

    /* 14. omi_lisp_symbol_equals rejects different content. */
    CHECK(omi_lisp_symbol_equals(&span_node, "world") == 0,
          "symbol_equals rejects different content");

    /* 15. omi_lisp_symbol_equals rejects longer text. */
    CHECK(omi_lisp_symbol_equals(&span_node, "hello!") == 0,
          "symbol_equals rejects longer text");

    /* 16. omi_lisp_symbol_equals NULL-safe. */
    CHECK(omi_lisp_symbol_equals(NULL, "x") == 0,
          "symbol_equals NULL node returns 0");
    CHECK(omi_lisp_symbol_equals(&span_node, NULL) == 0,
          "symbol_equals NULL text returns 0");

    /* 17. omi_lisp_symbol_equals on non-SYMBOL returns 0. */
    CHECK(omi_lisp_symbol_equals(omi_lisp_null(), "x") == 0,
          "symbol_equals on NULL node returns 0");

    /* 18. omi_lisp_symbol sets span from symbol string. */
    OMI_LispNode via_symbol = omi_lisp_symbol("abc");
    CHECK(via_symbol.span.ptr != NULL && via_symbol.span.len == 3,
          "omi_lisp_symbol sets span.len == 3");

    if (failures == 0) {
        printf("\nAll symbol tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
