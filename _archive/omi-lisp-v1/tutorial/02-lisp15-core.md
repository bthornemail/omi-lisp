# The Lisp 1.5 Core

Lisp 1.5 is close to the origin of Lisp. It is old enough that some names and
mechanics feel unfamiliar, but the central machine is still recognizable:

```text
S-expressions + eval/apply + functions + symbolic data
```

For your purposes, learn Lisp 1.5 as a model of how dot notation becomes a
programming language.

## S-Expressions

Lisp 1.5 is built around symbolic expressions, or S-expressions.

An S-expression is either:

```text
atom
pair of S-expressions
```

That definition is recursive. A pair can contain pairs, which can contain more
pairs. This is why a tiny notation can represent programs, trees, tables,
proofs, and control structures.

## Five Primitive Operations

The classic Lisp core is often taught through these operations:

```lisp
atom
eq
car
cdr
cons
```

Their roles:

| Operation | Question Or Action |
|-----------|---------------------|
| `atom` | Is this not a pair? |
| `eq` | Are these the same atom/object? |
| `car` | Get the left side of a pair. |
| `cdr` | Get the right side of a pair. |
| `cons` | Make a new pair. |

Everything interesting begins when these are combined with `quote`, `cond`,
and `lambda`.

## Eval And Apply

The deep Lisp idea is that programs are also list structure.

```lisp
(cons 'a '(b c))
```

is both:

```text
a list whose first element is the symbol CONS
a program asking the evaluator to call CONS
```

The evaluator roughly asks:

```text
Is the form an atom?
  Look up its value.

Is the form (quote x)?
  Return x unchanged.

Is the form (cond ...clauses...)?
  Test clauses in order.

Is the form (function arg1 arg2 ...)?
  Evaluate arguments, then apply function.
```

This is the moment where dot notation evolves into computation.

## Lisp 1.5 Function Forms

Lisp 1.5 used several categories that later Lisps reorganized.

| Lisp 1.5 Idea | Meaning |
|---------------|---------|
| `EXPR` | Interpreted function with evaluated arguments. |
| `FEXPR` | Function-like form that receives unevaluated arguments. |
| `SUBR` | Machine-coded function with evaluated arguments. |
| `FSUBR` | Machine-coded special form receiving unevaluated arguments. |
| `LAMBDA` | Anonymous function. |
| `LABEL` | Gives a function a name, often for recursion. |

Common Lisp no longer exposes this exact taxonomy in normal programming, but
the underlying distinction remains:

```text
ordinary functions evaluate arguments
special operators and macros control evaluation
```

## Cond

`cond` is the old Lisp conditional:

```lisp
(cond ((atom x) 'atomic)
      ((eq (car x) 'quote) 'quoted)
      ('t 'compound))
```

Each clause has a test and result. Lisp 1.5 style often used `T` as the final
always-true case.

## Lambda

A lambda expression describes a function:

```lisp
(lambda (x) (cons x x))
```

Applied to `a`, it returns:

```lisp
(a . a)
```

In modern Common Lisp:

```lisp
(funcall (lambda (x) (cons x x)) 'a)
```

## Association Lists

An association list stores bindings as pairs:

```lisp
((x . 10)
 (y . 20)
 (z . 30))
```

This is important because early Lisp evaluators used association lists to
represent environments:

```text
symbol -> value
```

For OMI, this is a useful pattern:

```text
control-name -> byte meaning
surface-token -> structural role
header-slot -> interpretation
```

## Property Lists

Lisp systems also used property lists: symbols could carry named properties.

Conceptually:

```lisp
symbol:
  pname -> printed name
  expr  -> function definition
  value -> current value
```

This is another bridge to OMI. You can treat a symbol not merely as text, but
as a node with attached metadata.

## What To Retain

For OMI work, keep these Lisp 1.5 lessons:

1. Dot notation is the concrete substrate.
2. The reader creates symbolic structure before evaluation.
3. Evaluation is a second phase layered on top of structure.
4. Environments can be represented as lists of dotted bindings.
5. A tiny set of operations can support a rich symbolic system.

