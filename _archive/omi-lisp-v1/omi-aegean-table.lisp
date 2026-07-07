;; -*- lisp -*-
;; omi-aegean-table.lisp
;; Lookup table: Aegean (0xC0-0xFF) → 6-bit values for exponent
;;
;; Aegean range: 0xC0-0xFF = 192-255 = 64 values
;; Each value = 6 bits (0-63)
;;
;; Usage: (aref omi-aegean-table byte) returns 0-63

(defvar omi-aegean-to-bits
  (let ((table (make-array 64 :element-type '(unsigned-byte 8))))
    (dotimes (i 64)
      (setf (aref table i) i))
    table))

;; Inverse: 6-bit value → Aegean byte
(defvar omi-bits-to-aegean
  (let ((table (make-array 64 :element-type '(unsigned-byte 8))))
    (dotimes (i 64)
      (setf (aref table i) (+ #xC0 i)))
    table))

;; Example: 0xC0 → 0, 0xC1 → 1, ..., 0xFF → 63
;;         0xC0 + n → n

;; Convert raw byte to lookup index
(defun omi-aegean-index (byte)
  "Convert byte 0xC0-0xFF to index 0-63"
  (cond
    ((and (>= byte #xC0) (<= byte #xFF))
     (- byte #xC0))
    (t
     (error "Not an Aegean byte: ~x" byte))))

;; Convert index to Aegean byte
(defun omi-index-aegean (index)
  "Convert index 0-63 to Aegean byte 0xC0-0xFF"
  (cond
    ((and (>= index 0) (< index 64))
     (+ #xC0 index))
    (t
     (error "Index out of range: ~d" index))))

;; IEEE 754 binary256 constants
(defconstant +exponent-bias+ 262143)
(defconstant +exponent-min+ -262142)
(defconstant +exponent-max+ 262143)
(defconstant +exponent-width+ 19)

;; Encode exponent to Aegean cells  
(defun omi-pack-exponent (exponent)
  "Pack 19-bit exponent (+bias) into 4 Aegean cells (0xC0-0xFF)"
  (let* ((biased (+ exponent +exponent-bias+))
         (cells (make-array 4 :element-type '(unsigned-byte 8))))
    (dotimes (i 4)
      (let ((bits (logand #x3F (ash biased (* -6 (- i 3)))))
        (setf (aref cells i) (+ #xC0 bits))))
    cells))

;; Unpack Aegean cells into exponent
(defun omi-unpack-exponent (cells)
  "Unpack 4 Aegean cells into 19-bit exponent (minus bias)"
  (let ((biased 0))
    (dotimes (i 4)
      (let ((cell (logand #x3F (- (aref cells i) #xC0))))
        (setf biased
              (logior biased
                     (ash cell (* 6 (- 3 i)))))))
    (- biased +exponent-bias+)))

;; Test
(print "Aegean table loaded")
(print (format nil "~%0xC0 -> ~d" (omi-aegean-index #xC0)))
(print (format nil "~%0xFF -> ~d" (omi-aegean-index #xFF)))
(print (format nil "~%index 0 -> ~x" (omi-index-aegean 0)))
(print (format nil "~%index 63 -> ~x" (omi-index-aegean 63)))
(print (format nil "~%exponent 0 biased -> ~d" (+ 0 +exponent-bias+)))
(print (format nil "~%exponent -1 biased -> ~d" (+ -1 +exponent-bias+)))
(print (format nil "~%exponent 1 biased -> ~d" (+ 1 +exponent-bias+)))