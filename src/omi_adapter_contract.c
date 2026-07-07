/*
 * OMI Layer: adapter
 * Purpose: Adapter shape stub — maps OMI_Candidate tree into OMI_AdapterNode
 *          tree for future omi-canvas / omi-tetragrammatron handoff.
 *          No validation, no acceptance, no receipts, no projection.
 * Authority: Non-authoritative declaration surface only.
 */

#include "omi_adapter_contract.h"

void omi_adapter_arena_init(OMI_AdapterArena* arena)
{
    arena->used = 0;
}

static OMI_AdapterNode* arena_alloc(OMI_AdapterArena* arena)
{
    if (arena->used >= OMI_ADAPTER_ARENA_MAX) {
        return NULL;
    }
    OMI_AdapterNode* node = &arena->nodes[arena->used++];
    node->kind = OMI_ADAPTER_KIND_NULL;
    node->atom_span = (OMI_LispSpan){NULL, 0};
    node->source_span = (OMI_SourceSpan){NULL, 0};
    node->car = NULL;
    node->cdr = NULL;
    node->accepted = 0;
    node->validated = 0;
    node->receipted = 0;
    return node;
}

static OMI_AdapterNode* convert_node(
    const OMI_Candidate* n,
    OMI_AdapterArena* arena
)
{
    if (n == NULL) {
        return NULL;
    }

    OMI_AdapterNode* node = arena_alloc(arena);
    if (node == NULL) {
        return NULL;
    }

    switch (n->kind) {
    case OMI_CANDIDATE_NULL:
        node->kind = OMI_ADAPTER_KIND_NULL;
        node->source_span = n->source_span;
        break;
    case OMI_CANDIDATE_SYMBOL:
        node->kind = OMI_ADAPTER_KIND_SYMBOL;
        node->atom_span = n->span;
        node->source_span = n->source_span;
        break;
    case OMI_CANDIDATE_PAIR:
        node->kind = OMI_ADAPTER_KIND_PAIR;
        node->source_span = n->source_span;
        node->car = convert_node(n->car, arena);
        if (node->car == NULL) {
            return NULL;
        }
        node->cdr = convert_node(n->cdr, arena);
        if (node->cdr == NULL) {
            return NULL;
        }
        break;
    default:
        node->kind = OMI_ADAPTER_KIND_NULL;
        break;
    }

    return node;
}

OMI_AdapterNode* omi_adapter_from_candidate(
    const OMI_Candidate* candidate,
    OMI_AdapterArena* arena
)
{
    if (arena == NULL) {
        return NULL;
    }
    omi_adapter_arena_init(arena);

    if (candidate == NULL) {
        return NULL;
    }

    return convert_node(candidate, arena);
}

int omi_adapter_is_authoritative(const OMI_AdapterNode* node)
{
    if (node == NULL) {
        return 0;
    }
    return (node->accepted == 0 && node->validated == 0 && node->receipted == 0) ? 0 : 0;
}
