;; -*- lisp -*-
;; omi-braille-table.lisp
;; Lookup table: Braille (0x80-0xBF) → 6-bit values for significand
;;
;; Braille range: 0x80-0xBF = 128-191 = 64 values
;; Each value = 6 bits (0-63)
;;
;; Usage: (aref omi-braille-table byte) returns 0-63

(defvar omi-braille-to-bits
  (let ((table (make-array 64 :element-type '(unsigned-byte 8))))
    (dotimes (i 64)
      (setf (aref table i) i))
    table))

;; Inverse: 6-bit value → Braille byte
(defvar omi-bits-to-braille
  (let ((table (make-array 64 :element-type '(unsigned-byte 8))))
    (dotimes (i 64)
      (setf (aref table i) (+ #x80 i)))
    table))

;; Example: 0x80 → 0, 0x81 → 1, ..., 0xBF → 63
;;         0x80 + n → n

;; Convert raw byte to lookup index
(defun omi-braille-index (byte)
  "Convert byte 0x80-0xBF to index 0-63"
  (cond
    ((and (>= byte #x80) (<= byte #xBF))
     (- byte #x80))
    (t
     (error "Not a Braille byte: ~x" byte))))

;; Convert index to Braille byte
(defun omi-index-braille (index)
  "Convert index 0-63 to Braille byte 0x80-0xBF"
  (cond
    ((and (>= index 0) (< index 64))
     (+ #x80 index))
    (t
     (error "Index out of range: ~d" index))))

;; Extract 6 bits from significand at position
(defun omi-extract-significand-bits ( significand position num-bits)
  "Extract NUM-BITS from SIGNIFICAND at bit POSITION"
  (logand #x3F (ash significand (- position))))

;; Pack significand into Braille cells
(defun omi-pack-significand (significand)
  "Pack 236-bit significand into 40 Braille cells (0x80-0xBF)"
  (let ((cells (make-array 40 :element-type '(unsigned-byte 8))))
    (dotimes (i 40)
      (let ((bits (logand #x3F (ash significand (* -6 (- i 39))))))
        (setf (aref cells i) (+ #x80 bits))))
    cells))

;; Unpack Braille cells into significand
(defun omi-unpack-significand (cells)
  "Unpack 40 Braille cells into 236-bit significand"
  (let ((significand 0))
    (dotimes (i 40)
      (let ((cell (logand #x3F (- (aref cells i) #x80))))
        (setf significand
              (logior significand
                     (ash cell (* 6 (- 39 i)))))))
    significand))

;; Test
(print "Braille table loaded")
(print (format nil "~%0x80 -> ~d" (omi-braille-index #x80)))
(print (format nil "~%0xBF -> ~d" (omi-braille-index #xBF)))
(print (format nil "~%index 0 -> ~x" (omi-index-braille 0)))
(print (format nil "~%index 63 -> ~x" (omi-index-braille 63)))