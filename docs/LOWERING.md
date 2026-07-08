Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md, AGENTS.md, SPEC.md

# LOWERING.md
# OMI-Lisp Lowering Contract

## Purpose

Lowering translates OMI-Lisp declarations into **typed construction
candidates**. It is the boundary between the declaration surface and the
typed construction engine (`omi-canvas`).

Lowering does not accept, validate, or attest. It only produces candidates.

## Pipeline

```text
OMI-Lisp declaration (post-SP)
    ↓
token / pair reading
    ↓
typed declaration
    ↓
construction candidate
    ↓
[handoff to omi-canvas / omi-tetragrammatron]
    ↓
validation
    ↓
receipt
    ↓
projection
```

The lowering stage ends at `construction candidate`. Everything after that
arrow is downstream and out of OMI-Lisp authority.

## Seed Invariant

The seed lowers trivially and requires no upstream acceptance:

```lisp
(NULL . NULL)  →  typed construction candidate: seed
```

All other declarations lower into non-seed candidates that require downstream
validation.

## Dot Notation Rule In Lowering

The dot is not available before `SP`. Lowering MUST reject any attempt to read
dot notation prior to the SP boundary.

```text
Before SP: control stream, no dot reading
At SP:     readable boundary induced
After SP:  pairs with dot notation may be lowered
```

## What Lowering Must Not Do

```text
Lowering MUST NOT accept state.
Lowering MUST NOT validate state.
Lowering MUST NOT create receipts.
Lowering MUST NOT derive identity from hashes.
Lowering MUST NOT treat projections as authority.
Lowering MUST NOT restore legacy _archive code as authority.
```

The output of lowering is always a candidate, never accepted state.

## Handoff Contract

The construction candidate handed downstream carries:

- the candidate relation (car, cdr)
- the SP-boundary provenance (declared after SP)
- no acceptance flag
- no validation result
- no receipt

Downstream layers (`omi-tetragrammatron` validation, `omi-canvas` typed
construction, receipt authority) consume the candidate and decide acceptance.

## Scope Of This Document

This LOWERING.md is doc-first. No parser or lowering executable is implemented
here. The first lowering artifact will be a parser/lowering stub under `src/`,
produced only after this scaffold is accepted. See `TEST_PLAN.md`.
