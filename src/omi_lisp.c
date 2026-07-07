/*
 * OMI Layer: adapter
 * Purpose: Lower OMI-Lisp declarations into typed construction candidates.
 * Authority: Non-authoritative declaration surface only.
 *
 * Minimal lowering stub. Implements only the canonical seed (NULL . NULL).
 * No parser, no evaluation, no validation, no receipt.
 */

#include <string.h>
#include "omi_lisp.h"

static const OMI_LispNode OMI_LISP_NULL_NODE = {
    .kind = OMI_LISP_NODE_NULL,
    .car = NULL,
    .cdr = NULL,
    .symbol = NULL,
    .span = {NULL, 0},
    .source_span = {NULL, 0}
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
    node.span = (OMI_LispSpan){NULL, 0};
    node.source_span = (OMI_SourceSpan){NULL, 0};
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
    seed_pair.span = (OMI_LispSpan){NULL, 0};
    seed_pair.source_span = (OMI_SourceSpan){NULL, 0};

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

OMI_LispCandidate omi_lisp_lower_pair(
    const OMI_LispNode* car,
    const OMI_LispNode* cdr,
    int sp_seen
)
{
    OMI_LispCandidate c;
    c.root = NULL;
    c.is_candidate = 0;
    c.accepted = 0;
    c.validated = 0;
    c.receipted = 0;

    /* Before SP no readable pair candidate may exist. */
    if (sp_seen == 0) {
        return c;
    }

    /* After SP the pair becomes a candidate. Still not accepted/validated/receipted. */
    static OMI_LispNode pair_node;
    pair_node.kind = OMI_LISP_NODE_PAIR;
    pair_node.car = car;
    pair_node.cdr = cdr;
    pair_node.symbol = NULL;
    pair_node.span = (OMI_LispSpan){NULL, 0};
    pair_node.source_span = (OMI_SourceSpan){NULL, 0};

    c.root = &pair_node;
    c.is_candidate = 1;
    c.accepted = 0;
    c.validated = 0;
    c.receipted = 0;
    return c;
}

OMI_LispNode omi_lisp_symbol(const char* symbol)
{
    OMI_LispNode node;
    node.kind = OMI_LISP_NODE_SYMBOL;
    node.car = NULL;
    node.cdr = NULL;
    node.symbol = symbol;
    node.span.ptr = symbol;
    node.span.len = (symbol != NULL) ? strlen(symbol) : 0;
    node.source_span = (OMI_SourceSpan){NULL, 0};
    return node;
}

OMI_LispNode omi_lisp_symbol_span(const char* ptr, size_t len)
{
    OMI_LispNode node;
    node.kind = OMI_LISP_NODE_SYMBOL;
    node.car = NULL;
    node.cdr = NULL;
    node.symbol = ptr;
    node.span.ptr = ptr;
    node.span.len = len;
    node.source_span = (OMI_SourceSpan){NULL, 0};
    return node;
}

int omi_lisp_symbol_equals(const OMI_LispNode* node, const char* text)
{
    if (node == NULL || text == NULL) return 0;
    if (node->kind != OMI_LISP_NODE_SYMBOL) return 0;
    if (node->span.ptr == NULL) return 0;
    size_t n = strlen(text);
    if (node->span.len != n) return 0;
    return memcmp(node->span.ptr, text, n) == 0;
}

OMI_LispCandidate omi_lisp_lower_symbol(
    const char* symbol,
    int sp_seen
)
{
    OMI_LispCandidate c;
    c.root = NULL;
    c.is_candidate = 0;
    c.accepted = 0;
    c.validated = 0;
    c.receipted = 0;

    /* Before SP no readable symbol candidate may exist. */
    if (sp_seen == 0) {
        return c;
    }

    /* NULL or empty symbol is not a valid candidate. */
    if (symbol == NULL || symbol[0] == '\0') {
        return c;
    }

    /* After SP the symbol becomes a candidate. Still not accepted/validated/receipted. */
    static OMI_LispNode symbol_node;
    symbol_node.kind = OMI_LISP_NODE_SYMBOL;
    symbol_node.car = NULL;
    symbol_node.cdr = NULL;
    symbol_node.symbol = symbol;
    symbol_node.span.ptr = symbol;
    symbol_node.span.len = strlen(symbol);
    symbol_node.source_span = (OMI_SourceSpan){NULL, 0};

    c.root = &symbol_node;
    c.is_candidate = 1;
    c.accepted = 0;
    c.validated = 0;
    c.receipted = 0;
    return c;
}
