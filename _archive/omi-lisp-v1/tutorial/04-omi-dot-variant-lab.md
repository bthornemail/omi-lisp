# OMI Dot Variant Lab

This lab is where Lisp history becomes your notation experiment.

The rule for this folder:

```text
invent freely, but always keep a translation back to ordinary pairs
```

That keeps the notation grounded.

## Baseline

Ordinary Lisp:

```lisp
(NULL . ESC)
```

OMI reading:

```text
NULL and ESC form a control inversion.
The dot is the turn where the left side is heard through the right side.
```

Canonical pair translation:

```lisp
(cons 'null 'esc)
```

## Design A Dot Variant

Before inventing syntax, answer these questions.

### 1. What Does The Dot Mean?

Pick one meaning for the first version:

| Dot Meaning | Example Interpretation |
|-------------|------------------------|
| Pair | `(A . B)` means cons cell. |
| Inversion | `(LOW . HIGH)` means polarity pair. |
| Boundary | `(LEFT . RIGHT)` means phase transition. |
| Binding | `(NAME . VALUE)` means environment entry. |
| Pointer | `(SOURCE . TARGET)` means reference edge. |
| Header slot | `(SLOT . BYTE)` means control injection. |

Start with one. Later versions can layer meanings by context.

### 2. What Counts As An Atom?

In Lisp, atoms include symbols, numbers, strings, and `nil`.

For OMI, atoms may include:

```text
ASCII control names
byte values
Braille cells
Aegean cells
header slot names
state names
ordinary Lisp symbols
```

Write down the atom set before extending the reader.

### 3. What Is The Canonical Form?

Every variant expression should lower into one ordinary Lisp object.

Example:

```text
surface:   NULL . ESC
canonical: (NULL . ESC)
lowered:   (:inversion NULL ESC)
runtime:   header slot update or control transition
```

Separate these layers:

```text
surface syntax
canonical pair structure
semantic tag
runtime action
```

## Suggested OMI Data Shapes

### Inversion

```lisp
(:inversion null esc)
```

Pair source:

```lisp
(null . esc)
```

### Boundary

```lisp
(:boundary left right)
```

Pair source:

```lisp
(left . right)
```

### Header Slot

```lisp
(:header8 0 null)
(:header8 1 esc)
(:header8 6 input-byte)
(:header8 7 current-state)
```

Pair source:

```lisp
(0 . null)
(1 . esc)
(6 . input-byte)
(7 . current-state)
```

### Control Ladder

Source:

```lisp
(!null
  .
  (((null . esc)
    (control . signal)
    (references . pointers)
    (non-printing . printing)
    (ascii . unicode)
    (unicode . braille))
   . aegean))
```

Possible lowered form:

```lisp
(:ladder
  (:entry !null)
  (:steps ((:inversion null esc)
           (:inversion control signal)
           (:inversion references pointers)
           (:inversion non-printing printing)
           (:inversion ascii unicode)
           (:inversion unicode braille)))
  (:closure aegean))
```

## Build The Variant In Three Passes

### Pass 1: Data Only

Use normal quoted Lisp data. No custom reader yet.

```lisp
'(null . esc)
```

Write functions that classify and lower this data.

### Pass 2: Macros

Use ordinary Common Lisp syntax to make authoring easier.

```lisp
(defmacro inversion (left right)
  `(:inversion ',left ',right))
```

Then:

```lisp
(inversion null esc)
```

### Pass 3: Reader Experiment

Only after the model is stable, consider reader macros. Reader macros should
translate surface notation into the same canonical forms from Pass 1.

This protects the design. If the reader experiment fails, the data model still
works.

## Exercises

1. Define `omi-atom-p`.
2. Define `proper-list-p`.
3. Define `dotted-pair-p`.
4. Define `inversion-p` for pairs like `(null . esc)`.
5. Define `lower-omi-pair` that returns `(:inversion left right)` for known
   inversion pairs and `(:pair left right)` otherwise.
6. Lower the full pre-header ladder from [../OMI_NOTATION.md](../OMI_NOTATION.md).

## Design Rule

Do not let a surface mark carry unlimited meaning.

Instead:

```text
dot creates a relation
context classifies the relation
lowering gives it a stable runtime shape
```

That is the clean path from Lisp dot notation to an OMI variant.

