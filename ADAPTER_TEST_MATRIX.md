# ADAPTER_TEST_MATRIX.md
# Adapter Shape Stub — Test and Preservation Matrix

Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md, ADAPTER_BOUNDARY.md

## 1. Adapter Chain

```text
.omi text
↓
OMI_ParseArena
↓
OMI_LispNode
↓
OMI_LispCandidate
↓
OMI_CandidateArena
↓
OMI_Candidate
↓
OMI_AdapterArena
↓
OMI_AdapterNode
↓
future downstream authority
```

Every layer preserves structure without adding authority.

## 2. Shape Preservation Matrix

For each declaration form the parser can produce, the matrix tracks whether kind,
spans, and authority flags survive conversion through the full chain.

| Form | Layer | Kind | Atom Span | Source Span | accepted=0 | validated=0 | receipted=0 |
|---|---|---|---|---|---|---|---|
| **NULL** | parser | OMI_LISP_NODE_NULL | — | zero | — | — | — |
| | candidate | OMI_CANDIDATE_NULL | — | preserved | ✓ | ✓ | ✓ |
| | adapter | OMI_ADAPTER_KIND_NULL | — | preserved | ✓ | ✓ | ✓ |
| **SYMBOL** | parser | OMI_LISP_NODE_SYMBOL | set | set | — | — | — |
| | candidate | OMI_CANDIDATE_SYMBOL | preserved | preserved | ✓ | ✓ | ✓ |
| | adapter | OMI_ADAPTER_KIND_SYMBOL | preserved | preserved | ✓ | ✓ | ✓ |
| **PAIR** (a . b) | parser | OMI_LISP_NODE_PAIR | — | set | — | — | — |
| | candidate | OMI_CANDIDATE_PAIR | — | preserved | ✓ | ✓ | ✓ |
| | adapter | OMI_ADAPTER_KIND_PAIR | — | preserved | ✓ | ✓ | ✓ |
| **Right nested** (a . (b . c)) | parser | nested PAIRs | set on symbols | set on all | — | — | — |
| | candidate | nested CANDIDATE_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |
| | adapter | nested ADAPTER_KIND_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |
| **Left nested** ((a . b) . c) | parser | nested PAIRs | set on symbols | set on all | — | — | — |
| | candidate | nested CANDIDATE_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |
| | adapter | nested ADAPTER_KIND_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |
| **Balanced** ((a . b) . (c . d)) | parser | nested PAIRs | set on symbols | set on all | — | — | — |
| | candidate | nested CANDIDATE_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |
| | adapter | nested ADAPTER_KIND_PAIRs | preserved | preserved | ✓ | ✓ | ✓ |

Key:
- `—` = not applicable (NULL has no atom span; PAIR kind checked on node itself)
- `set` = span assigned by parser (does not exist on static/lowered nodes)
- `preserved` = span copied without loss through conversion step
- `zero` = static source span is {NULL, 0}
- `✓` = verified by test assertion

## 3. Explicit Non-Authority

An `OMI_AdapterNode` is:

- **not** validation
- **not** acceptance
- **not** receipt
- **not** projection
- **not** identity

It is a shape contract. It carries structural information from the declaration
surface through a typed intermediate representation that downstream authority
may later judge.

OMI-Lisp lowers readable declarations into inspectable candidates.
Inspection is not acceptance.
Normalization is not validation.
Projection is not receipt.

## 4. Current Tests

All 9 test binaries pass (293+ assertions):

| Binary | Assertions | Scope |
|---|---|---|
| `test_seed` | 11 | seed candidate lowering contract |
| `test_pair` | 8 | post-SP pair lowering gate |
| `test_symbol` | 24 | post-SP symbol lowering, atom spans |
| `test_candidate` | 22 | OMI_LispCandidate → OMI_CandidateArena |
| `test_parse` | 62 | parser with arena ownership, source spans |
| `test_fixtures` | 78 | fixture parsing from disk, conversion |
| `test_nested` | 50 | nested pairs, atom/source spans, depth bounds |
| `test_adapter_contract` | 36 | OMI_Candidate → OMI_AdapterNode handoff |
| `test_canvas_boundary` | 11 | compile-time boundary stub, enum guard |

## 5. Integration Gate

Before importing omi-canvas (or any downstream dependency), the following
must be true:

- [x] adapter test matrix complete (this document)
- [x] all tests pass (`make clean && make test`)
- [x] no identity drift (grep-clean in src/ and adapter-layer docs)
- [x] no authority flags set (accepted=validated=receipted=0 everywhere)
- [x] no omi-protocol modifications
- [x] root boundary guard clean (no unexpected root files)
- [x] build artifacts git-ignored

When all gates are green, the first integration step is a **compile-time
boundary stub** — a header that downstream can include without runtime
dependency. Not an import. Not a functional integration. Not validation.
