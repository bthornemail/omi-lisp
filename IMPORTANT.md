# IMPORTANT: BOM as Variable-Declaration Bit-Mask

## The BOM Has Two Jobs

The BOM (Byte Order Marker) isn't just for byte order. It serves two roles in the HEADER8 interface:

| Role | BOM | Purpose |
|------|-----|---------|
| **Byte Order** | 0x00 NULL | Marks the start of a control frame |
| **Variable Declarator** | 0x7F DEL | Declares next N bits as typed bit-field |

## BOM as Bit-Mask for HEADER8 Variables

When HEADER8 declares variables, the BOM acts as a bit-mask prefix:

```
┌─────────────────────────────────────────────────────────────────┐
│  HEADER8 Variable Declaration                                   │
│                                                                 │
│  BOM (bit-mask) → Variable Type → Bit Layout                   │
│  ─────────────────────────────────────────────────────────────  │
│                                                                 │
│  NULL (0x00)  → Cardinality  → Count bits (exponent width)       │
│  DEL (0x7F)   → Chirality    → Orientation bits (sign)          │
│  [???]        → Ordinality    → Sequence bits (index)            │
│                                                                 │
│  Example:                                                       │
│    0x00 0xC3 0x80 → NULL marker, Aegean exponent, Braille      │
│                                                                 │
│    The 0x00 BOM tells decoder:                                  │
│    "The following byte is an EXPONENT in Cardinality context"  │
└─────────────────────────────────────────────────────────────────┘
```

## Three BOM Types = Three Variable Types

Each BOM byte declares a different variable category:

| BOM | Hex | Bit-Mask | Declares |
|-----|-----|----------|----------|
| **NULL** | 0x00 | `00000000` | Cardinality variables (count, exponent) |
| **DEL** | 0x7F | `01111111` | Chirality variables (orientation, sign) |
| **?** | ? | ? | Ordinality variables (sequence, index) |

## HEADER8 Slot Structure (Tesseract = 4D Hypercube)

The 16 vertices of a tesseract map to HEADER8's 16-byte variable space:

```
Tesseract (4D Hypercube) → HEADER8 Variable Space
─────────────────────────────────────────────────
16 vertices = 16 possible variable slots
8 edges per vertex (4D) = 8-byte HEADER8 structure
4 cells per edge = 4 control planes (card/chir/ord/???)

Configuration Matrix: [16 4 6 4 2 32 3 3 4 4 24 2 8 12 6 8]
                      ↓  ↓ ↓ ↓ ↓ ↓  ↓ ↓ ↓  ↓  ↓  ↓  ↓
                      V  E F C ... edges=32, faces=24, cells=8
```

## Cyclic Sieving and Hypercube Symmetry

The cyclic sieving phenomenon connects tesseract rotations to counting fixed points:

```
C_n acts on words, tableaux, permutations by rotation
f(ω_d) = number of objects fixed by d rotations

Tesseract has 384 rotational symmetries (B4 group)
Each symmetry = permutation of the 16 vertices

HEADER8: Each vertex position = potential variable slot
         Cyclic sieving = tracking which slots are active
```

### q-Analogue Connection

```c
// q-analogue for HEADER8 variable counting
f(q) = Σ (variable_count) * q^index

// Evaluating at roots of unity:
// f(ω) = count of variables in specific cyclic state
// ω^n = 1 → back to original state
```

## Why 16 Vertices for 256-bit Octuple?

```
256 bits = 4 × 64-bit channels
         = 16 × 16-bit sub-channels
         = 8 × 32-bit words
         = 1 tesseract (4D hypercube) vertex count

The tesseract's 16 vertices perfectly encode:
  - 16 possible BOM type combinations
  - 16 distinct variable declarations
  - 16 slots in the control lattice
```

## Bit-Mask Declaration with Cyclic Action

```c
// HEADER8 declares variables using BOM as type prefix
// C_n rotation acts on the variable lattice

// NULL (0x00) declares: next field is exponent (cardinality)
uint8_t bom_type = 0x00;  // cardinality marker
uint8_t exponent = 0xC3;  // Aegean value

// DEL (0x7F) declares: next field is sign (chirality)  
uint8_t bom_sign = 0x7F;  // chirality marker
uint8_t sign = 0x01;      // orientation bit

// [???] declares: next field is index (ordinality)
uint8_t bom_index = 0x??;  // ordinality marker
uint32_t index = 0x00000012; // position in sequence

// Cyclic sieving on the lattice:
// After n rotations, variables return to original positions
// f(ω^n) counts fixed variables at rotation n
```

## Tesseract Projection → Octuple Visualization

```
Vertex-first projection (rhombic dodecahedron):
  Layer counts: 1 4 6 4 1  (4th row of Pascal's triangle)

This maps to HEADER8 as:
  Byte 0:    1 vertex (BOM type)
  Bytes 1-4: 4 vertices (exponent channels)
  Bytes 5-8: 6 vertices (control planes)
  Bytes 9-12: 4 vertices (significand parts)
```

## Files That Use This

- `omi-bom-table.lisp` - defines BOM as bit-mask constants
- `omi-aegean-table.lisp` - Aegean exponent bit patterns
- `omi-braille-table.lisp` - Braille significand bit patterns
- `omi-riscv_vm.c` - parses BOM-prefixed headers
- `simulator.html` - hypercube visualization practice

## Reference

See also:
- `OMI_NOTATION.md` - pre-header ladder
- `dev-docs/01-physical/rv128-scaling.md` - octuple precision
- `dev-docs/06-presentation/osi-model-why.md` - encoding philosophy