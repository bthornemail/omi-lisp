# REBUILD.md
# OMI Protocol Rebuild: Canonical Construction System

## Supersession Notice

This is REBUILD.md v2. It explicitly supersedes the older OMI-Lisp protocol rebuild audit now archived at:

```text
_archive/REBUILD-v1-superseded.md
_archive/omi-lisp-v1/
```

The older audit correctly found that there was no single `omi-lsp` repository and that the historical material lived across `omi-lisp/` and `omi-protocol/`, with newer extracted work in `omi-tetragrammatron/` and `omi-isa/`. Its interpretation is now replaced.

The rebuild target is:

```text
archive legacy OMI-Lisp / protocol material
then rebuild the production protocol from the current canonical stack
```

## Corrected Doctrine

OMI-Lisp is not the language of the whole system. OMI-Lisp is one lazy declaration surface.

The production rebuild is grounded in this dependency order:

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
  typed construction engine and adapters
```

Core rule:

```text
Address is identity.
Carrier transports identity-bearing relation.
Carrier does not become identity.
```

## Repository Authority Map

```text
omi-canon
  owns constitutional language

omi-axioms
  owns formal invariants

omi-isa
  owns deterministic execution substrate

omi-tetragrammatron
  owns validation, construction geometry, projection kernel

omi-canvas
  owns typed construction engine, adapters, normalization, projection surfaces

legacy omi-lisp / omi-protocol
  archived source material only
```

`omi-lisp/` and `omi-protocol/` are historical source material. They may be cited, mined, or ported, but they are not production authority.

No files in `/home/main/omi/omi-protocol` are moved or edited by this rebuild boundary. Historical `omi-lisp` material is archived inside this repository under `_archive/`.

## Identity Frame, Carrier Envelope, Carrier Block

The older audit conflated relation frame and carrier envelope.

The rebuild separates:

```text
Identity frame
  addressed place-value relation

Carrier envelope
  pre-header + place-value frame + typed declaration payload

Carrier block
  8-cell staging / boot / projection block
```

Correct sizing doctrine:

```text
128-bit / 256-bit place-value frame = identity-bearing relation frame

512-bit envelope = carrier / boot envelope

512-byte sector = 8-cell carrier block

4096-bit block = carrier orbit
```

A 512-byte carrier block may contain eight 512-bit boot cells. The carrier block does not create identity. The addressed relation remains identity.

Therefore the active doctrine is:

```text
The old audit conflated relation frame and carrier envelope.
Carrier structures transport or stage identity-bearing relations, but do not create identity.
```

## Declaration Surfaces

These are declaration surfaces:

```text
Markdown
JSON Canvas
CanvasEDSL
OMI-Lisp
omi---imo notation
typed Haskell constructors
future DOM / GPIO / LoRa / barcode projections
```

None of these surfaces are authority.

The authority rule is:

```text
Declaration surfaces propose.
Typed construction normalizes.
Validation determines.
Attestation witnesses.
Projection displays.
```

A single symbolic grammar may project into Markdown, JSON Canvas, OMI-Lisp, Haskell, ISA, DOM, SVG, React, GPIO, LoRa, eMMC, bitboards, barcode carriers, or future carriers. None of those implementations redefine the grammar.

## Canonical Architecture Pipeline

Use this canonical pipeline:

```text
Canonical Grammar
↓
Relational Access
↓
Construction Grammar
↓
Typed Resolution
↓
Carrier Projection
```

OMI Canvas is the typed construction, normalization, adapter, and projection engine. Canvas output is a projection surface, not an authority surface.

Runtime is Carrier Runtime. It handles memory, storage, network, gossip, scheduler, reconcile, and stream behavior. Runtime does not become protocol authority.

## Rebuild Phases

### Phase 0: Archive legacy repositories

Archive OMI-Lisp material as historical source material. Keep `omi-protocol` untouched and reference it only as external legacy provenance.

### Phase 1: Normalize canonical doctrine

Use `omi-canon` as the constitutional language source.

### Phase 2: Normalize formal mathematics

Use `omi-axioms` as the formal invariant and proof boundary.

### Phase 3: Normalize deterministic ISA

Use `omi-isa` as the deterministic executable substrate.

### Phase 4: Normalize Tetragrammatron

Use `omi-tetragrammatron` for construction geometry, validation, and projection kernel behavior.

### Phase 5: Build OMI Canvas

Use `omi-canvas` as the typed construction engine.

### Phase 6: Port Markdown

Map Markdown to typed declarations, then to the construction graph.

### Phase 7: Port CanvasEDSL

Map Canvas JSON through adapters into the construction graph, then into projection surfaces.

### Phase 8: Rewrite MCRSGSP

Use:

```text
Omi-Notation
↓
Omi-Citation
↓
Omi-Delta
↓
Validation
↓
Omi-Attestation
↓
Omi-Attribution
↓
Projection
```

### Phase 9: Rewrite Runtime as Carrier Runtime

Rebuild runtime around memory, storage, network, gossip, scheduler, reconcile, and stream carrier behavior.

### Phase 10: Hardware adapters

Treat LoRa, GPIO, ESP32, sensors, and eMMC as carrier or projection adapters.

### Phase 11: Projection surfaces

Treat DOM, SVG, React, Canvas, barcode, and JabCode as projection surfaces.

### Phase 12: Production integration

Establish CI, tests, docs, and release boundaries around the canonical stack.

## Corrected MCRSGSP Target

The older JSON/digest propagation model is deprecated.

The propagation unit is:

```text
omi---imo address
↓
Gauge Header
↓
Place-Value Frame
↓
typed declaration
↓
validated relation
↓
attestation / attribution
↓
projection
```

Explicit exclusions:

```text
No hash identity.
No digest identity.
No JSON blob as authority.
No projection authority.
```

MCRSGSP may participate as carrier recovery, anti-entropy, or propagation machinery. It does not define identity, acceptance, or projection authority.

## Runtime And Hardware Adapter Boundaries

Carrier Runtime owns operational transport and staging:

```text
memory
storage
network
gossip
scheduler
reconcile
stream
```

Hardware adapters are projection or carrier endpoints:

```text
LoRa
GPIO
ESP32
sensors
eMMC
```

Projection adapters display or emit accepted construction:

```text
DOM
SVG
React
Canvas
barcode
JabCode
bitboards
future carriers
```

Adapters can carry, stage, render, or witness. They do not create identity.

## Acceptance Criteria

The rebuild is acceptable only when:

- legacy OMI-Lisp material is archived under `_archive/`
- `omi-protocol` remains untouched by this repository-local rebuild
- production authority is based on `omi-canon`, `omi-axioms`, `omi-isa`, `omi-tetragrammatron`, and `omi-canvas`
- OMI-Lisp is described as one declaration surface, not the system language
- the 128-bit / 256-bit identity frame is distinguished from 512-bit carrier envelopes
- carrier blocks and carrier orbits are treated as staging and transport, not identity
- MCRSGSP is rewritten around notation, citation, delta, validation, attestation, attribution, and projection
- hash identity, digest identity, JSON blob authority, and projection authority are rejected
- runtime is described as Carrier Runtime, not protocol authority
