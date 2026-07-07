;;; omi_canonical_diagram.lisp
;;;
;;; Canonical OMI dot-diagram model.
;;; This is data, not eval/apply. The adder is represented as alists and
;;; S-expressions that can be consumed by the rewrite layer.

(defparameter *omi-gate-kinds*
  '(AND OR XOR))

(defparameter *omi-adder4-dot-diagram*
  '(ADDER4
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
     (outputs . (S0 S1 S2 S3 C4)))))

(defparameter *omi-half-adder-component*
  '(component
    . ((name . HALF-ADDER)
       (inputs . (A B))
       (outputs . (S G))
       (diagram
        . ((S . (XOR A B))
           (G . (AND A B)))))))

(defparameter *omi-cla-bit-component*
  '(component
    . ((name . CLA-BIT)
       (inputs . (A B C))
       (outputs . (P G S))
       (diagram
        . ((P . (XOR A B))
           (G . (AND A B))
           (S . (XOR P C)))))))

(defun omi-diagram ()
  *omi-adder4-dot-diagram*)

