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
