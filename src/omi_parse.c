/*
 * OMI Layer: adapter
 * Purpose: Tiny fixture parser for OMI-Lisp declarations.
 * Authority: Non-authoritative declaration surface only.
 *
 * Parsing creates candidates only.
 * Parsing does not evaluate.
 * Parsing does not validate.
 * Parsing does not accept.
 * Parsing does not receipt.
 *
 * Uses a caller-owned OMI_ParseArena for all parsed nodes. No global
 * static pool. The arena is reset at the start of each parse call, so
 * separate calls with separate arenas produce independent parse trees.
 */

#include "omi_parse.h"
#include "omi_lisp.h"
#include <ctype.h>
#include <string.h>

/* Maximum recursion depth for nested pairs (trivial for current grammar). */
#define OMI_PARSE_MAX_DEPTH 16

void omi_parse_arena_init(OMI_ParseArena* arena)
{
    arena->used = 0;
}

static OMI_LispNode* pool_alloc(OMI_ParseArena* arena)
{
    if (arena->used >= OMI_PARSE_ARENA_MAX) {
        return NULL;
    }
    return &arena->nodes[arena->used++];
}

typedef struct {
    const char* src;
    const char* p;
    int depth;
    int depth_overflow;
    OMI_ParseArena* arena;
} OMI_ParseState;

/* Skip whitespace. */
static void skip_ws(OMI_ParseState* s)
{
    while (*s->p && isspace((unsigned char)*s->p)) {
        s->p++;
    }
}

/* Check if current position matches a keyword and advance. */
static int match_keyword(OMI_ParseState* s, const char* kw)
{
    size_t len = strlen(kw);
    if (strncmp(s->p, kw, len) == 0) {
        char c = s->p[len];
        if (c == '\0' || isspace((unsigned char)c) || c == ')' || c == '.') {
            s->p += len;
            return 1;
        }
    }
    return 0;
}

/* Parse a symbol (identifier). Returns 1 on success, 0 on failure.
 * Allocates from the caller-owned arena; pointer valid until arena reuse. */
static int parse_symbol(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    const char* start = s->p;
    if (!*s->p || !isalpha((unsigned char)*s->p)) {
        return 0;
    }
    while (*s->p && (isalnum((unsigned char)*s->p) || *s->p == '_' || *s->p == '-')) {
        s->p++;
    }

    OMI_LispNode* node = pool_alloc(s->arena);
    if (node == NULL) {
        return 0;
    }
    node->kind = OMI_LISP_NODE_SYMBOL;
    node->car = NULL;
    node->cdr = NULL;
    node->symbol = start;
    node->span.ptr = start;
    node->span.len = (size_t)(s->p - start);
    node->source_span.ptr = start;
    node->source_span.len = (size_t)(s->p - start);
    *out_node = node;
    return 1;
}

/* Forward declaration. */
static int parse_atom(OMI_ParseState* s, const OMI_LispNode** out_node);

/* Parse a pair: (a . b).
 * Allocates from the caller-owned arena; pointer valid until arena reuse. */
static int parse_pair(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    const char* open = s->p;
    if (*s->p != '(') {
        return 0;
    }
    s->p++; /* consume '(' */
    skip_ws(s);

    if (s->depth >= OMI_PARSE_MAX_DEPTH) {
        s->depth_overflow = 1;
        return 0;
    }
    s->depth++;

    const OMI_LispNode* car = NULL;
    const OMI_LispNode* cdr = NULL;

    if (!parse_atom(s, &car)) {
        s->depth--;
        return 0;
    }
    skip_ws(s);

    if (*s->p != '.') {
        s->depth--;
        return 0;
    }
    s->p++; /* consume '.' */
    skip_ws(s);

    if (!parse_atom(s, &cdr)) {
        s->depth--;
        return 0;
    }
    skip_ws(s);

    if (*s->p != ')') {
        s->depth--;
        return 0;
    }
    s->p++; /* consume ')' */
    s->depth--;

    OMI_LispNode* pair = pool_alloc(s->arena);
    if (pair == NULL) {
        return 0;
    }
    pair->kind = OMI_LISP_NODE_PAIR;
    pair->car = car;
    pair->cdr = cdr;
    pair->symbol = NULL;
    pair->span = (OMI_LispSpan){NULL, 0};
    pair->source_span.ptr = open;
    pair->source_span.len = (size_t)(s->p - open);
    *out_node = pair;
    return 1;
}

/* Parse an atom: NULL, symbol, or pair. */
static int parse_atom(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    skip_ws(s);

    if (match_keyword(s, "NULL")) {
        *out_node = omi_lisp_null();
        return 1;
    }

    if (*s->p == '(') {
        return parse_pair(s, out_node);
    }

    return parse_symbol(s, out_node);
}

OMI_ParseResult omi_lisp_parse_candidate_into(
    const char* src,
    int sp_seen,
    OMI_ParseArena* arena,
    OMI_LispCandidate* out
)
{
    if (src == NULL) {
        return OMI_PARSE_ERR_NULL_INPUT;
    }
    if (sp_seen == 0) {
        return OMI_PARSE_ERR_PRE_SP;
    }
    if (out == NULL || arena == NULL) {
        return OMI_PARSE_ERR_UNEXPECTED;
    }

    omi_parse_arena_init(arena);

    OMI_ParseState state = { .src = src, .p = src, .depth = 0, .depth_overflow = 0, .arena = arena };
    const OMI_LispNode* root = NULL;

    if (!parse_atom(&state, &root)) {
        if (state.depth_overflow) {
            return OMI_PARSE_ERR_DEPTH;
        }
        if (arena->used >= OMI_PARSE_ARENA_MAX) {
            return OMI_PARSE_ERR_ARENA_FULL;
        }
        return OMI_PARSE_ERR_UNEXPECTED;
    }
    skip_ws(&state);

    if (*state.p != '\0') {
        return OMI_PARSE_ERR_TRAILING;
    }

    /* Build candidate from parsed root.
     * For the seed (standalone NULL), use omi_lisp_lower_seed which produces
     * the canonical (NULL . NULL) pair. For everything else, the parsed nodes
     * live in the caller-owned arena and are referenced directly. */
    out->accepted = 0;
    out->validated = 0;
    out->receipted = 0;

    if (root->kind == OMI_LISP_NODE_NULL) {
        /* Standalone "NULL" becomes the seed (NULL . NULL). */
        *out = omi_lisp_lower_seed();
    } else {
        out->is_candidate = 1;
        out->root = root;
    }

    return OMI_PARSE_OK;
}

/* Static arena for the convenience wrapper. */
static OMI_ParseArena OMI_PARSE_STATIC_ARENA;

OMI_ParseResult omi_lisp_parse_candidate(
    const char* src,
    int sp_seen,
    OMI_LispCandidate* out
)
{
    return omi_lisp_parse_candidate_into(src, sp_seen, &OMI_PARSE_STATIC_ARENA, out);
}
