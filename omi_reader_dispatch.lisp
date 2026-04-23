;;; omi_reader_dispatch.lisp
;;;
;;; OMI-LISP READER / DISPATCH BRIDGE
;;; Brian Thorne
;;;
;;; Purpose:
;;;   Bridge codepoint planes (ASCII / AEGEAN / BRAILLE) into the
;;;   dotted Lisp 1.5-style evaluator core.
;;;
;;; Canonical flow:
;;;   surface codepoint
;;;      -> (PLANE . OFFSET)
;;;      -> plane closure lookup
;;;      -> canonical token
;;;      -> reader state
;;;      -> evaluator form
;;;
;;; This file assumes the presence of:
;;;   *planes*              ; from omi_planes.lisp
;;;   plane-lookup
;;;   close-codepoint
;;;   EVAL / APPLY core     ; from omi_lisp15_dot.lisp
;;;
;;; ----------------------------------------------------------------------
;;; 0. CANONICAL TOKEN SHAPE
;;; ----------------------------------------------------------------------
;;;
;;; A canonical token is an alist-like record:
;;;
;;;   ((TYPE  . <kind>)
;;;    (PLANE . <plane>)
;;;    (LOW   . <offset>)
;;;    (VALUE . <payload>))
;;;
;;; TYPE is one of:
;;;   CONTROL | GLYPH | DELIMITER | ATOM | NUMBER | UNKNOWN

(OMI-TOKEN .
 (LAMBDA . ((TYPE . (PLANE . (LOW . (VALUE . NIL)))) .
   ((CONS . ((CONS . (TYPE . (TYPE . NIL)))
           . ((CONS . (PLANE . (PLANE . NIL)))
           . ((CONS . (LOW . (LOW . NIL)))
           . ((CONS . (VALUE . (VALUE . NIL)))
           . (NIL . NIL))))))
    . NIL))))

;;; ----------------------------------------------------------------------
;;; 1. SMALL ALIST HELPERS
;;; ----------------------------------------------------------------------

(ALIST-GET .
 (LAMBDA . ((KEY . (ALIST . NIL)) .
   ((COND .
      ((((NULLP . (ALIST . NIL)) . NIL) . (NIL . NIL))
       ((((EQ . (KEY . (CAAR . (ALIST . NIL)))) . NIL)
         . ((CDAR . (ALIST . NIL)) . NIL))
        ((T . ((ALIST-GET . (KEY . ((CDR . (ALIST . NIL)) . NIL))) . NIL)) . NIL)))
    . NIL))))

(MAKE-TOKEN .
 (LAMBDA . ((TYPE . (PLANE . (LOW . (VALUE . NIL)))) .
   ((LIST4 . (TYPE . (PLANE . (LOW . (VALUE . NIL))))) . NIL))))

;;; LIST4 built from LIST3/LIST2 if present; otherwise define it here.
(LIST4 .
 (LAMBDA . ((A . (B . (C . (D . NIL)))) .
   ((CONS . (A . ((CONS . (B . ((CONS . (C . ((CONS . (D . (NIL . NIL))) . NIL))) . NIL))) . NIL))) . NIL))))

;;; ----------------------------------------------------------------------
;;; 2. ASCII CLASSIFICATION
;;; ----------------------------------------------------------------------
;;;
;;; This is deliberately minimal. Expand it later if desired.

(ASCII-CONTROL-P .
 (LAMBDA . ((LOW . NIL) .
   ((COND .
      ((((EQ . (LOW . (#x00 . NIL))) . NIL) . (T . NIL))
       ((((EQ . (LOW . (#x01 . NIL))) . NIL) . (T . NIL))
        ((((EQ . (LOW . (#x02 . NIL))) . NIL) . (T . NIL))
         ((((EQ . (LOW . (#x03 . NIL))) . NIL) . (T . NIL))
          ((((EQ . (LOW . (#x1C . NIL))) . NIL) . (T . NIL))
           ((((EQ . (LOW . (#x1D . NIL))) . NIL) . (T . NIL))
            ((((EQ . (LOW . (#x1E . NIL))) . NIL) . (T . NIL))
             ((((EQ . (LOW . (#x1F . NIL))) . NIL) . (T . NIL))
              ((((EQ . (LOW . (#x7F . NIL))) . NIL) . (T . NIL))
               ((T . (NIL . NIL)) . NIL)))))))))
    . NIL))))

(ASCII-DELIMITER-P .
 (LAMBDA . ((LOW . NIL) .
   ((COND .
      ((((EQ . (LOW . (#x28 . NIL))) . NIL) . (T . NIL))   ; (
       ((((EQ . (LOW . (#x29 . NIL))) . NIL) . (T . NIL))  ; )
        ((((EQ . (LOW . (#x2E . NIL))) . NIL) . (T . NIL)) ; .
         ((((EQ . (LOW . (#x20 . NIL))) . NIL) . (T . NIL)) ; space
          ((T . (NIL . NIL)) . NIL)))))
    . NIL))))

;;; ----------------------------------------------------------------------
;;; 3. TOKENIZATION OF A CLOSED CODEPOINT
;;; ----------------------------------------------------------------------

(CODEPOINT->TOKEN .
 (LAMBDA . ((CELL . NIL) .
   ((LAMBDA . ((ENTRY . NIL) .
      ((COND .
         ((((NULLP . (ENTRY . NIL)) . NIL)
           . ((MAKE-TOKEN . (UNKNOWN . ((CAR . (CELL . NIL))
                                  . ((CDR . (CELL . NIL))
                                  . (NIL . NIL))))) . NIL))

          ;; ASCII
          ((((EQ . ((CAR . (CELL . NIL)) . (ASCII . NIL))) . NIL)
            .
            ((COND .
               ((((ASCII-CONTROL-P . ((CDR . (CELL . NIL)) . NIL)) . NIL)
                 . ((MAKE-TOKEN . (CONTROL
                                 . (ASCII
                                 . ((CDR . (CELL . NIL))
                                 . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))
                ((((ASCII-DELIMITER-P . ((CDR . (CELL . NIL)) . NIL)) . NIL)
                  . ((MAKE-TOKEN . (DELIMITER
                                  . (ASCII
                                  . ((CDR . (CELL . NIL))
                                  . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))
                 ((T . ((MAKE-TOKEN . (ATOM
                                     . (ASCII
                                     . ((CDR . (CELL . NIL))
                                     . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))
                  . NIL)))
             . NIL))

           ;; AEGEAN
           ((((EQ . ((CAR . (CELL . NIL)) . (AEGEAN . NIL))) . NIL)
             . ((MAKE-TOKEN . (NUMBER
                             . (AEGEAN
                             . ((CDR . (CELL . NIL))
                             . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))

            ;; BRAILLE
            ((((EQ . ((CAR . (CELL . NIL)) . (BRAILLE . NIL))) . NIL)
              . ((MAKE-TOKEN . (GLYPH
                              . (BRAILLE
                              . ((CDR . (CELL . NIL))
                              . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))
             ((T . ((MAKE-TOKEN . (UNKNOWN
                                 . ((CAR . (CELL . NIL))
                                 . ((CDR . (CELL . NIL))
                                 . ((CDR . (ENTRY . NIL)) . NIL))))) . NIL))
              . NIL)))))
       . NIL)))
    . ((CLOSE-CODEPOINT . (CELL . NIL)) . NIL))))

;;; ----------------------------------------------------------------------
;;; 4. READER STATE
;;; ----------------------------------------------------------------------
;;;
;;; Reader state is an alist:
;;;   ((MODE . READ)
;;;    (STACK . NIL)
;;;    (ACC . NIL))
;;;
;;; MODE is symbolic only for now.

(MAKE-READER-STATE .
 (LAMBDA . NIL
   ((LIST3 . ((CONS . (MODE . (READ . NIL)))
            . ((CONS . (STACK . (NIL . NIL)))
            . ((CONS . (ACC . (NIL . NIL)))
            . NIL))))
    . NIL)))

(READER-MODE .
 (LAMBDA . ((STATE . NIL) . ((ALIST-GET . (MODE . (STATE . NIL))) . NIL))))

(READER-STACK .
 (LAMBDA . ((STATE . NIL) . ((ALIST-GET . (STACK . (STATE . NIL))) . NIL))))

(READER-ACC .
 (LAMBDA . ((STATE . NIL) . ((ALIST-GET . (ACC . (STATE . NIL))) . NIL))))

;;; ----------------------------------------------------------------------
;;; 5. DELIMITER INTERPRETATION
;;; ----------------------------------------------------------------------
;;;
;;; Minimal ASCII grammar:
;;;   LPAREN -> open list frame
;;;   RPAREN -> close list frame
;;;   DOT    -> dotted pair separator
;;;   SPACE  -> ignore / token boundary

(TOKEN-VALUE .
 (LAMBDA . ((TOK . NIL) .
   ((CADDDR . (TOK . NIL)) . NIL))))

(TOKEN-TYPE .
 (LAMBDA . ((TOK . NIL) .
   ((CADR . (TOK . NIL)) . NIL))))

(IS-LPAREN .
 (LAMBDA . ((TOK . NIL) .
   ((EQ . ((TOKEN-VALUE . (TOK . NIL)) . (LPAREN . NIL))) . NIL))))

(IS-RPAREN .
 (LAMBDA . ((TOK . NIL) .
   ((EQ . ((TOKEN-VALUE . (TOK . NIL)) . (RPAREN . NIL))) . NIL))))

(IS-DOT .
 (LAMBDA . ((TOK . NIL) .
   ((EQ . ((TOKEN-VALUE . (TOK . NIL)) . (DOT . NIL))) . NIL))))

(IS-SPACE .
 (LAMBDA . ((TOK . NIL) .
   ((EQ . ((TOKEN-VALUE . (TOK . NIL)) . (SPACE . NIL))) . NIL))))

;;; ----------------------------------------------------------------------
;;; 6. TOKEN -> ATOM / DATA
;;; ----------------------------------------------------------------------
;;;
;;; For now:
;;;   ASCII ATOM/CONTROL payloads stay symbolic
;;;   AEGEAN NUMBER payloads stay tagged
;;;   BRAILLE GLYPH payloads stay tagged

(TOKEN->FORM .
 (LAMBDA . ((TOK . NIL) .
   ((COND .
      ((((EQ . ((TOKEN-TYPE . (TOK . NIL)) . (ATOM . NIL))) . NIL)
        . ((TOKEN-VALUE . (TOK . NIL)) . NIL))
       ((((EQ . ((TOKEN-TYPE . (TOK . NIL)) . (CONTROL . NIL))) . NIL)
         . ((TOKEN-VALUE . (TOK . NIL)) . NIL))
        ((((EQ . ((TOKEN-TYPE . (TOK . NIL)) . (NUMBER . NIL))) . NIL)
          . (TOK . NIL))
         ((((EQ . ((TOKEN-TYPE . (TOK . NIL)) . (GLYPH . NIL))) . NIL)
           . (TOK . NIL))
          ((T . (TOK . NIL)) . NIL)))))
    . NIL))))

;;; ----------------------------------------------------------------------
;;; 7. VERY SMALL READER STEP
;;; ----------------------------------------------------------------------
;;;
;;; This is a placeholder stepping interface, not yet a full parser.
;;; It demonstrates:
;;;   surface codepoint -> token -> form fragment

(READ-STEP .
 (LAMBDA . ((CELL . (STATE . NIL)) .
   ((LAMBDA . ((TOK . NIL) .
      ((COND .
         ((((IS-SPACE . (TOK . NIL)) . NIL) . (STATE . NIL))
          ((T .
            ((CONS . ((CONS . (LAST-TOKEN . (TOK . NIL)))
                   . (STATE . NIL)))
             . NIL))
           . NIL)))
       . NIL)))
    . ((CODEPOINT->TOKEN . (CELL . NIL)) . NIL))))

;;; ----------------------------------------------------------------------
;;; 8. DIRECT EVALUATION BRIDGE
;;; ----------------------------------------------------------------------
;;;
;;; If a codepoint directly resolves to a symbolic atom, this bridge sends it
;;; into the evaluator environment.

(EVAL-CODEPOINT .
 (LAMBDA . ((CELL . (ENV . NIL)) .
   ((EVAL . ((TOKEN->FORM . ((CODEPOINT->TOKEN . (CELL . NIL)) . NIL))
          . (ENV . NIL)))
    . NIL))))

;;; ----------------------------------------------------------------------
;;; 9. EXAMPLES
;;; ----------------------------------------------------------------------

(EXAMPLE-ASCII-FS .
 ((CODEPOINT->TOKEN . ((ASCII . #x1C) . NIL)) . NIL))

(EXAMPLE-AEGEAN-1 .
 ((CODEPOINT->TOKEN . ((AEGEAN . #x07) . NIL)) . NIL))

(EXAMPLE-BRAILLE-1 .
 ((CODEPOINT->TOKEN . ((BRAILLE . #x01) . NIL)) . NIL))

;;; ----------------------------------------------------------------------
;;; 10. SUMMARY
;;; ----------------------------------------------------------------------
;;;
;;; This file is the bridge:
;;;
;;;   (PLANE . OFFSET)
;;;      -> CLOSE-CODEPOINT
;;;      -> CODEPOINT->TOKEN
;;;      -> TOKEN->FORM
;;;      -> EVAL
;;;
;;; It is intentionally minimal and leaves room for:
;;;   - full reader stack discipline
;;;   - true dotted pair parsing
;;;   - Aegean numeric reduction
;;;   - Braille pointer/payload decoding
;;;   - delta-law state threading
