# OMI-Lisp

Pre-Lisp Lisp: control lattice before eval/apply. Your octuple precision kernel.

## Getting Started

### 1. Quick Test (5 seconds)

```sh
sbcl --noinform --non-interactive --load omi_vm_loader.lisp --eval '(format t "~S~%" (omi-vm-self-test))'
```

### 2. Practice Tool

Open `simulator.html` in a browser:
- Assembly editor with step-through (RV64I)
- Logic gate drawing (AND, OR, XOR)
- List box diagrams (your dot notation)
- 4-bit carry-lookahead adder
- Karnaugh map

### 3. Build RISC-V VM

```sh
chmod +x build_omi_riscv.sh
./build_omi_riscv.sh
./build-riscv/run-omi-riscv-vm.sh
```

### 4. Declarative Dev Environment

```sh
guix shell --manifest=omi-dev-manifest.scm
```

Then cross-compile:
```sh
guix shell --manifest=omi-dev-manifest.scm -- riscv64-linux-gnu-gcc -std=c99 -O2 -static omi_riscv_vm.c -o init
```

## Architecture

```
omi-lisp kernel
  → Layer 7: Application (your code)
  → Layer 6: Presentation (YOUR octuple: Braille/Aegean/BOM)
  → Layer 5: Session (auth)
  → Layer 4: Transport (TCP/UDP)
  → Layer 3: Network (IP)
  → Layer 2: Data Link (MAC)
  → Layer 1: Physical (RISC-V + QEMU)
```

## Your Octuple Precision Encoding

| Plane | Range | Purpose |
|-------|-------|---------|
| Braille | 0x80-0xBF | 236-bit significand |
| Aegean | 0xC0-0xFF | 19-bit exponent |
| BOM | NULL/DEL | Sign + 4×16-bit channels |

## Key Files

| File | Purpose |
|------|---------|
| `omi_vm_loader.lisp` | Host test loader |
| `omi_riscv_vm.c` | RISC-V PID-1 runtime |
| `omi-riscv-braille-table.lisp` | Braille lookup |
| `omi-aegean-table.lisp` | Aegean lookup |
| `omi-bom-table.lisp` | BOM lookup |
| `omi-dev-manifest.scm` | Guix declarative env |
| `simulator.html` | Practice tool |
| `dev-docs/` | Full documentation |

## Documentation

See `dev-docs/`:
- `99-build/bootable-runtime.md` - Build from scratch
- `01-physical/risc-v-isa.md` - RISC-V ISA complete
- `01-physical/rv128-scaling.md` - Scaling 32→64→128→256-bit
- `06-presentation/osi-model-why.md` - Why OSI exists
- `99-build/gdb-usage.md` - GDB debugging

## Project Structure

```
omi-lisp/
├── omi_vm_loader.lisp      # SBCL test loader
├── omi_riscv_vm.c          # RISC-V kernel (C)
├── omi-braille-table.lisp  # Braille 0x80-0xBF
├── omi-aegean-table.lisp   # Aegean 0xC0-0xFF
├── omi-bom-table.lisp      # BOM NULL/DEL
├── omi-dev-manifest.scm    # Guix env
├── build_omi_riscv.sh     # RISC-V build
├── simulator.html          # Practice tool
├── AGENTS.md              # Agent instructions
└── dev-docs/              # Full docs
    ├── 01-physical/        # Hardware, RISC-V
    ├── 06-presentation/    # OSI model, QEMU
    └── 99-build/          # Build system
```

## Build Commands

| Command | Purpose |
|---------|---------|
| `sbcl --load omi_vm_loader.lisp` | Host test |
| `guix shell --manifest=omi-dev-manifest.scm` | Dev environment |
| `riscv64-linux-gnu-gcc -c99 -O2 -static omi_riscv_vm.c -o init` | Cross-compile |
| `./build_omi_riscv.sh` | Build VM image |
| `./build-riscv/run-omi-riscv-vm.sh` | Run VM |

## OSI Model Layers

| Layer | Your Components |
|-------|------------------|
| 7: Application | Your kernel code |
| 6: Presentation | Braille/Aegean/BOM (octuple) |
| 5: Session | Auth, TLS |
| 4: Transport | TCP/UDP |
| 3: Network | IP |
| 2: Data Link | MAC |
| 1: Physical | RISC-V + QEMU |

## Learn More

- RISC-V ISA: `dev-docs/01-physical/risc-v-isa.md`
- QEMU Internals: `dev-docs/06-presentation/how-it-connects.md`
- Octuple Precision: `dev-docs/06-presentation/octuple-precision.md`