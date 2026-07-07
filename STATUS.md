# STATUS.md
# OMI-Lisp Rebuild — Milestone Status

## 1. Current Milestone

```
OMI-Lisp declaration surface v0 + symbol span model + parser arena ownership + nested pair grammar + source-span tracking
```

## 2. Implemented Pipeline

```
fixture text
→ tiny parser (caller-owned OMI_ParseArena, nested pair recursion)
→ OMI_LispCandidate
→ OMI_CandidateArena
→ neutral typed construction candidate
→ downstream validation later
```

## 3. Supported Grammar

```
NULL
symbol
(a . b)
(NULL . NULL)
```

No lists, no quote, no numbers, no comments.

## 4. Explicit Non-Goals

- no lists
- no quote
- no numbers
- no comments
- no evaluation
- no validation
- no receipts
- no acceptance
- no omi-canvas import
- no tetragrammatron import

## 5. Authority Boundary

- OMI-Lisp proposes only
- candidates are accepted=0, validated=0, receipted=0
- downstream validation happens later (omi-canvas / omi-tetragrammatron)

## 6. Test Status

```
make clean && make test passes

test_seed       — seed candidate lowering contract
test_pair       — post-SP pair lowering gate
test_symbol     — post-SP symbol lowering gate
test_candidate  — OMI_LispCandidate → OMI_CandidateArena handoff
test_parse      — tiny fixture parser with caller-owned OMI_ParseArena,
                  two-arena concurrent parse verification
test_fixtures   — fixture file parsing from disk, separate-arena
                  concurrent fixture parse verification,
                  nested fixture parsing from disk + conversion
test_nested     — nested pair grammar: right, left, seed-nested, balanced,
                  candidate conversion, atom span verification,
                  source-span verification from parse through conversion,
                  depth overflow (ERR_DEPTH), malformed nested forms
test_adapter_contract
                — adapter shape stub: OMI_Candidate -> OMI_AdapterNode handoff,
                  tree shape, spans, authority flags, NULL safety
```

## 7. Fixture Status

```
tests/fixtures/seed.omi             (NULL . NULL)        — parsed
tests/fixtures/pair.omi             (a . b)              — parsed
tests/fixtures/symbol.omi           a                    — parsed
tests/fixtures/nested-right.omi     (a . (b . c))        — parsed + converted
tests/fixtures/nested-left.omi      ((a . b) . c)        — parsed + converted
tests/fixtures/nested-balanced.omi  ((a . b) . (c . d))  — parsed + converted
```

## 8. Symbol Span Model

Symbol text is represented as an atom span (OMI_LispSpan: pointer + length).
The parser does not require null-terminated symbol substrings.
The span is declaration-surface text only, not identity authority.

```
OMI_LispSpan { const char* ptr; size_t len; }

Symbol node:
    span.ptr -> first byte of symbol token
    span.len -> atom token length only (not full expression)

(a . b):
    car symbol: span.len = 1
    cdr symbol: span.len = 1
    pair node:  source_span covers full "(a . b)" (len = 7)
```

Spans are preserved through OMI_Candidate conversion.
Tests use `omi_lisp_symbol_equals(node, "text")` instead of `strcmp`.
Backward compat: `omi_lisp_lower_symbol("X", 1)` still sets `symbol` ptr.

## 9. Parser Arena Ownership

Parser node storage moved from a static pool to a caller-owned `OMI_ParseArena`.
All parsed `OMI_LispNode` values are allocated from the arena supplied by the caller.
No global parser pool. No hidden state between parse calls.

```
OMI_ParseArena { OMI_LispNode nodes[64]; int used; }

omi_lisp_parse_candidate_into(src, sp_seen, &arena, &out)
    → allocates all parsed nodes from arena
    → arena is reset at call start

omi_lisp_parse_candidate(src, sp_seen, &out)
    → convenience wrapper using internal static arena

Tests use _into with explicit arenas.
Two separate arenas can hold two independent parse results at once.
```

Arena exhaustion returns `OMI_PARSE_ERR_ARENA_FULL`.

## 10. Nested Pair Grammar

Parser already supported recursive `parse_atom` → `parse_pair` → `parse_atom` for
car/cdr, so nested pairs worked structurally. The only change needed was plumbing
`depth_overflow` through `OMI_ParseState` so depth failure returns `OMI_PARSE_ERR_DEPTH`
instead of `OMI_PARSE_ERR_UNEXPECTED`.

```
Supported nested forms:
(a . (b . c))        — right-nested
((a . b) . c)        — left-nested
((NULL . NULL) . a)  — seed-nested
((a . b) . (c . d))  — balanced

Depth bounded at 16 by OMI_PARSE_MAX_DEPTH.
17-level nesting returns OMI_PARSE_ERR_DEPTH.
```

All span and non-authoritative invariants preserved.

## 11. Source-Span Tracking

Each parsed node now carries a `source_span` (`OMI_SourceSpan`) that records the
full source range occupied by that node in the input text.

```
OMI_SourceSpan { const char* ptr; size_t len; }

Symbol node:
    source_span = same as atom span (one token)

Pair node:
    source_span.ptr -> '('
    source_span.len  = full distance from '(' through ')'

Static/manually constructed nodes (e.g. seed pair, lowered nodes):
    source_span = { NULL, 0 }
```

Three-layer span model:
1. **Atom span** (`span`) — symbol token only (declaration-surface text)
2. **Source span** (`source_span`) — full surface occupied by node (parser tracking)
3. **Candidate tree** — structural handoff (still non-authoritative, authority flags = 0)

Source spans are preserved through `OMI_CandidateArena` conversion via `convert_node()`.

### Verified Span Values

```
Input text              Node        source_span.len
───────                 ────        ───────────────
"a"                     symbol      1
"(a . b)"               outer pair  7
"(a . b)" → car         symbol      1
"(a . b)" → cdr         symbol      1
"(NULL . NULL)"         seed pair   13
"(a . (b . c))"         outer pair  13
"(a . (b . c))" → cdr   inner pair  7
"((a . b) . c)"         outer pair  13
"((a . b) . c)" → car   inner pair  7
"((a . b) . (c . d))"   outer pair  19
"((a . b) . (c . d))" → car  pair  7
"((a . b) . (c . d))" → cdr  pair  7
```

## 12. Next Safe Steps

1. ~~**symbol text ownership / span model**~~ (DONE)
2. ~~**parser arena ownership**~~ (DONE)
3. ~~**nested pair grammar**~~ (DONE)
4. ~~**source-span/range tracking**~~ (DONE)
5. ~~**adapter boundary doc**~~ (DONE — see `ADAPTER_BOUNDARY.md`)
6. ~~**adapter shape stub**~~ (DONE — see `src/omi_adapter_contract.{h,c}`)
7. **adapter handoff to omi-canvas** — only after ownership, grammar, boundary doc, and shape stub are stable
