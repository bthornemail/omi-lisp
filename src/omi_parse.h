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
    OMI_PARSE_ERR_DEPTH
} OMI_ParseResult;

/* Parses a single OMI-Lisp declaration into an OMI_LispCandidate.
 * src: null-terminated input string.
 * sp_seen: must be non-zero (SP gate). If 0, returns OMI_PARSE_ERR_PRE_SP.
 * out: on success, filled with the candidate (never accepted/validated/receipted).
 * Returns OMI_PARSE_OK on success, or an error code. */
OMI_ParseResult omi_lisp_parse_candidate(
    const char* src,
    int sp_seen,
    OMI_LispCandidate* out
);

#endif /* OMI_PARSE_H */