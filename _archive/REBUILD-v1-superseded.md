REBUILD.md — OMI-Lisp Protocol Rebuild Audit & Plan
0. Terminology Correction
Your Term	Actual Folder	Status
omi-lsp	omi-lisp/ + omi-protocol/	Legacy, no lsp folder exists
omi-tetragrammatron	omi-tetragrammatron/	Extracted upgrade
omi-isa	omi-isa/	Canonical latest
1. Full File Inventory & Mapping
1.1 Legacy: omi-lisp/ (87 files, ~12.5 MB)
Module	Files	Lines	Fate
Core C runtime	omi_header8_runtime.c, omi_riscv_vm.c	~35K	Archive
Lisp source	6 .lisp files	~47K	Archive
RISC-V build	build-riscv/ (kernel, initramfs, qcow2)	~7.4MB	Archive
Build scripts	3 .sh, 3 .scm	~6K	Archive
Documentation	14 dev-docs/ layers, AGENTS, README	~200K	Archive
Tutorial	tutorial/ (5 files)	~16K	Archive
Simulator	simulator.html	16K	Archive
Map to new home: _archive/omi-lisp/ with original tree preserved.
1.2 Legacy: omi-protocol/ (30 files, ~1.49 MB)
Module	Files	Lines	Fate
AGI contracts	REPO.md, AGENTS.md, SKILLS.md, ADAPTERS.md	~90K	Archive
Protocol spec	OMI-PROTOCOL.md, OMI-QUICK-REFERENCE.md	~20K	Archive
Comprehensive docs	OMI-Lisp_Complete_Specification.md, OMI_System_Integration_Layer.md, OMI_Canonical_Doctrine.md, OMI_Complete_System_Unified_Synthesis.md	~106K	Archive
Coq proofs	omi_pi_proof.v (+ compiled artifacts)	~147K + 821K	Archive (superseded by omi-axioms/)
HTML projections	index.html, resolver.html, world.html, governance.html	~87K	Archive
Session logs	session-ses_0db3.md, DeepSeek Chat about Coq Proofs.md	~456K	Archive
Map to new home: _archive/omi-protocol/ with original tree preserved.
1.3 Extracted: omi-tetragrammatron/ (~120 files, ~400 MB inc. PDFs)
Module	Files	Lines	Fate
Core C (4 authorities)	core/omi.c/h, core/tetragrammatron.c/h, core/metatron.c/h, core/imo.c/h, core/omicron.c/h	~320K	Merge into omi-isa/lib/
Coq proofs	proof/phi_proof.v, proof/omi_pi_proof.v	~42K	Archive (superseded by omi-axioms/)
Portal (React/TS)	portal/ (package.json, src/, vite, etc.)	~12K + 133K lock	Keep as gate/ subdirectory
Agent docs	agent-docs/AGENTS.md, SKILLS.md, ADAPTERS.md, REPO.md	~93K	Merge into omi-isa/docs/
Build/Make	Makefile	3.4K	Merge
Scripts/tests	scripts/test-*.c (3 files)	~24K	Merge into omi-isa/test/
Dev docs	dev-docs/ (many, inc. large PDFs 13-20MB each)	~200MB	Archive (selective)
Archive docs	dev-docs/archive/ (historical papers, viewer, chat history)	~200MB	Archive
Key merge candidates from tetragrammatron → isa:
Feature	File(s)	Priority
21 solid shape DB + 6 renderers	core/metatron.c (64KB)	High — isa lacks projection layer
Nibble CPU (32-nibble executor)	core/omi.c (embedded)	Medium — isa has different CPU model
Ring serialization (file I/O)	core/imo.c (ring_save/load)	Medium — isa has ring via omi_orbit.c
S-expression parser	core/imo.c (parse_sexpr)	Low — isa has lexer/parser in lib/
Polybius/QuQuart frame	core/tetragrammatron.c	High — constitutional requirement
React portal UI	portal/	Keep separate — gate/ subdir
1.4 Canonical: omi-isa/ (~68 files, ~14K lines C + 2.9K JS + docs)
Module	Files	Lines	Status
CPU/VM	lib/cpu.c/h, lib/boot.c, lib/isa.h	~222	Keep
Compiler frontend	lib/lexer.c/h, lib/parser.c, lib/ast.c/h, lib/compiler.c, lib/asm.c	~254	Keep
Envelope layer	lib/omienv.c/h, lib/stream.c/h, lib/sector.c/h, lib/loader.c/h	~445	Keep
Dispatch	lib/omi_dispatch.c/h, lib/gauge_exec.c/h	~406	Keep
Transport	lib/omi_transport.c/h, omi_transport_sim.c/h, omi_transport_lora.c/h	~258	Keep
Mesh networking	lib/omi_mesh.c/h	~568	Keep
Omicron/OMIOM	lib/omicron.c/h, lib/omi_omion.c/h	~264	Keep
Receipts	lib/omi_receipt.c/h	~122	Keep
Sensors	lib/omi_sense.c/h	~242	Keep
PG(4,2) kernel	lib/omi_pg.c/h	~309	Keep
GL(16,2) orbit	lib/omi_orbit.c/h	~234	Keep
Probe protocol	lib/omi_probe.c/h	~168	Keep
Tests (11)	test/test_*.c	~2,389	Keep
WASM bridge	web/omi_web_bridge.c	116	Keep
Web UI	web/ (13 files)	~2,600	Keep
Firmware	firmware/ (7 files)	~760	Keep
Programs	programs/*.omi (3 files)	3	Keep
Build system	Makefile	179	Expand
Docs	docs/, *.md (16 files)	~3,554	Expand
2. Constraint Validation Matrix
Every module in the rebuilt protocol MUST satisfy these constraints from omi-axioms/ + omi-canon/:
2.1 Axiom Constraints (from omi-axioms/)
#	Constraint	Source Module	Affects
A1	delta_deterministic	AtomicKernel	All state transitions
A2	Fano plane valid (7pts, 7 lines, 3/line)	FanoIncidence02	omi_pg.c, tetragrammatron routing
A3	Golden field Q(√5) closed	GoldenField06	BQF, orbit metric
A4	Icosian units: 120, norm 1, closed under ×	IcosianUnits08	omi_orbit.c gauge mapping
A5	Bialgebra: projection commutes with transition	OMI_bialgebra	All envelope dispatch
A6	Bisimulation = equivalence relation	coalgebraic_bisimulation	Mesh routing, probe
A7	Orbit period divides 8	delta_orbit_theory	omi_orbit.c
A8	Bitmask closure terminates ≤7 steps	BitmaskClosure03	Gauge bands, address masks
A9	Truth-table equivalence decidable	KarnaughReduction04	Decision tables
A10	Weyl reflection involutive, norm-preserving	WeylReflection12	E8 roots, geometry
A11	Hopf map S³→S² verified	HopfProjection13	Projection pipeline
A12	P0/P1 only in deterministic core	ProofStatus00	All runtime code
A13	d² = 0 (boundary composition)	PROOF_CANONIZATION.md	Validation pipeline
A14	No zero fixed point for delta	delta_orbit_theory	omi_orbit.c
2.2 Canon Constraints (from omi-canon/)
#	Constraint	Source Doc	Affects
C1	128-bit frame only	01-ONTOLOGY	omienv.h (currently 512-bit!)
C2	Omicron anchors: S1=0x03BF, S6=0x039F	03-FOUNDATIONS	Envelope parsing
C3	Address = identity (no hashes, no signatures)	10-BLACKBOARD	Receipts, probe
C4	Delta law is the clock (period 8)	01-ONTOLOGY	cpu.c step()
C5	Five truth gates: RULES/FACTS/CLOSURES/COMBINATORS/CONS	01-ONTOLOGY	Dispatch table
C6	Gauge interrupts selection, not state	18-GAUGE-INTERRUPTS	gauge_exec.c
C7	Projection is NOT authority	24-OBSERVER-CHANGELOG	All rendering code
C8	16 Wittgenstein truth-functions	09-OMI-LISP	Decision table impl
C9	OMI-Lisp declares, does not compute	09-OMI-LISP	Compiler frontend
C10	Slot5040 = fano7×720 + role3×240 + local240	03-FOUNDATIONS	Receipt ring
C11	448 proof cells (8Q×7A×4C×2P)	constitutional-grid/	Test suite
C12	Carrier prefix: FF 00 1C 1D 1E 1F 20 FF	11-NOTATION-MULTIPLEXING	loader.c
C13	Earned gauge bands (4 bands × 16 codes)	11-NOTATION-MULTIPLEXING	omi_dispatch.c
C14	Propagation as classification, not measurement	22-PROPAGATION-MODEL	omi_sense.c
C15	eBPF/XDP Gate 1 + Gate 2 at line rate	22-PROPAGATION-MODEL	Stream parser
Key finding: omi-isa uses a 512-bit envelope (64 bytes) but canon requires 128-bit (01-ONTOLOGY.md). This is a critical boundary violation that needs resolution. The 512-bit structure has gauge[8]+orientation[8]+recovery[8]+target[8]+relation[32]. Canon says the frame is exactly S0-S7 (8×16=128 bits). The extra 384 bits (relation[32] = 256 bits alone) exceed the constitutional frame size.
3. Proposed Production File Structure
omi-protocol/                   # REBUILT root
├── README.md                   # Top-level: what, why, how to build
├── LICENSE                     # MIT (same as omi-isa)
├── Makefile                    # Unified build (merged from isa + tetragrammatron)
├── _CoqProject                 # Coq project (delegates to omi-axioms/)
├── .github/workflows/ci.yml    # Expanded CI
├── .gitignore
│
├── _archive/                   # Historical reference, NEVER compiled
│   ├── omi-lisp/               # Full original tree from omi-lisp/
│   ├── omi-protocol/           # Full original tree from omi-protocol/
│   ├── tetragrammatron-proofs/ # omi-tetragrammatron/proof/ (superseded)
│   └── legacy-viewer/          # omi-tetragrammatron/dev-docs/archive/viewer/
│
├── lib/                        # Core C library (from omi-isa/lib/)
│   ├── isa.h                   # 32-opcode ISA (carry forward)
│   ├── cpu.c/h                 # 16-bit VM (must enforce C4: delta-as-clock)
│   ├── boot.c                  # ROM boot sequence
│   ├── lexer.c/h               # S-expression tokenizer
│   ├── parser.c                # Recursive-descent parser
│   ├── ast.c/h                 # Cons-cell AST
│   ├── compiler.c              # AST → bytecode
│   ├── asm.c                   # Instruction encoder
│   ├── omienv.c/h              # 128-bit envelope (was 512-bit — VIOLATES C1, fix needed)
│   ├── stream.c/h              # Byte-at-a-time parser
│   ├── sector.c/h              # Sector iterator
│   ├── loader.c/h              # Carrier strip (C12: FF 00 1C 1D 1E 1F 20 FF)
│   ├── omi_dispatch.c/h        # 32-slot dispatch (C5: truth gates)
│   ├── gauge_exec.c/h          # Gauge lambda engine (C6: selection, not state)
│   ├── omi_transport.c/h       # Abstract transport vtable
│   ├── omi_transport_sim.c/h   # Sim transport
│   ├── omi_transport_lora.c/h  # LoRa transport
│   ├── omi_mesh.c/h            # Mesh routing (A6: bisimulation)
│   ├── omicron.c/h             # Omicron resolution (C2: anchors)
│   ├── omi_omion.c/h           # Prefix resolver
│   ├── omi_receipt.c/h         # Receipts (C3: no hashes, C10: slot5040)
│   ├── omi_sense.c/h           # Sensor layer (C14: classification not measurement)
│   ├── omi_pg.c/h              # PG(4,2) incidence kernel (A2: Fano)
│   ├── omi_orbit.c/h           # GL(16,2) orbit engine (A7: period|8)
│   ├── omi_probe.c/h           # Probe protocol
│   │
│   ├── tetragrammatron.c/h     # MERGED: ring validation, folds (from tetragrammatron)
│   ├── metatron.c/h            # MERGED: 21-solid shape DB, 6 renderers (from tetragrammatron)
│   └── imo.c/h                 # MERGED: ring I/O, HTTP server (from tetragrammatron)
│
├── test/                       # Tests (from omi-isa/test/ + tetragrammatron)
│   ├── test_env.c
│   ├── test_dispatch.c
│   ├── test_gauge_exec.c
│   ├── test_radio_vm.c
│   ├── test_mesh.c
│   ├── test_omicron.c
│   ├── test_omion.c
│   ├── test_receipt.c
│   ├── test_omi_sense.c
│   ├── test_pg.c
│   ├── test_orbit.c
│   ├── test_metatron.c         # NEW (from tetragrammatron/test-metatron-scribe.c)
│   ├── test_ring.c             # NEW (from tetragrammatron/test-c-scaffold.c)
│   └── test_constraints.c      # NEW: validates all A1-A14 + C1-C15 at runtime
│
├── gate/                       # Surface projections (was tetragrammatron portal/)
│   ├── portal/                 # React/TS/Vite portal (unchanged from tetragrammatron)
│   └── web/                    # WASM bridge + JS tools (from omi-isa/web/)
│
├── firmware/                   # ESP32-S3 + LoRa (from omi-isa/firmware/)
│   └── ...
│
├── programs/                   # OMI-Lisp source programs
│   ├── init.omi
│   ├── kernel.omi
│   ├── test.omi
│   └── ...
│
├── scripts/                    # Build/codegen tools
│   ├── gen_bootstrap.py
│   └── validate_constraints.py # NEW: automated constraint violation checker
│
├── docs/                       # Documentation (curated, not dumped)
│   ├── README.md               # Implementation guide
│   ├── architecture/           # Maps to omi-isa/docs/architecture/
│   │   ├── overview.md
│   │   ├── isa-spec.md
│   │   ├── envelope-spec.md
│   │   ├── mesh-protocol.md
│   │   └── constraint-map.md   # NEW: maps every module to its axioms + canon constraints
│   ├── foundations/            # Maps to omi-canon/01-ONTOLOGY through 03-FOUNDATIONS
│   ├── mathematics/            # Maps to omi-axioms/
│   │   └── proof-lineage.md
│   ├── cosmology/
│   │   └── vision.md
│   └── implementation/
│
├── constraints/                # NEW: formal constraint artifacts
│   ├── axiom-registry.md       # All A1-A14 with module ownership
│   ├── canon-registry.md       # All C1-C15 with module ownership
│   ├── 448-grid/               # NEW: constitutional grid test cases
│   │   └── ...
│   └── violations/             # Known violations to fix
│       └── 001-512bit-envelope.md  # C1 violation: 512-bit > 128-bit frame
│
├── coq/                        # Coq proof bindings (delegation to omi-axioms/)
│   └── Makefile                # Delegates `make proof` to ../omi-axioms/
│
└── _opencode/                  # opencode agent configuration
    └── plans/
4. Build Target Map
Target	Source	Output	Status
omi_vm	main.c + lib/*.c	omi_vm	From isa, keep
omi_toolchain	toolchain_main.c + lib/*.c	omi_toolchain	From isa, keep
omicron	core/*.c from tetragrammatron	core/omicron.bin	Merge into omi_vm
wasm	web/omi_web_bridge.c	web/omi_wasm.js+.wasm	From isa, keep
test	All test/test_*.c	Run all	Expand
proof	Delegates to ../omi-axioms/	Coq .vo files	Delegate
portal	gate/portal/ npm build	gate/portal/dist/	Keep separate
firmware	firmware/ PlatformIO	ESP32-S3 binary	Keep
lint-constraints	NEW	Validates A1-A14, C1-C15	Add
5. Critical Fixes Required
Priority	Issue	Source	Fix
HIGH	C1 violation: 512-bit envelope > 128-bit canon	omi-isa/lib/omienv.h	Redesign envelope to 128-bit (8×16). Move relation/gauge data to sidecar or multiplex.
HIGH	No constitutional grid tests	Missing	Add 448-cell test matrix mapping each feature to Q×A×C×P cells
HIGH	tetragrammatron scribe/render pipeline missing from isa	omi-tetragrammatron/core/metatron.c	Merge 21-solid DB + 6 renderers into lib/metatron.c
MEDIUM	No runtime constraint enforcement	Missing	Add test_constraints.c and scripts/validate_constraints.py
MEDIUM	Ring receipt model differs between isa and tetragrammatron	Compare omi_orbit.c vs tetragrammatron.c	Unify on tetragrammatron's 5040-slot ring (closer to canon C10)
MEDIUM	Decision tables (C8, Wittgenstein) not implemented in isa	Missing	Add from omi-canon/09-OMI-LISP.md spec
LOW	Enterprise-sized PDFs (13-20MB) in dev-docs	omi-tetragrammatron/dev-docs/temp/	Archive only, not in build path
6. Integration Points with Sibling Projects
Consumer Project	Interface	Required From omi-protocol
o---o (canon)	Reads constraint-compliance reports	docs/architecture/constraint-map.md
omi---imo (graph machine)	Uses lib/lexer, lib/parser, lib/compiler	Stable API: .omi → bytecode
omi-portal (runtime)	Uses lib/omi_dispatch, lib/omi_mesh, lib/omi_orbit	C headers + WASM bindings
omi-canvas (Haskell pipeline)	Uses envelope → receipt → projection pipeline	lib/omienv.h, lib/omi_receipt.h
omnicron (boot logic)	Uses carrier strip + pre-header bootstrap	lib/loader.h
omi-vault (storage)	Uses receipt ring I/O	lib/omi_receipt.h, ring serialization
That's the complete audit and rebuild plan. You can save this to REBUILD.md at the repo root when you're ready to execute. I recommend starting with Phase 0 (archive creation) and the HIGH-priority fixes (C1 envelope size, missing constitutional tests, metatron merge).