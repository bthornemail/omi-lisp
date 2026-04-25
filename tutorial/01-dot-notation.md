# Dot Notation And Cons Cells

Dot notation is the smallest visible form of Lisp structure.

```lisp
(A . B)
```

means:

```text
one pair cell
  car -> A
  cdr -> B
```

Historically and practically, this is the root. Lists are not primitive magic;
they are chains of pair cells.

## The Pair

In Common Lisp:

```lisp
(cons 'a 'b)  ; => (A . B)
```

`cons` creates a pair. `car` reads the left side. `cdr` reads the right side.

```lisp
(car '(a . b)) ; => A
(cdr '(a . b)) ; => B
```

The old names come from IBM 704 machine fields, but their useful modern meaning
is simple:

```text
car = left / first / head
cdr = right / rest / tail
```

## Proper Lists

A proper list is a chain of pairs ending in `nil`.

```lisp
(A B C)
```

is shorthand for:

```lisp
(A . (B . (C . NIL)))
```

Draw it:

```text
[ car | cdr ] -> [ car | cdr ] -> [ car | cdr ] -> NIL
   |              |              |
   A              B              C
```

This is the first big lesson: the friendly list notation is only a surface
syntax. The underlying structure is dotted pairs.

## Improper Lists

An improper list is a pair chain that does not end in `nil`.

```lisp
(A B . C)
```

means:

```lisp
(A . (B . C))
```

Draw it:

```text
[ car | cdr ] -> [ car | cdr ] -> C
   |              |
   A              B
```

This matters for your OMI variant because the final cdr can be a payload,
pointer, boundary marker, header, or other non-list object.

## Quote

Without quote, Lisp tries to evaluate an expression.

```lisp
(a . b)
```

is read as data, but if sent to the evaluator as a form, it is not a normal
function call. To talk about the pair itself, quote it:

```lisp
'(a . b)
```

which is shorthand for:

```lisp
(quote (a . b))
```

This distinction is central:

```text
reader builds structure
evaluator gives structure meaning
```

OMI's pre-header idea lives even earlier than this: before ordinary evaluation,
the stream is already being heard as control structure.

## Dot As A Turn

In ordinary Lisp, dot separates the two halves of one pair:

```text
left . right
```

In OMI language, the dot is a structural turning point. It says:

```text
the left side and right side are joined by an interpretation boundary
```

For plain Lisp, the boundary means `cons`. For OMI, you can experiment with
other meanings, but you should always be able to map back to a pair:

```text
OMI variant meaning -> canonical dotted-pair meaning
```

That gives your notation a stable skeleton.

## Practice

Write the full dotted form for each expression:

```lisp
(a b c)
(a (b c) d)
((a . b) . c)
(null . esc)
(!null . (((null . esc) (control . signal)) . aegean))
```

Then draw each as boxes.

