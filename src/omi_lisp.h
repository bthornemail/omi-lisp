/*
 * OMI Layer: adapter
 * Purpose: Lower OMI-Lisp declarations into typed construction candidates.
 * Authority: Non-authoritative declaration surface only.
 *
 * This file defines the minimal node and candidate representation for the
 * OMI-Lisp lowering contract. It deliberately implements NO parser, NO
 * evaluation, NO validation, and NO receipt creation.
 *
 * The only construct realized here is the canonical seed:
 *     (NULL . NULL)
 * lowered into a typed construction candidate that is explicitly NOT accepted,
 * NOT validated, and NOT receipted, because OMI-Lisp cannot accept state.
 */

#ifndef OMI_LISP_H
#define OMI_LISP_H

#include <stddef.h>

typedef enum {
    OMI_LISP_NODE_NULL,
    OMI_LISP_NODE_PAIR,
    OMI_LISP_NODE_SYMBOL
} OMI_LispNodeKind;

typedef struct OMI_LispNode OMI_LispNode;

struct OMI_LispNode {
    OMI_LispNodeKind kind;
    const OMI_LispNode* car;
    const OMI_LispNode* cdr;
    const char* symbol;
};

typedef struct {
    const OMI_LispNode* root;
    int is_candidate;
    int accepted;
    int validated;
    int receipted;
} OMI_LispCandidate;

/* Returns the canonical NULL node. Not a pair, not a symbol. */
const OMI_LispNode* omi_lisp_null(void);

/* Builds a pair candidate node from car/cdr. Pure structural constructor. */
OMI_LispNode omi_lisp_pair(const OMI_LispNode* car, const OMI_LispNode* cdr);

/* Lowers the canonical seed (NULL . NULL) into a typed construction candidate.
 * The candidate is marked accepted = false, validated = false, receipted = false
 * because OMI-Lisp does not accept state. */
OMI_LispCandidate omi_lisp_lower_seed(void);

/* Lowers an explicit pair declaration into a typed construction candidate.
 * sp_seen must be non-zero: a pair declaration is only readable after the SP
 * boundary is reached. If sp_seen == 0, a non-candidate is returned
 * (is_candidate = 0) to prove the SP gate. The candidate is always marked
 * accepted = false, validated = false, receipted = false because OMI-Lisp
 * does not accept state. */
OMI_LispCandidate omi_lisp_lower_pair(
    const OMI_LispNode* car,
    const OMI_LispNode* cdr,
    int sp_seen
);

/* Builds a symbol node from a non-NULL, non-empty string. Pure structural
 * constructor. Symbol identity is the text; OMI-Lisp does not validate it. */
OMI_LispNode omi_lisp_symbol(const char* symbol);

/* Lowers a symbol declaration into a typed construction candidate.
 * sp_seen must be non-zero. A NULL or empty symbol returns a non-candidate.
 * The candidate is always marked accepted = false, validated = false,
 * receipted = false because OMI-Lisp does not accept state. */
OMI_LispCandidate omi_lisp_lower_symbol(
    const char* symbol,
    int sp_seen
);

/* Returns non-zero iff the candidate root is exactly the seed (NULL . NULL). */
int omi_lisp_candidate_is_seed(const OMI_LispCandidate* c);

#endif /* OMI_LISP_H */
