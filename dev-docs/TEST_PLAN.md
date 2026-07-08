Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md, AGENTS.md, SPEC.md, LOWERING.md

# TEST_PLAN.md
# OMI-Lisp Verification Plan

## Status

This is a **doc-first test plan**. No parser or test executable exists yet.
The plan enumerates intended verification categories so that the future
parser/lowering stub has a defined acceptance surface.

## Required Properties To Verify

### 1. Non-authority

- OMI-Lisp declarations never produce accepted state.
- OMI-Lisp never emits a receipt.
- OMI-Lisp never performs validation.

### 2. Seed Invariant

- The literal `(NULL . NULL)` lowers to the seed construction candidate.
- No other literal lowers to the seed.

### 3. SP Boundary

- Any dot-notation reading before `SP` is rejected.
- Declarations are only readable after the SP boundary is reached.
- The control stream prior to `SP` does not yield pairs.

### 4. Candidate Lowering

- Every post-SP pair lowers to a typed construction candidate.
- The candidate carries no acceptance flag.
- The candidate carries no validation result.
- The candidate carries no receipt.

### 5. Identity Doctrine

- No hash, digest, checksum, signature, or encrypted label defines identity.
- No hash/digest/checksum is treated as validation proof or receipt.

### 6. No Archive Restore

- No code from `_archive/omi-lisp-v1` is restored into authority.
- `_archive/` remains historical source material only.

### 7. omi-protocol Clean

- `/home/main/omi/omi-protocol` is not modified by OMI-Lisp rebuild work.

## Future Test Layout

When the parser/lowering stub is added under `src/`, tests should live in
`tests/`:

```text
tests/
    fixtures/    sample OMI-Lisp declarations
    golden/      expected construction candidates
    negative/    SP-boundary and malformed inputs
```

## Scope Of This Document

Plan only. No executable tests are added in this pass. The next concrete step
is a parser/lowering stub under `src/` with minimal tests covering the seed
invariant and the SP boundary.
