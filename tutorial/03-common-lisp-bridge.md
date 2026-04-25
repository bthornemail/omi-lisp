# The Common Lisp Bridge

Common Lisp is much larger than Lisp 1.5, but the ancestral core is still
there. You can use Common Lisp as your laboratory while keeping Lisp 1.5 as the
historical map.

## Same Bones, Larger Body

| Lisp 1.5 / Early Lisp | Common Lisp |
|-----------------------|-------------|
| S-expression | S-expression |
| Atom | Atom |
| Cons cell | Cons cell |
| `car`, `cdr`, `cons` | `car`, `cdr`, `cons` |
| `quote` | `quote`, `'` |
| `cond` | `cond` |
| `lambda` | `lambda` |
| Association list | Association list |
| Property list | Property list |
| Special forms / FEXPR-like control | Special operators and macros |

The continuity is real. Common Lisp did not replace the pair model; it built a
full programming environment around it.

## Important Differences

### One Namespace Or Two

Common Lisp has separate namespaces for values and functions:

```lisp
(setq cons 'my-symbol)
(cons 'a 'b)
```

The variable named `cons` and the function named `cons` do not collide.

Scheme is a Lisp-1: one main namespace for functions and values. Common Lisp is
often called a Lisp-2.

Lisp 1.5 does not map perfectly onto this modern terminology, but when learning
Common Lisp, remember:

```text
symbol value cell and symbol function cell are distinct
```

### NIL Is Both False And Empty List

In Common Lisp:

```lisp
nil
'()
```

are the same object.

`nil` also means false. Any non-`nil` value counts as true.

### Macros Are The Modern Syntax Tool

If you want your own notation or control form in Common Lisp, macros are often
the right tool.

Example:

```lisp
(defmacro when-atom (x &body body)
  `(when (atom ,x)
     ,@body))
```

Macros receive code as data, transform it, and return new code. This is the
modern, disciplined descendant of the old idea that program structure is list
structure.

## Reader Versus Macro

Common Lisp has two different extension levels:

```text
reader macro -> changes how characters become Lisp objects
ordinary macro -> changes how Lisp objects become evaluated code
```

For an OMI dot variant, this distinction is crucial.

Use an ordinary macro when your input is already valid Lisp:

```lisp
(omi-pair null esc)
```

Use a reader macro only when you want new surface syntax:

```text
null . esc
```

Reader macros are powerful, but they change the language front door. Learn the
ordinary structure first.

## Recommended Common Lisp Practice

Start SBCL:

```sh
sbcl
```

Try:

```lisp
(cons 'a 'b)
(cons 'a (cons 'b nil))
'(a . (b . nil))
(equal '(a b) '(a . (b . nil)))
(car '((null . esc) . aegean))
(cdr '((null . esc) . aegean))
```

Then define a small translation function:

```lisp
(defun pair-role (pair)
  (cond ((atom pair) :atom)
        ((eq (car pair) 'null) :null-left)
        ((eq (cdr pair) 'esc) :esc-right)
        (t :ordinary-pair)))
```

This is a tiny version of an OMI reader idea: inspect pair shape, then attach
meaning.

## Common Lisp Names Worth Learning Early

| Name | Why It Matters |
|------|----------------|
| `defun` | Define a function. |
| `let` | Local bindings. |
| `labels` | Local recursive functions. |
| `destructuring-bind` | Pull apart list shapes. |
| `typecase` | Branch on object type. |
| `loop` | Practical iteration. |
| `mapcar` | Transform lists. |
| `read` | Read Lisp objects from text/streams. |
| `print`, `prin1`, `format` | Output. |
| `macroexpand-1` | See what a macro produces. |

## Learning Rule

When Common Lisp feels large, reduce the problem back to three questions:

```text
What object did the reader build?
What does the evaluator do with it?
What pair/list shape is underneath?
```

