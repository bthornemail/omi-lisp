# ADAPTER_BOUNDARY.md
# OMI-Lisp Adapter Boundary Contract

Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md, SPEC.md, LOWERING.md

## 1. Boundary Statement

OMI-Lisp produces declaration candidates only.
It does not validate, accept, receipt, or project.

OMI-Lisp lowers readable declarations into inspectable candidates.

Inspection is not acceptance.
Normalization is not validation.
Projection is not receipt.

## 2. Input Surface

Supported current declaration grammar:

```text
NULL
symbol
(a . b)
(a . (b . c))       right-nested
((a . b) . c)       left-nested
((NULL . NULL) . a) seed-nested
((a . b) . (c . d)) balanced
```

Not supported (explicit non-goals):
- lists
- quote
- numbers
- comments
- evaluation
- validation
- receipts
- acceptance

## 3. Parser Output

Parser produces:

- **OMI_ParseArena**: caller-owned node storage; no static pool, no hidden state
- **OMI_LispCandidate**: wraps parsed root node with authority flags at zero
- **OMI_LispNode tree**: kind (NULL/SYMBOL/PAIR), car/cdr links
- **Atom spans**: `OMI_LispSpan { ptr, len }` for each symbol token
- **Source spans**: `OMI_SourceSpan { ptr, len }` for every parsed node
- **accepted = 0**
- **validated = 0**
- **receipted = 0**

### Span model

```text
atom span ≠ source span ≠ identity

atom span:
    symbol token only (declaration-surface text)

source span:
    full source range occupied by a parsed node

candidate tree:
    structural handoff, still non-authoritative
```

### Example

```text
(a . (b . c))

root pair source span      = "(a . (b . c))"  (len 13)
inner pair source span     = "(b . c)"         (len 7)
symbol "a" source span     = "a"               (len 1, equals atom span)
symbol "b" source span     = "b"               (len 1, equals atom span)
symbol "c" source span     = "c"               (len 1, equals atom span)

seed pair "(NULL . NULL)"  source span len 13  (static, zero source_span)
```

## 4. Typed Candidate Handoff

`OMI_LispCandidate` converts via `omi_candidate_from_lisp_into()` into:

- **OMI_CandidateArena**: arena-allocated typed candidate tree
- **OMI_Candidate tree**: NULL/SYMBOL/PAIR with typed discriminators
- **Preserved atom spans**: copied from source node `span` field
- **Preserved source spans**: copied from source node `source_span` field
- **Non-authoritative flags**: accepted=0, validated=0, receipted=0

## 5. What Downstream May Do

Downstream adapters (omi-canvas, omi-tetragrammatron) may:

- inspect `OMI_Candidate` kind
- inspect tree shape (car/cdr structure)
- inspect atom spans for symbol text
- inspect source spans for original parse range
- normalize into their own typed construction forms
- reject malformed or unsupported structures
- submit candidates to validation authority later

## 6. What Downstream Must Not Assume

Downstream must not assume:

- parser success means validation
- candidate means accepted state
- source span means identity
- atom span means identity
- tree shape means receipt
- symbol text means canonical address
- fixture text means authority
- parser arena lifetime extends beyond immediate handoff

## 7. Authority Boundary

Only downstream validation/receipt authority may set:

- **accepted = 1**
- **validated = 1**
- **receipted = 1**

OMI-Lisp must never set those flags.

All current candidates carry:

```text
accepted  = 0
validated = 0
receipted = 0
```

This is verified by every test binary.

## 8. Current Test Evidence

All 7 test binaries pass (246 assertions total):

| Binary | Scope |
|---|---|
| `test_seed` | seed candidate lowering contract |
| `test_pair` | post-SP pair lowering gate |
| `test_symbol` | post-SP symbol lowering gate |
| `test_candidate` | OMI_LispCandidate → OMI_CandidateArena handoff |
| `test_parse` | parser with arena ownership, source-span tracking |
| `test_fixtures` | fixture file parsing from disk + arena conversion |
| `test_nested` | nested pair grammar + span preservation + depth bounds |

### Verification invariants

- root boundary clean: no unexpected root files
- `omi-protocol` untouched
- no identity drift in `src/`
- `build/` and `*.o` git-ignored
- no hash identity introduced
- all authority flags at zero

## 9. Next Implementation Step

After this doc is committed:

1. Create `src/omi_adapter_contract.h` — adapter shape stub declaring the handoff types a downstream consumer should implement. Still no real `omi-canvas` import. Still no validation.
2. Update `STATUS.md` to mark adapter boundary doc done and promote adapter shape stub as the next step.
