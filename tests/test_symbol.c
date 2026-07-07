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

    /* 4. root symbol equals input. */
    CHECK(after.root->symbol != NULL, "symbol text present");
    CHECK(strcmp(after.root->symbol, "X") == 0, "root symbol equals input string");

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

    if (failures == 0) {
        printf("\nAll symbol tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
