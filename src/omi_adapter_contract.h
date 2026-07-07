/*
 * OMI Layer: adapter
 * Purpose: Adapter shape stub for future omi-canvas / omi-tetragrammatron
 *          handoff. No omi-canvas import. No tetragrammatron import.
 *          No validation, no acceptance, no receipts, no projection.
 * Authority: Non-authoritative declaration surface only.
 *
 * This is the shape contract a downstream adapter must implement.
 * It does NOT import omi-canvas or omi-tetragrammatron.
 * It does NOT validate, accept, receipt, or project.
 *
 * Handoff chain:
 *   OMI_LispNode -> OMI_Candidate -> OMI_AdapterNode -> future authority
 */

#ifndef OMI_ADAPTER_CONTRACT_H
#define OMI_ADAPTER_CONTRACT_H

#include <stddef.h>
#include "omi_lisp.h"
#include "omi_candidate.h"

typedef enum {
    OMI_ADAPTER_KIND_NULL,
    OMI_ADAPTER_KIND_SYMBOL,
    OMI_ADAPTER_KIND_PAIR
} OMI_AdapterKind;

typedef struct OMI_AdapterNode OMI_AdapterNode;

struct OMI_AdapterNode {
    OMI_AdapterKind kind;
    OMI_LispSpan atom_span;
    OMI_SourceSpan source_span;
    const OMI_AdapterNode* car;
    const OMI_AdapterNode* cdr;
    int accepted;
    int validated;
    int receipted;
};

#define OMI_ADAPTER_ARENA_MAX 64

typedef struct {
    OMI_AdapterNode nodes[OMI_ADAPTER_ARENA_MAX];
    int used;
} OMI_AdapterArena;

void omi_adapter_arena_init(OMI_AdapterArena* arena);

OMI_AdapterNode* omi_adapter_from_candidate(
    const OMI_Candidate* candidate,
    OMI_AdapterArena* arena
);

int omi_adapter_is_authoritative(const OMI_AdapterNode* node);

#endif /* OMI_ADAPTER_CONTRACT_H */
