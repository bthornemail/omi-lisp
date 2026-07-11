/*
 * OMI Layer: adapter
 * Purpose: Spec unit data model and lazy registry for projective grammar
 *          spec units (rules, facts, closures, combinators, constructs).
 * Authority: Non-authoritative declaration surface only.
 *
 * Spec units are embedded as constant data structures — no YAML parsing.
 * Registration is lazy: units are indexed by ID on first lookup.
 * The Latin-square resolver (omi_resolver.h) builds on this registry.
 */

#ifndef OMI_SPEC_H
#define OMI_SPEC_H

#include <stddef.h>

/* ------------------------------------------------------------------ */
/*  Kinds                                                             */
/* ------------------------------------------------------------------ */

typedef enum {
    OMI_SPEC_KIND_RULE,
    OMI_SPEC_KIND_FACT,
    OMI_SPEC_KIND_CLOSURE,
    OMI_SPEC_KIND_COMBINATOR,
    OMI_SPEC_KIND_CONSTRUCT,
    OMI_SPEC_KIND_UNKNOWN
} OMI_SpecKind;

const char* omi_spec_kind_name(OMI_SpecKind k);

/* ------------------------------------------------------------------ */
/*  Content types                                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    const char* id;
    const char* sentence;
} OMI_SpecRule;

typedef struct {
    const char* id;
    const char* clause;
} OMI_SpecFact;

typedef struct {
    const char* term;
    const char* invariant;
} OMI_SpecClosure;

typedef struct {
    const char* id;
    const char** terms;
    int terms_count;
    const char* law;
    const char* role;
} OMI_SpecCombinator;

/* ------------------------------------------------------------------ */
/*  Spec unit — complete page model                                   */
/* ------------------------------------------------------------------ */

typedef struct {
    /* identity */
    const char* id;
    const char* name;
    OMI_SpecKind kind;

    /* grammar (subject-predicate-object) */
    const char* subject;
    const char* predicate;
    const char* object;

    /* content blocks */
    const OMI_SpecRule* rules;
    int rules_count;
    const OMI_SpecFact* facts;
    int facts_count;
    const char** closure_refs;
    int closure_refs_count;
    const OMI_SpecCombinator* combinators;
    int combinators_count;
    const char** required_terms;
    int required_terms_count;

    /* four axes */
    const char* cardinality;
    const char* chirality;
    const char* ordinality;
    const char* modality;

    /* boundary */
    const char** includes;
    int includes_count;
    const char** excludes;
    int excludes_count;
    int application_authority;

    /* status */
    const char* claim_status;
    const char* proof_status;
    const char* implementation_status;

    /* summary */
    const char* summary;
} OMI_SpecUnit;

/* ------------------------------------------------------------------ */
/*  Registry — lazy-loaded table of spec units                        */
/* ------------------------------------------------------------------ */

/* Maximum number of registrable spec units. */
#define OMI_SPEC_REGISTRY_MAX 32

/* A registered entry (wraps a const SpecUnit*). */
typedef struct {
    const OMI_SpecUnit* unit;
} OMI_SpecEntry;

/* The registry itself. */
typedef struct {
    OMI_SpecEntry entries[OMI_SPEC_REGISTRY_MAX];
    int count;
} OMI_SpecRegistry;

/* Initialize an empty registry. */
void omi_spec_registry_init(OMI_SpecRegistry* reg);

/* Register a spec unit. Returns 0 on success, -1 if full or duplicate. */
int omi_spec_registry_add(OMI_SpecRegistry* reg, const OMI_SpecUnit* unit);

/* Look up a spec unit by ID. Returns NULL if not found.
 * Binary search on sorted-by-ID entries. */
const OMI_SpecUnit* omi_spec_registry_find(const OMI_SpecRegistry* reg,
                                            const char* id);

/* Sort entries by ID (call after bulk add, before find). */
void omi_spec_registry_sort(OMI_SpecRegistry* reg);

/* ------------------------------------------------------------------ */
/*  Lazy field resolution — return a named field from a spec unit     */
/* ------------------------------------------------------------------ */

/* Resolve a field within a spec unit.
 * field_name: "id", "name", "kind", "predicate", "object",
 *             "cardinality", "chirality", "ordinality", "modality",
 *             "summary", "claim_status", "proof_status",
 *             "implementation_status"
 * Returns the field's string value, or NULL if field not found. */
const char* omi_spec_resolve_field(const OMI_SpecUnit* unit,
                                   const char* field_name);

/* Resolve an indexed sub-field.
 * field_name: "rules[i]", "facts[i]", "closure_refs[i]",
 *             "combinators[i]", "required_terms[i]"
 * Returns the string value, or NULL if out of range or not found. */
const char* omi_spec_resolve_indexed(const OMI_SpecUnit* unit,
                                     const char* field_name,
                                     int index);

#endif /* OMI_SPEC_H */
