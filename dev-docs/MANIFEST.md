# MANIFEST.md
# OMI-Lisp Rebuild Root Manifest

## Active Root Files

The active root is intentionally small:

```text
REBUILD.md
STATUS.md
MANIFEST.md
_archive/
```

`REBUILD.md` is the active rebuild doctrine. `STATUS.md` records the current stable milestone. `MANIFEST.md` records the repository boundary and rebuild entry points. `_archive/` contains historical source material only.

Do not restore legacy files to the root.

## Archived Legacy Tree

Legacy material is preserved here:

```text
_archive/REBUILD-v1-superseded.md
_archive/omi-lisp-v1/
```

`_archive/REBUILD-v1-superseded.md` is the old audit. `_archive/omi-lisp-v1/` is the previous active `omi-lisp` tree moved intact under archive.

The archive may be read, cited, mined, or ported. It is not authority.

## Canonical Upstream Dependencies

Rebuild work must derive from the current canonical stack:

```text
omi-canon
  constitutional doctrine

omi-axioms
  formal constraints

omi-isa
  deterministic executable substrate

omi-tetragrammatron
  validation / construction geometry / projection kernel

omi-canvas
  typed construction engine, adapters, normalization, projection surfaces
```

Dependency order matters. Do not let legacy source material override this stack.

## Forbidden Authority Sources

These sources are not allowed to define production authority:

```text
_archive/omi-lisp-v1/
_archive/REBUILD-v1-superseded.md
/home/main/omi/omi-protocol
hash identity
digest identity
JSON blob authority
projection authority
runtime authority
carrier authority
```

Legacy OMI-Lisp and OMI protocol files are historical provenance only. Runtime, carrier, and projection layers may transport, stage, witness, or display accepted relations, but they do not create identity.

## Allowed Declaration Surfaces

The following surfaces may propose declarations:

```text
Markdown
JSON Canvas
CanvasEDSL
OMI-Lisp
omi---imo notation
typed Haskell constructors
future DOM / GPIO / LoRa / barcode projections
```

Declaration surfaces propose. Typed construction normalizes. Validation determines. Attestation witnesses. Projection displays.

## OMI-Lisp Rebuild Scaffold

ADAPTER_BOUNDARY.md
    handoff contract boundary definition

Doc-first declaration surface scaffold, added at the omi-lisp repo root.
This repository is already the OMI-Lisp rebuild root, so the scaffold lives
here directly rather than under a consumer adapter path.

```text
SPEC.md
    OMI-Lisp declaration surface specification (adapter layer, non-authoritative)

ADAPTER_BOUNDARY.md
    handoff contract from OMI-Lisp parser output to downstream adapters
    defines what downstream may and may not assume
    (adapter layer, non-authoritative)

ADAPTER_TEST_MATRIX.md
    shape preservation matrix and integration gate
    documents proof surface before any real omi-canvas dependency
    (adapter layer, non-authoritative)

LOWERING.md
    lowering contract: OMI-Lisp → typed construction candidate (downstream)

TEST_PLAN.md
    doc-first verification plan; no executable tests yet

src/
    omi_lisp.h       minimal node + candidate types (adapter layer),
                     OMI_LispSpan { ptr, len } for atom token ownership,
                     OMI_SourceSpan { ptr, len } for full parse span
    omi_lisp.c       seed + post-SP pair + post-SP symbol lowering stub,
                     zero-initializes source_span in all constructors,
                     omi_lisp_symbol_span() + omi_lisp_symbol_equals()
    omi_candidate.h  neutral typed construction candidate handoff shape + arena,
                     carries OMI_LispSpan from source node,
                     carries OMI_SourceSpan for parse-range tracking
    omi_candidate.c  maps OMI-Lisp candidate -> real OMI_Candidate tree in arena,
                     zero-initializes source_span in arena_alloc(),
                     copies source_span in convert_node() for all node kinds
    omi_parse.h      tiny fixture parser API + OMI_ParseResult enum,
                     OMI_ParseArena (caller-owned node storage),
                     depth overflow returns OMI_PARSE_ERR_DEPTH
    omi_parse.c      parser for NULL / symbol / (a . b) / (NULL . NULL) grammar,
                     plus nested pairs: (a . (b . c)) / ((a . b) . c) / etc.
                     stores parsed symbols as atom span (ptr + len) instead of
                     relying on null-terminated substrings,
                     allocates all parsed nodes from caller-owned OMI_ParseArena
                     (no static pool, no hidden global state),
                     sets source_span on parsed nodes:
                       symbol: source_span = atom span
                       pair:   source_span covers '(' through ')'
                       static: source_span = { NULL, 0 }
                     depth bounded at OMI_PARSE_MAX_DEPTH (16)
                     (no lists, no quote, no numbers, no comments, no eval,
                      no validation, no receipt, no omi-canvas import)
    omi_adapter_contract.h
                   adapter shape stub: OMI_AdapterNode, OMI_AdapterArena,
                   omi_adapter_from_candidate(), omi_adapter_is_authoritative()
                   (no omi-canvas import, no tetragrammatron import)
    omi_adapter_contract.c
                   maps OMI_Candidate tree -> OMI_AdapterNode tree,
                   preserves kind, atom_span, source_span, tree shape,
                   all authority flags at zero
    omi_canvas_boundary.h
                   compile-time boundary stub for future omi-canvas handoff
                   header-only, no .c implementation
                   declares OMI_CanvasBoundaryInput, OMI_CanvasBoundaryResult,
                   omi_canvas_boundary_check_shape()
                   (no omi-canvas import, no tetragrammatron import)

tests/
    test_seed.c      verifies seed candidate: pair, NULL car/cdr, accepted/validated/receipted = false
    test_pair.c      verifies SP gate: pair only becomes candidate after SP
    test_symbol.c    verifies SP gate + non-empty rule for symbol declarations
    test_candidate.c verifies handoff mapping: NULL/SYMBOL/PAIR -> typed candidate, never authoritative
    test_parse.c     verifies parser: NULL input, pre-SP gate, NULL/symbol/pair parse, trailing,
                     malformed, accepted/validated/receipted = 0, arena conversion,
                     source-span verification on seed, symbol, and pair nodes
    test_fixtures.c  verifies parser reads all .omi fixtures (including nested) from disk,
                     parses correctly, converts to arena, remains non-authoritative
    test_nested.c    verifies nested pair grammar: right/left/seed-nested/balanced,
                     atom span preservation, candidate conversion,
                     source-span verification from parse through conversion,
                     depth overflow (OMI_PARSE_ERR_DEPTH), malformed nested forms
    test_adapter_contract.c
                     verifies OMI_Candidate -> OMI_AdapterNode handoff:
                     tree shape, symbol atom_span, source_span, authority flags,
                     authoritative check, NULL input safety
    test_canvas_boundary.c
                     compile smoke test for omi_canvas_boundary.h header
                     verifies enum constants, struct init, function pointer,
                     no link-time dependency on omi-canvas
    test_authority_negative.c
                     proves no layer permits accepted/validated/receipted
                     to become authority; even manual flag mutation on
                     OMI_Candidate and OMI_AdapterNode does not slip past
                     is_authoritative stubs (hardcoded to 0)
    fixtures/
        seed.omi     declaration fixture: (NULL . NULL) — parsed by tests
        pair.omi     declaration fixture: (a . b) — parsed by tests
        symbol.omi   declaration fixture: a — parsed by tests
        nested-right.omi     declaration fixture: (a . (b . c)) — parsed by tests
        nested-left.omi      declaration fixture: ((a . b) . c) — parsed by tests
        nested-balanced.omi  declaration fixture: ((a . b) . (c . d)) — parsed by tests

Makefile
    builds test binaries into build/ via `make`
    runs test_seed, test_pair, test_symbol, test_candidate, test_parse, test_fixtures, test_nested via `make test`
    (build/ and *.o are git-ignored, not committed authority)

.gitignore
    ignores test_seed, test_parse, *.o, build/, dist/, .cache/

tests/
    placeholder for fixtures/ golden/ negative/ (future)
```

First implementation doctrine:

```text
OMI-Lisp text
    ↓
omi_parse_candidate (tiny fixture parser)
    ↓
OMI-Lisp candidate
    ↓
typed construction candidate (omi_candidate, real arena tree)
    ↓
downstream validation later (omi-canvas / omi-tetragrammatron)

Surface atoms representable as candidates:
    NULL    (seed: NULL . NULL)
    SYMBOL  (post-SP, non-empty text)
    PAIR    (post-SP, car/cdr candidates)
while still refusing acceptance, validation, and receipt.

Before SP: no readable candidate
After SP:  declaration may become a candidate
After validation: only downstream authority may accept state

accepted  = 0
validated = 0
receipted = 0
```

Implementation is a tiny fixture parser: NULL, symbol, (a . b), nested pairs.
No lists, no quote, no numbers, no comments, no evaluation, no validation
engine, no receipt authority, no omi-canvas import. `omi-protocol`
is untouched.

The three irreducible OMI-Lisp surface atoms are now representable, hand
off into a neutral typed construction candidate: NULL, SYMBOL, PAIR. The
parser reads text and produces candidates only — it does not evaluate,
validate, accept, or receipt.

Doctrine encoded in the scaffold:

```text
OMI-Lisp is not authority.
OMI-Lisp does not accept state.
OMI-Lisp does not validate state.
OMI-Lisp does not create receipts.
Dot notation is earned after SP.
The seed is (NULL . NULL).
Pair declarations are candidate relations only.
Lowering target is typed construction candidate, not accepted state.
Validation and receipt happen downstream.
```

This pass is doc-first only. No parser, lexer, or lowering executable is
implemented. No code is restored from `_archive/`. `omi-protocol` is untouched.

## Next Rebuild Entry Points

Begin rebuild work from these boundaries:

```text
1. REBUILD.md
   active doctrine, phase order, and acceptance criteria

2. omi-canon
   normalize constitutional language before implementation

3. omi-axioms
   map formal invariants before executable behavior

4. omi-isa
   import deterministic substrate only after canon and axioms are fixed

5. omi-tetragrammatron
   port validation, construction geometry, and projection kernel behavior

6. omi-canvas
   build typed construction, adapters, normalization, and projection surfaces

7. _archive/omi-lisp-v1/
   consult only as historical source material when a phase explicitly needs provenance
```

Guardrail:

```text
Do not rebuild from _archive/omi-lisp-v1 as authority.
Do not restore legacy files to root.
Do not edit /home/main/omi/omi-protocol as part of this rebuild root.
```
