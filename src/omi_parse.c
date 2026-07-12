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
 */

#include "omi_parse.h"
#include "omi_lisp.h"
#include <ctype.h>
#include <string.h>

/* Maximum recursion depth for nested pairs. */
#define OMI_PARSE_MAX_DEPTH 16

/* Small static arena for parse nodes, avoiding shared-static aliasing. */
#define OMI_PARSE_NODE_MAX 32

static OMI_LispNode OMI_PARSE_NODES[OMI_PARSE_NODE_MAX];
static int OMI_PARSE_NODE_USED;

static OMI_LispNode* parse_alloc(void)
{
    if (OMI_PARSE_NODE_USED >= OMI_PARSE_NODE_MAX) {
        return NULL;
    }
    return &OMI_PARSE_NODES[OMI_PARSE_NODE_USED++];
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
 * out_node is set to an arena-allocated symbol node if successful. */
static int parse_symbol(OMI_ParseState* s, const OMI_LispNode** out_node)
{
    const char* start = s->p;
    if (!*s->p || !isalpha((unsigned char)*s->p)) {
        return 0;
    }
    while (*s->p && (isalnum((unsigned char)*s->p) || *s->p == '_' || *s->p == '-')) {
        s->p++;
    }
    OMI_LispNode* node = parse_alloc();
    if (node == NULL) {
        return 0;
    }
    *node = omi_lisp_symbol(start);
    *out_node = node;
    return 1;
}

/* Forward declaration. */
static int parse_atom(OMI_ParseState* s, const OMI_LispNode** out_node);

/* Parse a pair: (a . b) */
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

    OMI_LispNode* pair_node = parse_alloc();
    if (pair_node == NULL) {
        return 0;
    }
    *pair_node = omi_lisp_pair(car, cdr);
    *out_node = pair_node;
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

    OMI_ParseState state = { .src = src, .p = src, .depth = 0 };
    OMI_PARSE_NODE_USED = 0;
    const OMI_LispNode* root = NULL;

    if (!parse_atom(&state, &root)) {
        return OMI_PARSE_ERR_UNEXPECTED;
    }
    skip_ws(&state);

    if (*state.p != '\0') {
        return OMI_PARSE_ERR_TRAILING;
    }

    /* Build candidate from parsed root. */
    if (root->kind == OMI_LISP_NODE_PAIR) {
        *out = omi_lisp_lower_pair(root->car, root->cdr, 1);
    } else if (root->kind == OMI_LISP_NODE_SYMBOL) {
        *out = omi_lisp_lower_symbol(root->symbol, 1);
    } else { /* NULL */
        *out = omi_lisp_lower_seed();
    }

    return OMI_PARSE_OK;
}