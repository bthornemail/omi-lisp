# STATUS.md
# OMI-Lisp Rebuild — Milestone Status

## 1. Current Milestone

```
OMI-Lisp declaration surface v0 + symbol span model + parser arena ownership
```

## 2. Implemented Pipeline

```
fixture text
→ tiny parser (caller-owned OMI_ParseArena)
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
                  concurrent fixture parse verification
```

## 7. Fixture Status

```
tests/fixtures/seed.omi      (NULL . NULL)  — parsed
tests/fixtures/pair.omi      (a . b)        — parsed
tests/fixtures/symbol.omi    a              — parsed
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
    pair node:  no span set (reserved for future source_span)
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

## 10. Next Safe Steps

1. ~~**symbol text ownership / span model** — formalize parser symbol text as pointer + length instead of bare source pointer~~ (DONE)
2. ~~**parser arena ownership** — move parser node lifetime into an explicit arena rather than static pool~~ (DONE)
3. **nested pair grammar** — extend parser to support pairs whose car/cdr are themselves pairs
4. **adapter handoff to omi-canvas** — only after ownership and grammar are stable
