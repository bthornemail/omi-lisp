;;;; Hands-on exercises for tutorial/*.md.
;;;; Run with:
;;;;   sbcl --noinform --script tutorial/exercises.lisp

(defparameter *omi-inversions*
  '((null . esc)
    (control . signal)
    (references . pointers)
    (non-printing . printing)
    (ascii . unicode)
    (unicode . braille)))

(defparameter *omi-pre-header-ladder*
  '(!null
    .
    (((null . esc)
      (control . signal)
      (references . pointers)
      (non-printing . printing)
      (ascii . unicode)
      (unicode . braille))
     . aegean)))

(defun proper-list-p (x)
  "Return true when X is a NIL-terminated list."
  (cond ((null x) t)
        ((consp x) (proper-list-p (cdr x)))
        (t nil)))

(defun dotted-pair-p (x)
  "Return true when X is a cons whose CDR is not a proper list."
  (and (consp x)
       (not (proper-list-p x))))

(defun omi-atom-p (x)
  "Tutorial predicate: an OMI atom is any non-cons object."
  (atom x))

(defun inversion-p (x)
  "Return true when X is one of the known OMI inversion pairs."
  (and (consp x)
       (not (proper-list-p x))
       (member x *omi-inversions* :test #'equal)))

(defun lower-omi-pair (x)
  "Lower one OMI pair into a tagged shape."
  (cond ((inversion-p x)
         (list :inversion (car x) (cdr x)))
        ((consp x)
         (list :pair (car x) (cdr x)))
        (t
         (list :atom x))))

(defun lower-tree (x)
  "Recursively lower a tree while preserving proper-list grouping."
  (cond ((inversion-p x)
         (list :inversion (car x) (cdr x)))
        ((consp x)
         (cons (lower-tree (car x))
               (lower-tree (cdr x))))
        (t x)))

(defun show (label value)
  (format t "~&~A~%  ~S~2%" label value))

(show "Dotted pair" '(a . b))
(show "Proper list as dotted structure" '(a . (b . (c . nil))))
(show "Improper list" '(a b . c))
(show "Known inversion" (lower-omi-pair '(null . esc)))
(show "Ordinary pair" (lower-omi-pair '(left . right)))
(show "Pre-header ladder" *omi-pre-header-ladder*)
(show "Lowered tree" (lower-tree *omi-pre-header-ladder*))

