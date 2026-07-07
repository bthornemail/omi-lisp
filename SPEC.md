Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md, AGENTS.md

# SPEC.md
# OMI-Lisp Declaration Surface Specification

## Layer and Authority

OMI-Lisp is an **adapter-layer declaration surface**. It is not protocol authority.
It is one of several allowed declaration surfaces (Markdown, JSON Canvas,
CanvasEDSL, OMI-Lisp, `omi---imo`, future DOM/GPIO/LoRa projections).

```text
Declaration surfaces propose.
Typed construction normalizes.
Validation determines.
Attestation witnesses.
Projection displays.
```

OMI-Lisp participates only in the first step.

## Required Doctrine

OMI-Lisp MUST be understood and implemented under these constraints:

```text
1. OMI-Lisp is not authority.
2. OMI-Lisp does not accept state.
3. OMI-Lisp does not validate state.
4. OMI-Lisp does not create receipts.
5. Dot notation is earned after SP.
6. The seed is (NULL . NULL).
7. Pair declarations are candidate relations only.
8. Lowering target is typed construction candidate, not accepted state.
9. Validation and receipt happen downstream.
```

## Readability Boundary

OMI-Lisp is not readable at byte zero. The pre-language control stream must
reach `SP` (space) before OMI-Lisp declarations become expressible.

```text
Before SP:
    control stream only

At SP:
    readable boundary induced

After SP:
    OMI-Lisp may express declarations

After validation + receipt:
    state may be accepted downstream only
```

Dot notation is **not primitive** at byte zero. It is **earned after SP**.

## Seed

The universal seed relation is the null pair:

```lisp
(NULL . NULL)
```

This is the only relation that requires no upstream acceptance. Every other
OMI-Lisp declaration is a candidate that lowers into a typed construction
candidate and awaits downstream validation.

## Declaration Shape (sketch, not implementation)

A declaration is a pair describing a candidate relation. The left and right
positions are candidate slots, not accepted state.

```text
(<candidate-car> . <candidate-cdr>)
```

Constraints:

- A pair is a **candidate relation only**.
- A pair does not imply acceptance.
- A pair does not imply validation.
- A pair does not create a receipt.
- Pair formation is permitted only after the SP boundary is reached.

## What OMI-Lisp Is Not

OMI-Lisp is not:

- the validation engine
- the receipt authority
- the construction engine
- the carrier identity source
- a competing protocol

OMI-Lisp lowers into `omi-canvas` typed construction candidates and lets
`omi-tetragrammatron` judge validity downstream.

## Forbidden In This Surface

```text
No hash identity.
No digest identity.
No JSON blob as authority.
No projection authority.
No carrier authority.
```

Hashes, digests, checksums, signatures, and encrypted labels are external
carrier artifacts only. They do not define OMI identity, acceptance, or
projection authority within OMI-Lisp.

## Scope Of This Document

This SPEC is doc-first. It defines the surface contract only. No parser,
lexer, or lowering executable is specified or implemented here. See
`LOWERING.md` for the lowering contract and `TEST_PLAN.md` for verification
intent.
