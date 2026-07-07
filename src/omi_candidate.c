/*
 * OMI Layer: adapter
 * Purpose: Neutral typed construction candidate handed from OMI-Lisp lowering
 *          downstream to omi-canvas / omi-tetragrammatron.
 * Authority: Non-authoritative declaration surface only.
 *
 * Maps OMI-Lisp candidates into neutral typed construction candidates.
 * No parser, no eval, no validation, no receipt.
 */

#include "omi_candidate.h"

void omi_candidate_arena_init(OMI_CandidateArena* arena)
{
    arena->used = 0;
}

/* Allocates one node from the arena, or NULL on exhaustion. */
static OMI_Candidate* arena_alloc(OMI_CandidateArena* arena)
{
    if (arena->used >= OMI_CANDIDATE_ARENA_MAX) {
        return NULL;
    }
    OMI_Candidate* node = &arena->nodes[arena->used++];
    node->kind = OMI_CANDIDATE_NULL;
    node->symbol = NULL;
    node->span = (OMI_LispSpan){NULL, 0};
    node->car = NULL;
    node->cdr = NULL;
    node->accepted = 0;
    node->validated = 0;
    node->receipted = 0;
    return node;
}

/* Recursive converter. Returns NULL on failure (invalid input or arena full). */
static OMI_Candidate* convert_node(const OMI_LispNode* n, OMI_CandidateArena* arena)
{
    if (n == NULL) {
        return NULL;
    }

    OMI_Candidate* node = arena_alloc(arena);
    if (node == NULL) {
        return NULL;
    }

    switch (n->kind) {
    case OMI_LISP_NODE_NULL:
        node->kind = OMI_CANDIDATE_NULL;
        break;
    case OMI_LISP_NODE_SYMBOL:
        node->kind = OMI_CANDIDATE_SYMBOL;
        node->symbol = n->symbol;
        node->span = n->span;
        break;
    case OMI_LISP_NODE_PAIR: {
        node->kind = OMI_CANDIDATE_PAIR;
        node->car = convert_node(n->car, arena);
        if (node->car == NULL) {
            return NULL;
        }
        node->cdr = convert_node(n->cdr, arena);
        if (node->cdr == NULL) {
            return NULL;
        }
        break;
    }
    default:
        node->kind = OMI_CANDIDATE_NULL;
        break;
    }

    return node;
}

OMI_Candidate* omi_candidate_from_lisp_into(
    const OMI_LispCandidate* c,
    OMI_CandidateArena* arena
)
{
    if (arena == NULL) {
        return NULL;
    }
    omi_candidate_arena_init(arena);

    /* Non-candidate / invalid input -> safe non-authoritative failure (NULL). */
    if (c == NULL || c->is_candidate == 0 || c->root == NULL) {
        return NULL;
    }

    return convert_node(c->root, arena);
}

/* Single shared arena for the convenience wrapper. */
static OMI_CandidateArena OMI_CANDIDATE_STATIC_ARENA;

OMI_Candidate* omi_candidate_from_lisp(const OMI_LispCandidate* c)
{
    return omi_candidate_from_lisp_into(c, &OMI_CANDIDATE_STATIC_ARENA);
}

int omi_candidate_is_authoritative(const OMI_Candidate* c)
{
    if (c == NULL) {
        return 0;
    }
    /* OMI-Lisp never accepts, validates, or receipts. */
    return (c->accepted == 0 && c->validated == 0 && c->receipted == 0) ? 0 : 0;
}
