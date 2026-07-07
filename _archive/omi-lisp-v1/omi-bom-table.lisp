;; -*- lisp -*-
;; omi-bom-table.lisp
;; Lookup table: BOM (NULL 0x00 / DEL 0x7F) → sign bit + 4 channels × 16-bit
;;
;; BOM = Beginning Of Message = control code
;; NULL (0x00) = positive sign (+1)
;; DEL (0x7F) = negative sign (-1)
;;
;; 4 channels × 16-bit = 64K frame control per channel
;; Channel 0: GPIO interrupt
;; Channel 1: PCI power management
;; Channel 2: USB wake
;; Channel 3: OEM specific

(defconstant +bom-null+ #x00)   ; NULL = positive
(defconstant +bom-del+ #x7F)   ; DEL = negative

;; Sign lookup
(defvar omi-bom-sign
  (list
    (cons +bom-null+ +1)    ; positive
    (cons +bom-del+ -1)))  ; negative

;; Get sign from BOM byte
(defun omi-bom-get-sign (byte)
  "Get sign from BOM byte: NULL → +1, DEL → -1"
  (cond
    ((= byte +bom-null+) +1)
    ((= byte +bom-del+) -1)
    (t (error "Not a BOM byte: ~x" byte))))

;; Set sign to BOM byte
(defun omi-bom-set-sign (sign)
  "Convert sign to BOM byte: +1 → NULL, -1 → DEL"
  (cond
    ((= sign +1) +bom-null+)
    ((= sign -1) +bom-del+)
    (t (error "Invalid sign: ~d" sign))))

;; Channel frame control
;; Each channel = 16 bits = values 0-65535
(defconstant +channel-width+ 16)
(defconstant +channel-mask+ #xFFFF)
(defconstant +num-channels+ 4)

;; Get channel value from 64-bit channel register
(defun omi-bom-get-channel (channels index)
  "Get 16-bit value from channel INDEX (0-3)"
  (let ((shift (* index +channel-width+)))
    (logand +channel-mask+ (ash channels (* -shift)))))

;; Set channel value in 64-bit channel register
(defun omi-bom-set-channel (channels index value)
  "Set 16-bit VALUE in channel INDEX (0-3)"
  (let ((shift (* index +channel-width+)))
    (logior channels
           (logand
             (ash (logand value +channel-mask+) shift)
             (lognot (ash +channel-mask+ shift))))))

;; Full octuple encoding
(defun omi-octuple-pack (significand exponent sign channels)
  "Pack full 256-bit octuple: significand (236) + exponent (19) + sign (1) + channels (64)"
  (let ((result (make-array 40 :element-type '(unsigned-byte 8))))
    ;; Byte 0: BOM (sign)
    (setf (aref result 0) (omi-bom-set-sign sign))
    ;; Bytes 1-2: Exponent (19 bits) - high byte first
    (let ((biased (+ exponent 262143)))
      (setf (aref result 1) (logand #xFF (ash biased -11)))
      (setf (aref result 2) (logand #xFF biased)))
    ;; Bytes 3: Reserved
    (setf (aref result 3) #x00)
    ;; Bytes 4-7: Channels (64 bits = 8 bytes)
    (setf (aref result 4) (logand #xFF (ash channels -0)))
    (setf (aref result 5) (logand #xFF (ash channels -8)))
    (setf (aref result 6) (logand #xFF (ash channels -16)))
    (setf (aref result 7) (logand #xFF (ash channels -24)))
    (setf (aref result 8) (logand #xFF (ash channels -32)))
    (setf (aref result 9) (logand #xFF (ash channels -40)))
    (setf (aref result 10) (logand #xFF (ash channels -48)))
    (setf (aref result 11) (logand #xFF (ash channels -56)))
    ;; Bytes 12-39: Significand (236 bits = 27 bytes, padded to 28)
    (dotimes (i 28)
      (let ((bits (logand #x3F (ash significand (* -6 (- i 27))))))
        (setf (aref result (+ 12 i)) (+ #x80 bits))))
    result))

;; Unpack full octuple
(defun omi-octuple-unpack (bytes)
  "Unpack 40-byte octuple → (significand exponent sign channels)"
  (let ((sign (omi-bom-get-sign (aref bytes 0)))
        (biased (logior (aref bytes 1) (ash (aref bytes 2) 8)))
        (exponent (- biased 262143))
        (channels 0))
    ;; Channels
    (dotimes (i 8)
      (setf channels
            (logior channels
                   (ash (aref bytes (+ 4 i)) (* 8 i)))))
    ;; Significand
    (let ((significand 0))
      (dotimes (i 28)
        (let ((cell (logand #x3F (- (aref bytes (+ 12 i)) #x80))))
          (setf significand
                (logior significand
                       (ash cell (* 6 (- 27 i)))))))
      (list significand exponent sign channels))))

;; Test
(print "BOM table loaded")
(print (format nil "~%NULL -> sign ~d" (omi-bom-get-sign #x00)))
(print (format nil "~%DEL -> sign ~d" (omi-bom-get-sign #x7F)))
(print (format nil "~%sign +1 -> BOM ~x" (omi-bom-set-sign +1)))
(print (format nil "~%sign -1 -> BOM ~x" (omi-bom-set-sign -1)))
(print (format nil "~%channels test -> ~x" 
              (omi-bom-set-channel 0 0 #x1234)))
(print (format nil "~%exponent bias = ~d" 262143))