/*
 * OMI Layer: test
 * Purpose: Verify the OMI-Lisp seed lowering contract.
 * Authority: Non-authoritative declaration surface.
 *
 * Tests only the canonical seed (NULL . NULL) lowering. No parser, no eval,
 * no validation, no receipt involved.
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
    OMI_LispCandidate c = omi_lisp_lower_seed();

    /* 1. lower_seed returns a candidate. */
    CHECK(c.is_candidate == 1, "lower_seed returns a candidate");

    /* 2. root is a pair. */
    CHECK(c.root != NULL, "candidate has a root");
    CHECK(c.root->kind == OMI_LISP_NODE_PAIR, "root is a pair");

    /* 3. car is NULL node. */
    CHECK(c.root->car != NULL, "car present");
    CHECK(c.root->car->kind == OMI_LISP_NODE_NULL, "car is NULL node");

    /* 4. cdr is NULL node. */
    CHECK(c.root->cdr != NULL, "cdr present");
    CHECK(c.root->cdr->kind == OMI_LISP_NODE_NULL, "cdr is NULL node");

    /* 5. accepted == false. */
    CHECK(c.accepted == 0, "accepted == false");

    /* 6. validated == false. */
    CHECK(c.validated == 0, "validated == false");

    /* 7. receipted == false. */
    CHECK(c.receipted == 0, "receipted == false");

    /* seed identity check */
    CHECK(omi_lisp_candidate_is_seed(&c) != 0, "candidate is the seed (NULL . NULL)");

    if (failures == 0) {
        printf("\nAll seed tests passed.\n");
        return 0;
    }
    printf("\n%d test(s) failed.\n", failures);
    return 1;
}
