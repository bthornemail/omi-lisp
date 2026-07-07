# Lisp 1.5, Common Lisp, and OMI Dot Notation

This tutorial is a personal study path for learning how Lisp grew out of
dot-notation list structure, how Lisp 1.5 organized that structure into an
evaluator, and how Common Lisp preserves the same core ideas in a larger,
modern language.

The goal is not to memorize old syntax. The goal is to build a good mental
model:

```text
pair cells -> S-expressions -> eval/apply -> Common Lisp -> OMI notation lab
```

OMI is exploring a pre-Lisp control lattice. Lisp 1.5 is useful because it is
close enough to the origin that the dot is still visible everywhere: `cons`,
`car`, `cdr`, `quote`, `cond`, `lambda`, `label`, property lists, and the
evaluator all sit near raw list structure.

## Path

1. [Dot Notation And Cons Cells](01-dot-notation.md)
2. [The Lisp 1.5 Core](02-lisp15-core.md)
3. [The Common Lisp Bridge](03-common-lisp-bridge.md)
4. [OMI Dot Variant Lab](04-omi-dot-variant-lab.md)
5. [Exercises](exercises.lisp)

## How To Study

Use a notebook. For every expression, draw the cons boxes first, then write the
printed S-expression, then write what the evaluator would do.

Run the exercise file with SBCL:

```sh
sbcl --noinform --script tutorial/exercises.lisp
```

## Primary References

- John McCarthy et al., *LISP 1.5 Programmer's Manual*, MIT Press, 1962:
  https://mitpress.mit.edu/9780262130110/lisp-1-5-programmers-manual/
- Preserved scan of the *LISP 1.5 Programmer's Manual*:
  https://www.softwarepreservation.org/projects/LISP/book/LISP%201.5%20Programmers%20Manual.pdf/view
- Local OMI notation reference: [../OMI_NOTATION.md](../OMI_NOTATION.md)

