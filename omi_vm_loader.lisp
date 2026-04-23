;;; omi_vm_loader.lisp
;;;
;;; Top-level OMI VM loader.
;;;
;;; This file deliberately does not define or call a classic Lisp EVAL/APPLY
;;; loop. It loads the HEADER8 reader and rewrite core, then exposes the VM as:
;;;
;;;   byte -> HEADER8 -> scope term -> match/rewrite/fixpoint -> emit
;;;
;;; The Common Lisp host is only a bootstrap substrate for testing this file.

(load "omi_header8_reader.lisp")
(load "omi_rewrite_core.lisp")
(load "omi_loader.lisp")

(defparameter *omi-vm-version* "0.1")

(defun omi-vm-reset ()
  (omi-reset))

(defun omi-vm-step (byte)
  (byte-transcript byte))

(defun omi-vm-batch (bytes)
  (omi-reset)
  (stream-transcript bytes))

(defun omi-vm-control-test ()
  (omi-vm-batch '(0 27 28 29 30 31)))

(defun omi-vm-plane-test ()
  (omi-vm-batch '(46 48 64 65 128 191 192 255)))

(defun omi-vm-self-test ()
  (list
    (cons 'VERSION *omi-vm-version*)
    (cons 'CONTROL (omi-vm-control-test))
    (cons 'PLANES (omi-vm-plane-test))))

(defun omi-vm-repl ()
  (format t "OMI-LISP VM ~A~%" *omi-vm-version*)
  (format t "Enter byte values 0..255, or QUIT.~%")
  (omi-reset)
  (loop
    (format t "> ")
    (finish-output)
    (let ((x (read)))
      (cond
        ((eq x 'quit) (return 'GOODBYE))
        ((and (integerp x) (<= 0 x) (<= x 255))
         (format t "~S~%" (omi-vm-step x)))
        (t
         (format t "? expected byte 0..255 or QUIT~%"))))))

(format t ";;; OMI VM loader ready: (omi-vm-self-test), (omi-vm-repl)~%")
