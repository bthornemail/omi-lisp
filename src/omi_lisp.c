/*
 * OMI Layer: adapter
 * Purpose: Lower OMI-Lisp declarations into typed construction candidates.
 * Authority: Non-authoritative declaration surface only.
 *
 * Minimal lowering stub. Implements only the canonical seed (NULL . NULL).
 * No parser, no evaluation, no validation, no receipt.
 */

#include "omi_lisp.h"

static const OMI_LispNode OMI_LISP_NULL_NODE = {
    .kind = OMI_LISP_NODE_NULL,
    .car = NULL,
    .cdr = NULL,
    .symbol = NULL
};

const OMI_LispNode* omi_lisp_null(void)
{
    return &OMI_LISP_NULL_NODE;
}

OMI_LispNode omi_lisp_pair(const OMI_LispNode* car, const OMI_LispNode* cdr)
{
    OMI_LispNode node;
    node.kind = OMI_LISP_NODE_PAIR;
    node.car = car;
    node.cdr = cdr;
    node.symbol = NULL;
    return node;
}

OMI_LispCandidate omi_lisp_lower_seed(void)
{
    OMI_LispCandidate c;
    /* The seed is the null pair (NULL . NULL). */
    static OMI_LispNode seed_pair;
    seed_pair.kind = OMI_LISP_NODE_PAIR;
    seed_pair.car = omi_lisp_null();
    seed_pair.cdr = omi_lisp_null();
    seed_pair.symbol = NULL;

    c.root = &seed_pair;
    c.is_candidate = 1;
    c.accepted = 0;
    c.validated = 0;
    c.receipted = 0;
    return c;
}

int omi_lisp_candidate_is_seed(const OMI_LispCandidate* c)
{
    if (c == NULL || c->root == NULL) {
        return 0;
    }
    if (c->root->kind != OMI_LISP_NODE_PAIR) {
        return 0;
    }
    const OMI_LispNode* car = c->root->car;
    const OMI_LispNode* cdr = c->root->cdr;
    if (car == NULL || cdr == NULL) {
        return 0;
    }
    return car->kind == OMI_LISP_NODE_NULL && cdr->kind == OMI_LISP_NODE_NULL;
}
