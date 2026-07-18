# AGENTS.md
# OMI-Lisp Rebuild Root Agent Contract

## Purpose

This repository is now a rebuild root.

It is not the active legacy implementation.

The active root contract is:

```text
REBUILD.md
MANIFEST.md
AGENTS.md
```

The archive is historical source material only. Future agents must not see the small active root and restore archived legacy code back into authority.

## Authority Hierarchy

Canonical dependency order:

```text
1. omi-canon
   constitutional doctrine

2. omi-axioms
   formal invariants and proof constraints

3. omi-isa
   deterministic executable runtime reference

4. omi-tetragrammatron
   validation, construction geometry, projection kernel

5. omi-canvas
   typed construction engine, adapters, normalization, projection surfaces

6. omi-lisp
   rebuild root and declaration-surface adapter workspace
```

If legacy code conflicts with the canonical stack, the canonical stack wins.

## Repository Role

`omi-lisp` is no longer the root language authority.

`omi-lisp` is a rebuild workspace for lazy declaration surfaces that lower into typed OMI construction candidates.

OMI-Lisp may describe candidate relations.

OMI-Lisp may not accept state.

OMI-Lisp may not validate state.

OMI-Lisp may not create protocol identity.

## Archive Rules

_archive/omi-lisp-v1 is historical source material.

Agents may:

- read it
- cite it
- extract ideas from it

Agents may not:

- restore it wholesale
- treat it as current architecture
- copy legacy files into root without mapping them to a declared rebuild layer

`_archive/REBUILD-v1-superseded.md` is retained only as historical audit material. The active rebuild doctrine is `REBUILD.md`.

## Required Layer Labels

Every new implementation file must declare which layer it belongs to:

```text
doctrine
axiom
ISA
validation
construction
adapter
carrier
projection
test
```

Example for C:

```c
/*
 * OMI Layer: adapter
 * Purpose: Lower OMI-Lisp declarations into typed construction candidates.
 * Authority: Non-authoritative declaration surface only.
 */
```

Example for Markdown:

```markdown
Status: Adapter Layer
Authority: Non-authoritative declaration surface
Depends on: REBUILD.md, MANIFEST.md
```

## Declaration Surface Doctrine

Declaration surfaces propose.

They do not accept.

They do not validate.

They do not create identity.

They do not create receipts.

Allowed declaration surfaces:

```text
Markdown
JSON Canvas
CanvasEDSL
OMI-Lisp
omi---imo notation
DOM declarations
hardware declarations
carrier declarations
```

All declaration surfaces must lower into:

```text
typed declaration
↓
construction candidate
↓
validation
↓
receipt
↓
projection
```

## Identity Doctrine

Address is identity.

Place-value relation is witness.

Validation judges relation.

Carry-forward proves validation.

Hashes, digests, checksums, signatures, encrypted labels, fingerprints, and cache keys are external carrier artifacts only.

Do not derive OMI identity from hashes.

Do not derive validation from hashes.

Do not derive slot selection from hashes.

Do not derive routing identity from hashes.

Do not derive projection authority from hashes.

Allowed external metadata:

```text
CRC for LoRa packet integrity
checksum for external file transfer
signature for external operator policy
hash for foreign database indexing
```

Forbidden authority substitutions:

```text
hash as OMI identity
digest as receipt
checksum as validation proof
signature as protocol authority
encrypted label as citation identity
```

## Carrier Distinction

Agents must preserve this separation:

```text
Place-value frame:
    identity-bearing relation frame

512-bit envelope:
    carrier / boot / staging envelope

512-byte sector:
    eight-cell carrier block

transport:
    byte-stream carrier only
```

Carrier layers transport identity-bearing relations.

Carrier layers do not create identity.

## OMI-Lisp Rule

OMI-Lisp is the lazy declaration language for OMI relations.

It is not the validation engine.

It is not the receipt authority.

It becomes readable only after the pre-language control stream reaches SP.

Dot notation is not primitive at byte zero.

Dot notation is earned after SP.

```text
Before SP:
    control stream only

At SP:
    readable boundary induced

After SP:
    OMI-Lisp may express declarations

After validation:
    state may be accepted by validation plus receipt only
```

## Canonical OMI Notation Version

The current canonical notation surface is:

```text
omi---imo?O_o
```

- `omi---imo` is the folded relation sign (carrier).
- `?O_o` is the readable witness surface (notation).
- Both share an OMI relation type.
- Their shared type does not imply identical spelling or identical local interpretation.

Canonical distinction:

```text
#/ belongs to carrier compatibility syntax.
?O_o belongs to OMI-native notation.
```

### 8-tuple (pedagogical scaffold)

The 8-tuple is a pedagogical automata-style scaffold for explaining
how OMI notation may be read as a bounded relation frame. It is not
canonical structure, not an authority layer, and not a proof object.

```text
Q Σ L R δ s t r
```

Role meanings (pedagogical, not canonical identity):

```text
Q   state space / identity context
Σ   alphabet / allowed symbols
L   left / source / beginning
R   right / target / ending
δ   transition / transformation
s   start value
t   accept / success / terminal output
r   reject / failure / alternative
```

Address cascade (explanatory overlay, not canonical identity):

```text
S0 may be explained as Q
S1 may be explained as Σ
S2 may be explained as L
S3 may be explained as R
S4 may be explained as δ
S5 may be explained as s
S6 may be explained as t
S7 may be explained as r
```

### Register compression (pedagogical pairings)

The 8-tuple may be paired into four pedagogical relation carriers:

```text
CAR      ≈ Q / Σ    pedagogical pairing, not required integer width
CDR      ≈ L / R    pedagogical pairing, not required integer width
PAYLOAD  ≈ δ / s    pedagogical pairing, not required integer width
MASK     ≈ t / r    pedagogical pairing, not required integer width
```

Full OMI cell:

```text
8 × 16-bit address/ruler fields  (S0-S7, pedagogically named by Q Σ L R δ s t r)
+ 4 × 32-bit paired relation fields  (CAR, CDR, PAYLOAD, MASK, pedagogical pairings)
```

Dotted-pair nesting:

```text
((Q . Σ)   CAR relation
 (L . R)   CDR relation
 (δ . s)   PAYLOAD relation
 (t . r))  MASK relation
```

### Null Ring

The folded null seed:

```text
(NULL . NULL)
```

Byte ring:

```text
(0x00 . 0x20)    0x00 ^ 0x20 = 0x20
(0x20 . 0x7F)    0x20 ^ 0x7F = 0x5F
(0x7F . 0xFF)    0x7F ^ 0xFF = 0x80
(0xFF . 0x00)    0xFF ^ 0x00 = 0xFF
```

Full witness closure:

```text
0x20 ^ 0x5F ^ 0x80 ^ 0xFF = 0x00
```

### F-column byte bands

The F-column is a tangential projection from the hidden control row endpoint
into the printable branch column. It is not arbitrary character selection.

Byte-geometry model:

```text
linear hidden row:
  0x00 ... 0x0F (pinch point: low nibble control row end)
  ...           0x1F (hinge: last hidden unit separator)

tangential printable branch (projecting from 0x1F):
  0x1F  US    hidden unit separator   (hinge point)
  0x2F  /     printable separator
  0x3F  ?     witness / query separator
  0x4F  O     upper / norm Omicron
  0x5F  _     carrier / floor separator
  0x6F  o     lower / local omicron

projective horizon:
  0x7F        seven-bit gauge endpoint (first deletion/seal boundary)
  0x80        interpretive space opens
  0xFF        saturated carrier horizon
```

Key invariants:

```text
0x0F pinches the low control nibble.
0x1F closes the hidden unit row AND is the hinge for the F-column branch.
0x20 opens readable separation.
0x1F branches tangentially through US / ? O _ o.
0x7F closes the seven-bit projective gauge.
0x80 opens interpretive space.
0xFF saturates the carrier horizon.
```

Therefore:

```text
omi---imo
  folded root relation

?O_o
  F-column branch from hidden unit separation into readable witnessed relation
```

### Agent rules

- Use `omi---imo?O_o` as the current canonical notation surface.
- Do not infer canonical notation from older nearby files.
- Do not promote `#/` from carrier compatibility into OMI-native syntax.
- Do not treat the 8-tuple as `8 × 16-bit` numeric coordinates unless
  explicitly discussing a later lowering.
- Do not treat the 8-tuple as canonical structure, authority, or proof object.
- Do not treat the four dotted pairs as `4 × 32-bit` integer widths.
- Do not treat geometry drawings as authority.
- Do not let projection, route, notation, or gauge become authority.
- Do not use older nearby forms as canonical unless explicitly marked legacy.

Authority order:

```text
Omnicron frames.
Tetragrammatron validates.
Metatron scribes.
Receipt records.
```

Short form:

```text
8-tuple = meaning.
F-column = notation.
Null Ring = closure.
Tetragrammatron = validation.
Metatron = scribe.
Receipt = authority.
```

## Forbidden Changes

Agents must not:

- restore archived legacy files to root
- treat `_archive/omi-lisp-v1` as authority
- make OMI-Lisp the validation engine
- make OMI-Lisp the receipt authority
- introduce hash identity into protocol core
- treat projection as authority
- treat runtime carrier services as authority
- collapse Markdown, CanvasEDSL, OMI-Lisp, and `omi---imo` into separate competing protocols
- modify `/home/main/omi/omi-protocol` during `omi-lisp` rebuild tasks unless explicitly requested
- create new top-level implementation trees without updating `MANIFEST.md`

## Allowed Changes

Agents may:

- add adapter specs
- add parser/lowering prototypes
- add tests
- add docs mapping legacy concepts to canonical layers
- extract small legacy pieces when they are assigned to a declared layer
- create stubs for Markdown, CanvasEDSL, and OMI-Lisp adapters
- update `MANIFEST.md` when root structure changes
- update `REBUILD.md` only when rebuild doctrine changes

## Coding Workflow

1. Read `REBUILD.md`.
2. Read `MANIFEST.md`.
3. Read `AGENTS.md`.
4. Identify the target layer.
5. Check canonical upstream dependency.
6. Make the smallest layer-correct change.
7. Add or update tests.
8. Verify no forbidden authority boundary was crossed.
9. Update `MANIFEST.md` if files or directories changed.
10. Report changes with file paths and verification results.

## Required Verification Commands

Use lightweight root checks:

```sh
pwd
find . -maxdepth 2 -type f | sort
git status --short
```

Guard against accidental restore:

```sh
find . -maxdepth 1 -mindepth 1 \
  ! -name '.git' \
  ! -name 'REBUILD.md' \
  ! -name 'STATUS.md' \
  ! -name 'MANIFEST.md' \
  ! -name 'AGENTS.md' \
  ! -name 'SPEC.md' \
  ! -name 'ADAPTER_BOUNDARY.md' \
  ! -name 'ADAPTER_TEST_MATRIX.md' \
  ! -name 'LOWERING.md' \
  ! -name 'TEST_PLAN.md' \
  ! -name 'Makefile' \
  ! -name '.gitignore' \
  ! -name 'src' \
  ! -name 'tests' \
  ! -name 'build' \
  ! -name '_archive'
```

That command should return nothing unless the agent intentionally added a new approved root file. `build/` is an ignored build-output directory and is explicitly allowed by this guard. Confirm it is ignored with `git check-ignore build`.

Check forbidden identity drift:

```sh
grep -R "hash.*identity\|digest.*identity\|checksum.*identity" \
  REBUILD.md MANIFEST.md AGENTS.md || true
```

Check `omi-protocol` untouched:

```sh
git -C /home/main/omi/omi-protocol status --short
```

For implementation changes, run the relevant local tests for the layer being edited. If no test exists yet, add a minimal test or document the missing test gap in the handoff.

## Handoff Format

When finishing a task, report:

```text
Summary:
- what changed

Files changed:
- path
- layer
- purpose

Boundary checks:
- archive not restored
- omi-protocol untouched unless requested
- no hash identity introduced
- declaration surfaces remain non-authoritative
- carrier layers remain non-identity

Verification:
- commands run
- results

Next recommended step:
- one concrete next action
```
