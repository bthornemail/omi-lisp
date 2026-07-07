/*
 * OMI Layer: adapter
 * Purpose: Tiny fixture parser for OMI-Lisp declarations.
 * Authority: Non-authoritative declaration surface only.
 *
 * Parses only the current fixture grammar:
 *   NULL
 *   symbol
 *   (a . b)
 *   (NULL . NULL)
 *
 * Parsing creates candidates only.
 * Parsing does not evaluate.
 * Parsing does not validate.
 * Parsing does not accept.
 * Parsing does not receipt.
 * No lists, no quote, no numbers, no comments.
 */

#ifndef OMI_PARSE_H
#define OMI_PARSE_H

#include <stddef.h>
#include "omi_lisp.h"

typedef enum {
    OMI_PARSE_OK = 0,
    OMI_PARSE_ERR_NULL_INPUT,
    OMI_PARSE_ERR_PRE_SP,
    OMI_PARSE_ERR_UNEXPECTED,
    OMI_PARSE_ERR_TRAILING,
    OMI_PARSE_ERR_DEPTH,
    OMI_PARSE_ERR_ARENA_FULL
} OMI_ParseResult;

#define OMI_PARSE_ARENA_MAX 64

typedef struct {
    OMI_LispNode nodes[OMI_PARSE_ARENA_MAX];
    int used;
} OMI_ParseArena;

void omi_parse_arena_init(OMI_ParseArena* arena);

/* Parses a single OMI-Lisp declaration into an OMI_LispCandidate using a
 * caller-owned arena. All parsed OMI_LispNode values live in the arena.
 * The caller may reuse the arena (discarding prior results) or use separate
 * arenas for concurrent parses.
 *
 * src: null-terminated input string.
 * sp_seen: must be non-zero (SP gate). If 0, returns OMI_PARSE_ERR_PRE_SP.
 * arena: caller-owned arena; nodes remain valid until arena is reused.
 * out: on success, filled with the candidate (never accepted/validated/receipted).
 *
 * Returns OMI_PARSE_OK on success, or an error code. */
OMI_ParseResult omi_lisp_parse_candidate_into(
    const char* src,
    int sp_seen,
    OMI_ParseArena* arena,
    OMI_LispCandidate* out
);

/* Convenience wrapper using an internal static arena. Result is only valid
 * until the next call. Tests should prefer the _into function. */
OMI_ParseResult omi_lisp_parse_candidate(
    const char* src,
    int sp_seen,
    OMI_LispCandidate* out
);

#endif /* OMI_PARSE_H */