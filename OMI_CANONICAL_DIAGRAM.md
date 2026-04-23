# OMI Canonical Dot-Diagram Model

Every functional component can be modeled as a subdiagram of one canonical
dot-notation graph:

```lisp
(NODE . EDGES)
```

This is the same shape as a cons cell:

```text
car = local value / operator / head
cdr = continuation / output / tail
```

A Lisp box diagram and a logic circuit diagram are therefore two renderings of
the same substrate:

```text
cons cell:      (car . cdr)
logic gate:     (inputs . output)
rewrite rule:   (pattern . body)
closure:        (environment . body)
f-expression:   (raw-form . interpreter)
wire junction:  (source . fanout)
```

The dot is the join. It is not only punctuation. It is the place where a
reference crosses from one side of the diagram to the other.

## Canonical Node

```lisp
(node
  . ((id . <symbol>)
     (kind . <kind>)
     (in . (<edge>*))
     (out . (<edge>*))
     (body . <dot-expression>)))
```

Kinds:

```text
ATOM      literal symbol or byte
CONS      car/cdr pair
WIRE      named signal
JUNCTION  shared reference / fanout
AND       logic conjunction
OR        logic disjunction
XOR       logic exclusive-or
RULE      Datalog/rewrite clause
CLOSURE   environment plus body
Y         fixed-point recursion
Z         applicative-order fixed-point recursion
FEXPR     unevaluated form interpreter
```

## Canonical Logic Gates

The three gates in the 4-bit carry-lookahead adder are just dot diagrams:

```lisp
(AND . (a b . out))
(OR  . (a b . out))
(XOR . (a b . out))
```

As rewrite forms:

```lisp
((AND (? a) (? b)) . (? out))
((OR  (? a) (? b)) . (? out))
((XOR (? a) (? b)) . (? out))
```

As structural circuit nodes:

```lisp
((in . (a . b)) . (out . g))
```

The same node can be read as:

```text
logic:       g = a AND b
datalog:     and(a,b,g)
s-expression:(AND A B)
f-expression: raw gate form interpreted by current control scope
box diagram: car points to input bundle, cdr points to output wire
```

## 4-Bit Carry-Lookahead Adder

The file
`/home/main/Downloads/3D/Four_bit_adder_with_carry_lookahead.txt` is an SVG
logic diagram with inputs `A0..A3`, `B0..B3`, `C0`; internal `P0..P3`,
`G0..G3`, `C1..C3`; outputs `S0..S3`, `C4`.

The canonical equations are:

```text
Pi = Ai XOR Bi
Gi = Ai AND Bi

C1 = G0 OR (P0 AND C0)
C2 = G1 OR (P1 AND G0) OR (P1 AND P0 AND C0)
C3 = G2 OR (P2 AND G1) OR (P2 AND P1 AND G0) OR (P2 AND P1 AND P0 AND C0)
C4 = G3 OR (P3 AND G2) OR (P3 AND P2 AND G1) OR (P3 AND P2 AND P1 AND G0)
     OR (P3 AND P2 AND P1 AND P0 AND C0)

Si = Pi XOR Ci
```

In OMI dot notation:

```lisp
(ADDER4
  .
  ((inputs . (A0 B0 A1 B1 A2 B2 A3 B3 C0))
   (propagate
    . ((P0 . (XOR A0 B0))
       (P1 . (XOR A1 B1))
       (P2 . (XOR A2 B2))
       (P3 . (XOR A3 B3))))
   (generate
    . ((G0 . (AND A0 B0))
       (G1 . (AND A1 B1))
       (G2 . (AND A2 B2))
       (G3 . (AND A3 B3))))
   (carry
    . ((C1 . (OR G0 (AND P0 C0)))
       (C2 . (OR G1 (AND P1 G0) (AND P1 P0 C0)))
       (C3 . (OR G2 (AND P2 G1) (AND P2 P1 G0) (AND P2 P1 P0 C0)))
       (C4 . (OR G3 (AND P3 G2) (AND P3 P2 G1)
                 (AND P3 P2 P1 G0) (AND P3 P2 P1 P0 C0)))))
   (sum
    . ((S0 . (XOR P0 C0))
       (S1 . (XOR P1 C1))
       (S2 . (XOR P2 C2))
       (S3 . (XOR P3 C3))))
   (outputs . (S0 S1 S2 S3 C4))))
```

## Carry-Lookahead As Cons Boxes

Each equation is a box-and-arrow object:

```text
 P0 cell:
 ---------------       ----------------
| car   | cdr   |     | car    | cdr   |
| XOR   |   o-------->| A0     |   o--------> B0
|       |       |     |        |       |
 ---------------       ----------------
```

The output binding is another cons:

```lisp
(P0 . (XOR A0 B0))
```

So the circuit does not need a separate representation from Lisp structure.
The wire named `P0` is the `car`; the subdiagram `(XOR A0 B0)` is the `cdr`.

## Component = Subdiagram

A component is any closed subgraph with named inputs and outputs:

```lisp
(component
  . ((name . <name>)
     (inputs . (...))
     (outputs . (...))
     (diagram . <dot-graph>)))
```

Examples:

```lisp
(component
  . ((name . HALF-ADDER)
     (inputs . (A B))
     (outputs . (S G))
     (diagram
      . ((S . (XOR A B))
         (G . (AND A B))))))
```

```lisp
(component
  . ((name . CLA-BIT)
     (inputs . (A B C))
     (outputs . (P G S))
     (diagram
      . ((P . (XOR A B))
         (G . (AND A B))
         (S . (XOR P C))))))
```

The 4-bit adder is then a component made of four `CLA-BIT` subdiagrams plus a
carry-lookahead subdiagram.

## Why This Fits OMI

OMI already treats:

```text
byte stream -> HEADER8 -> scope -> match -> rewrite -> emit
```

A circuit diagram is the same thing with a different surface:

```text
signal stream -> gate cells -> wire scope -> match -> propagate -> output
```

The canonical bridge is:

```text
wire = reference
junction = shared cons cell
gate = f-expression over raw inputs
carry equation = Datalog clause / rewrite rule
feedback/fixpoint = Y/Z/closure
```

Therefore the adder SVG, Karnaugh torus, graph colorings, and QPBO diagrams can
all be normalized into one dot-notation graph model. The renderer may differ;
the substrate is the same.

