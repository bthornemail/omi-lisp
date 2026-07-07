# AGENTS.md

## Quick Test

```sh
sbcl --noinform --non-interactive --load omi_vm_loader.lisp --eval '(format t "~S~%" (omi-vm-self-test))'
```

## Build VM (RISC-V)

```sh
chmod +x build_omi_riscv.sh
./build_omi_riscv.sh
./build-riscv/run-omi-riscv-vm.sh
```

## Declarative Dev Environment (Guix)

```sh
guix shell --manifest=omi-dev-manifest.scm
```

Then cross-compile:
```sh
guix shell --manifest=omi-dev-manifest.scm -- riscv64-linux-gnu-gcc -std=c99 -O2 -static omi_riscv_vm.c -o init
```

## OSI Model

The OSI model = how your kernel is structured. Each layer has ONE job.

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

## Key Docs

- `dev-docs/99-build/bootable-runtime.md` - How to build from scratch
- `dev-docs/06-presentation/osi-model-why.md` - Why OSI exists
- `dev-docs/06-presentation/how-it-connects.md` - How everything works
- `dev-docs/01-physical/risc-v-isa.md` - Complete RISC-V reference
- `dev-docs/01-physical/rv128-scaling.md` - Scaling 32→64→128→256-bit
- `dev-docs/99-build/gdb-usage.md` - GDB debugging

## Practice Tool

Open `simulator.html` in a browser for:
- Assembly editor with step-through
- Logic gate drawing (AND, OR, XOR)
- List box diagrams (your dot notation)
- 4-bit carry-lookahead adder
- Karnaugh map

## Key Files

- `omi_vm_loader.lisp`: Host test loader (byte -> HEADER8 -> rewrite -> emit)
- `omi_riscv_vm.c`: RISC-V PID-1 runtime with control plane
- `omi-dev-manifest.scm`: Guix declarative env
- `omi-braille-table.lisp`: Braille lookup tables
- `omi-aegean-table.lisp`: Aegean lookup tables
- `omi-bom-table.lisp`: BOM lookup tables

## Architecture

- **OMI**: Pre-Lisp Lisp - control lattice before eval/apply
- **HEADER8**: 8-byte control injection surface (slots 0-7)
- **Control plane**: BOM (0xEF), ESC (0x1B), BOUNDARY (0x1C-0x1F), ECC (0xE0-0xEE)
- **Surface planes**: ASCII (0x00-0x7F), Braille (0x80-0xBF), Aegean (0xC0-0xFF)