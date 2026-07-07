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
 * Uses an internal static node pool for the parsed tree. The tree lives
 * until the next call, so callers must convert via omi_candidate_from_lisp_into
 * before reusing the parser. For a fixture parser this is acceptable.
 */

#include "omi_parse.h"
#include "omi_lisp.h"
#include <ctype.h>
#include <string.h>

/* Maximum recursion depth for nested pairs (trivial for current grammar). */
#define OMI_PARSE_MAX_DEPTH 16

/* Internal node pool: parsed nodes live here across sub-calls within one
 * omi_lisp_parse_candidate invocation. Static so pointers remain valid
 * across the call; the pool is reset at the start of each parse. */
#define OMI_PARSE_POOL_SIZE 64

static OMI_LispNode omi_parse_pool[OMI_PARSE_POOL_SIZE];
static int omi_parse_pool_used;

static void pool_init(void)
{
    omi_parse_pool_used = 0;
}

static OMI_LispNode* pool_alloc(void)
{
    if (omi_parse_pool_used >= OMI_PARSE_POOL_SIZE) {
        return NULL;
    }
    return &omi_parse_pool[omi_parse_pool_used++];
}

typedef struct {
    const char* src;
    const char* p;
    int depth;
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
 * Allocates from the internal pool; pointer valid until next pool_init. */
static int parse_symbol(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    const char* start = s->p;
    if (!*s->p || !isalpha((unsigned char)*s->p)) {
        return 0;
    }
    while (*s->p && (isalnum((unsigned char)*s->p) || *s->p == '_' || *s->p == '-')) {
        s->p++;
    }

    OMI_LispNode* node = pool_alloc();
    if (node == NULL) {
        return 0;
    }
    node->kind = OMI_LISP_NODE_SYMBOL;
    node->car = NULL;
    node->cdr = NULL;
    node->symbol = start;
    node->span.ptr = start;
    node->span.len = (size_t)(s->p - start);
    *out_node = node;
    return 1;
}

/* Forward declaration. */
static int parse_atom(OMI_ParseState* s, const OMI_LispNode** out_node);

/* Parse a pair: (a . b).
 * Allocates from the internal pool; pointer valid until next pool_init. */
static int parse_pair(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    if (*s->p != '(') {
        return 0;
    }
    s->p++; /* consume '(' */
    skip_ws(s);

    if (s->depth >= OMI_PARSE_MAX_DEPTH) {
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

    OMI_LispNode* pair = pool_alloc();
    if (pair == NULL) {
        return 0;
    }
    pair->kind = OMI_LISP_NODE_PAIR;
    pair->car = car;
    pair->cdr = cdr;
    pair->symbol = NULL;
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

OMI_ParseResult omi_lisp_parse_candidate(
    const char* src,
    int sp_seen,
    OMI_LispCandidate* out
)
{
    if (src == NULL) {
        return OMI_PARSE_ERR_NULL_INPUT;
    }
    if (sp_seen == 0) {
        return OMI_PARSE_ERR_PRE_SP;
    }
    if (out == NULL) {
        return OMI_PARSE_ERR_UNEXPECTED;
    }

    pool_init();

    OMI_ParseState state = { .src = src, .p = src, .depth = 0 };
    const OMI_LispNode* root = NULL;

    if (!parse_atom(&state, &root)) {
        return OMI_PARSE_ERR_UNEXPECTED;
    }
    skip_ws(&state);

    if (*state.p != '\0') {
        return OMI_PARSE_ERR_TRAILING;
    }

    /* Build candidate from parsed root.
     * For the seed (standalone NULL), use omi_lisp_lower_seed which produces
     * the canonical (NULL . NULL) pair. For everything else, the parsed nodes
     * live in the static pool and are referenced directly. */
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