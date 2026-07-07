# OMI Pre-Header Notation

OMI is a pre-Lisp Lisp: the control lattice comes before `eval/apply`.
The runtime scaffold hears a byte stream as lexical scope, not just as
characters.

## Pre-Header Ladder

The pre-header is the in-band control sequence that prepares HEADER8. It is a
ladder of binary inversions:

```lisp
(!NULL
  .
  (((NULL . ESC)
    (CONTROL . SIGNAL)
    (REFERENCES . POINTERS)
    (NON-PRINTING . PRINTING)
    (ASCII . UNICODE))
    (UNICODE . BRAILLE))
   . AEGEAN))
```

Read this as:

```text
NOT-NULL stream
  enters NULL/ESC chirality
  enters CONTROL/SIGNAL cardinality
  enters REFERENCES/POINTERS ordinality
  enters NON-PRINTING/PRINTING modality
  enters ASCII/BRAILLE surface polarity
  closes upward into AEGEAN header/governance scope
```
These are non-printing control characters, they are runtine dependent

Event(NUL SOH) 
Event(NUL SOH	STX	ETX) : Event selector bit field (32 bit) set by QEMU.

CONTROL(NUL SOH	STX	ETX)/SIGNAL(EOT ENQ	ACK	BEL)
 
 REFERENCES(NUL	SOH	STX	ETX	EOT	ENQ	ACK	BEL	 BS 	 HT 	 LF 	 VT 	 FF 	 CR 	 SO 	 SI )/POINTERS(	DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	CAN	 EM 	SUB	ESC	 FS 	 GS 	 RS 	 US ) 

NON-PRINTING(	
NUL	SOH	STX	ETX	EOT	ENQ	ACK	BEL	 BS 	 HT 	 LF 	 VT 	 FF 	 CR 	 SO 	 SI 
DLE	DC1	DC2	DC3	DC4	NAK	SYN	ETB	CAN	 EM 	SUB	ESC	 FS 	 GS 	 RS 	 US 
 SP 	!	"	#	$	%	&	'	(	)	*	+	,	-	.	/
0	1	2	3	4	5	6	7	8	9	:	;	<	=	>	?
)/PRINTING(
@	A	B	C	D	E	F	G	H	I	J	K	L	M	N	O
P	Q	R	S	T	U	V	W	X	Y	Z	[	\	]	^	_
`	a	b	c	d	e	f	g	h	i	j	k	l	m	n	o
p	q	r	s	t	u	v	w	x	y	z	{	|	}	~	DEL
)

(ASCII)/(BRAILLE)

( !NULL
((NULL . ESC) (CONTROL . SIGNAL) . (REFERENCES . POINTERS) . ( NON-PRINTING . PRINTING ) . (ASCII . UNICODE). (UNICODE . BRAILLE))  . AEGEAN)

## Circular Inversion

Each pair is a circular inversion:

```text
(low . high)
```

The low side is the immediate byte-level hearing. The high side is the inverted
or lifted control meaning. The dot is not only a pair constructor; it is the
turning point where one side is interpreted through the other.

Examples:

```text
NULL . ESC
```

means quiescence and escape are joined as one control polarity.

```text
REFERENCES . POINTERS
```

means small control values can be heard either as symbolic references or as
addresses/pointers, depending on the active scope.

## ASCII Control Lattice

### Boundary

```text
NULL = 0x00
ESC  = 0x1B
```

This is the first control inversion. `NULL` is the quiescent floor. `ESC` is
the mode-shift ceiling.

### Control / Signal

```text
CONTROL = NUL SOH STX ETX
SIGNAL  = EOT ENQ ACK BEL
```

These are the first eight ASCII control cells split into two intonations:
control setup and signal response.

### References / Pointers

```text
REFERENCES = 0x00..0x0F
POINTERS   = 0x10..0x1F
```

The low half of the ASCII control range behaves as references. The high half
behaves as pointers and includes `ESC`, `FS`, `GS`, `RS`, and `US`.

### Non-Printing / Printing

```text
NON-PRINTING = 0x00..0x3F
PRINTING     = 0x40..0x7F
```

OMI intentionally places `SP` through `?` in the non-printing/intensional half.
This makes punctuation, decimal digits, and dot notation part of reader control
rather than ordinary payload text.

`@` through `DEL` form the printing/extensional half.

## Surface Planes

```text
ASCII   = 0x00..0x7F
BRAILLE = 0x80..0xBF
AEGEAN  = 0xC0..0xFF
```

ASCII is the control and structural substrate.

Braille is the dense payload plane. In the compact byte VM it is treated as
radix 256 payload notation.

Aegean is the lifted header/governance plane. In the compact byte VM it is
treated as radix 64 by using the low six bits as the active digit.

## Radix Witness

The notation stack has finite radices:

```text
BINARY      radix 2      raw bit control
HEXADECIMAL radix 16     nibble/byte address hearing
OMICRON     radix 4      BOM / ESC / BOUNDARY / ECC
BRAILLE     radix 256    payload cell
AEGEAN      radix 64     header/governance cell
```

This is why binary, hexadecimal, Omicron, Braille, and Aegean all matter. They
are not decorative alphabets. They are finite digit sets for different levels
of the same stream.

## Meta-Circular Interpolation

Meta-circular interpolation happens when a structural mark in the stream
changes how later stream elements are heard.

The minimal case is dot notation:

```text
A . B
```

The byte `.` is first heard as ASCII `0x2E`, then as a structural turn:

```text
left atom  -> DOT -> right atom
```

which emits:

```text
STRUCT.PAIR(left=A,right=B)
```

The same principle applies to the whole pre-header ladder. Each pair is a
turning point that lets the stream interpret the next part of itself.

## HEADER8 Relation

HEADER8 is the concrete runtime projection of this pre-header hearing:

```text
[0] NULL
[1] ESC
[2] FS
[3] GS
[4] RS
[5] US
[6] input byte
[7] current state
```

The pre-header ladder tells the VM how to hear slots 6 and 7. HEADER8 is not
just eight bytes; it is the control injection surface.

