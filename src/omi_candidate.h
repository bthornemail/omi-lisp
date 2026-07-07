/*
 * OMI Layer: adapter
 * Purpose: Neutral typed construction candidate handed from OMI-Lisp lowering
 *          downstream to omi-canvas / omi-tetragrammatron.
 * Authority: Non-authoritative declaration surface only.
 *
 * This file defines the neutral handoff shape. It does NOT implement
 * omi-canvas. It does NOT import omi-canvas. It does NOT implement
 * tetragrammatron validation. It does NOT create receipts. It does NOT parse
 * text. It only maps an OMI-Lisp candidate into a neutral typed construction
 * candidate that downstream authority may later judge.
 */

#ifndef OMI_CANDIDATE_H
#define OMI_CANDIDATE_H

#include <stddef.h>
#include "omi_lisp.h"

typedef enum {
    OMI_CANDIDATE_NULL,
    OMI_CANDIDATE_SYMBOL,
    OMI_CANDIDATE_PAIR
} OMI_CandidateKind;

typedef struct OMI_Candidate OMI_Candidate;

struct OMI_Candidate {
    OMI_CandidateKind kind;
    const char* symbol;
    const OMI_Candidate* car;
    const OMI_Candidate* cdr;
    int accepted;
    int validated;
    int receipted;
};

/* Fixed-size arena for tree-safe candidate conversion. The candidate layer
 * owns its own OMI_Candidate tree rather than aliasing OMI_LispNode*.
 * Depth is bounded by OMI_CANDIDATE_ARENA_MAX. */
#define OMI_CANDIDATE_ARENA_MAX 64

typedef struct {
    OMI_Candidate nodes[OMI_CANDIDATE_ARENA_MAX];
    int used;
} OMI_CandidateArena;

/* Initializes an arena to empty. */
void omi_candidate_arena_init(OMI_CandidateArena* arena);

/* Converts an OMI-Lisp candidate into a real OMI_Candidate tree allocated from
 * the arena. Every node is non-authoritative (accepted/validated/receipted=0).
 * NULL node  -> OMI_CANDIDATE_NULL
 * SYMBOL node -> OMI_CANDIDATE_SYMBOL (car/cdr NULL)
 * PAIR node   -> OMI_CANDIDATE_PAIR (car/cdr are real arena nodes)
 * Invalid / non-candidate input, or arena exhaustion / depth overflow, returns
 * NULL (caller treats as safe non-authoritative failure). */
OMI_Candidate* omi_candidate_from_lisp_into(
    const OMI_LispCandidate* c,
    OMI_CandidateArena* arena
);

/* Convenience wrapper: converts into an internal static arena. Allocated from a
 * single shared arena, so the result is only valid until the next call. Useful
 * for single-shot checks; tests should prefer the explicit arena function. */
OMI_Candidate* omi_candidate_from_lisp(const OMI_LispCandidate* c);

/* Returns non-zero iff the candidate carries authority. Currently always 0,
 * because OMI-Lisp never accepts, validates, or receipts. */
int omi_candidate_is_authoritative(const OMI_Candidate* c);

#endif /* OMI_CANDIDATE_H */
