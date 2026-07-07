/*
 * OMI Layer: test
 * Purpose: Verify the post-SP pair lowering contract.
 * Authority: Non-authoritative declaration surface.
 *
 * Tests the SP gate: a pair declaration only becomes a candidate after SP.
 * No parser, no eval, no validation, no receipt involved.
 */

#include <stdio.h>
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
    const OMI_LispNode* null_node = omi_lisp_null();

    /* Pair declaration before SP must not yield a candidate. */
    OMI_LispCandidate before = omi_lisp_lower_pair(null_node, null_node, 0);
    CHECK(before.is_candidate == 0, "lowering a pair before SP is not a candidate");

    /* Pair declaration after SP succeeds as a candidate. */
    OMI_LispCandidate after = omi_lisp_lower_pair(null_node, null_node, 1);
    CHECK(after.is_candidate == 1, "lowering a pair after SP is a candidate");

    /* root is a pair. */
    CHECK(after.root != NULL, "candidate has a root");
    CHECK(after.root->kind == OMI_LISP_NODE_PAIR, "root is a pair");

    /* Never accepted / validated / receipted. */
    CHECK(after.accepted == 0, "accepted == false");
    CHECK(after.validated == 0, "validated == false");
    CHECK(after.receipted == 0, "receipted == false");

    /* Pre-existing seed invariant must still hold. */
    OMI_LispCandidate seed = omi_lisp_lower_seed();
    CHECK(omi_lisp_candidate_is_seed(&seed) != 0, "seed lowering still passes");

    if (failures == 0) {
        printf("\nAll pair tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
