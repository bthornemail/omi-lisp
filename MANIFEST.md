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

Doc-first declaration surface scaffold, added at the omi-lisp repo root.
This repository is already the OMI-Lisp rebuild root, so the scaffold lives
here directly rather than under a consumer adapter path.

```text
SPEC.md
    OMI-Lisp declaration surface specification (adapter layer, non-authoritative)

LOWERING.md
    lowering contract: OMI-Lisp → typed construction candidate (downstream)

TEST_PLAN.md
    doc-first verification plan; no executable tests yet

src/
    omi_lisp.h       minimal node + candidate types (adapter layer),
                     OMI_LispSpan { ptr, len } for atom token ownership
    omi_lisp.c       seed + post-SP pair + post-SP symbol lowering stub,
                     omi_lisp_symbol_span() + omi_lisp_symbol_equals()
    omi_candidate.h  neutral typed construction candidate handoff shape + arena,
                     carries OMI_LispSpan from source node
    omi_candidate.c  maps OMI-Lisp candidate -> real OMI_Candidate tree in arena
    omi_parse.h      tiny fixture parser API + OMI_ParseResult enum,
                     OMI_ParseArena (caller-owned node storage)
    omi_parse.c      parser for NULL / symbol / (a . b) / (NULL . NULL) grammar,
                     stores parsed symbols as atom span (ptr + len) instead of
                     relying on null-terminated substrings,
                     allocates all parsed nodes from caller-owned OMI_ParseArena
                     (no static pool, no hidden global state)
                     (no lists, no quote, no numbers, no comments, no eval,
                      no validation, no receipt, no omi-canvas import)

tests/
    test_seed.c      verifies seed candidate: pair, NULL car/cdr, accepted/validated/receipted = false
    test_pair.c      verifies SP gate: pair only becomes candidate after SP
    test_symbol.c    verifies SP gate + non-empty rule for symbol declarations
    test_candidate.c verifies handoff mapping: NULL/SYMBOL/PAIR -> typed candidate, never authoritative
    test_parse.c     verifies parser: NULL input, pre-SP gate, NULL/symbol/pair parse, trailing,
                     malformed, accepted/validated/receipted = 0, arena conversion
    test_fixtures.c  verifies parser reads seed.omi / pair.omi / symbol.omi from disk,
                     parses correctly, converts to arena, remains non-authoritative
    fixtures/
        seed.omi     declaration fixture: (NULL . NULL) — parsed by tests
        pair.omi     declaration fixture: (a . b) — parsed by tests
        symbol.omi   declaration fixture: a — parsed by tests

Makefile
    builds test binaries into build/ via `make`
    runs test_seed, test_pair, test_symbol, test_candidate, test_parse, test_fixtures via `make test`
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

Implementation is a tiny fixture parser only: NULL, symbol, (a . b).
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
