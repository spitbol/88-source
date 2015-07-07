;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;-------------------------- blocks88.inc -----------------------------
.xlist
;	Version 0.90	18-Dec-89
;			Initial version
;
;	Version 1.00	28-Feb-90
;			First release version.  Add additional blocks.
;
;	Version 1.10	30-Oct-90
;			8088 version.
;
;	Version 1.20	10-Sep-92
;			Add string buffer blocks.
;
;	Version	1.27	01-Feb-93
;			Added fcbmod to fcb structure.
;
;	Copyright 1989-1993 Catspaw, Inc.   All Rights Reserved.
;
;	SPITBOL BLOCKS
;
;	All SPITBOL objects are encapsulated into blocks of memory.
;	The first word of each block identifies the block type in a
;	curious way.  Rather than containing a simple integer type
;	code, it contains the address of a subroutine that performs
;	some particular action on the block, such as placing it on
;	the stack.  Because each block type uses a different subroutine,
;	the subroutine addresses serve as a type identifier.
;
;	Each subroutine is proceeded in memory by a one-word integer
;	type code (given below).  Thus to obtain a simple ordinal
;	type code for a block pointed to by an address in BX, the following
;	code sequence will load the ordinal value into register AX:
;
;	....			Block pointer in BX
;	MOV	BX,[BX]	Type word (subroutine pointer) to BX
;	MOV	AX,[BX-2]	Load integer type code to AX
;
;
;	+---------------------+               +----------------------------+
;	|      Type Word      |----+          |      Integer type word     |
;	+---------------------+    |          +-------------------+--------+
;	|      Block Data     |    +--------->|   Program Code for this    |
;      ...                   ...              |   Block Type               |
;	|                     |               |                            |
;	+---------------------+               +----------------------------+
;
;
;	Given an integer type, the type word can be obtained by
;	looking it up in a table provided to external functions in ptyptab
;	(provided in the miscellaneous area).
;	For example, if the ptyptab pointer is loaded into ES:SI, then given
;	a small integer type code in BX, the type word is obtained with:
;
;	SHL	BX,1
;	MOV	AX,ES:[SI+BX]     Obtain type word
;
;
;	BLOCK CODES FOR ACCESSIBLE DATATYPES
;
;	These blocks may appear in an argument list if left untranslated
;	by the LOAD function definition.
;
BL_AR	EQU	0		;ARBLK	ARRAY
BL_CD	EQU	BL_AR+1		;CDBLK	CODE
BL_EX	EQU	BL_CD+1		;EXBLK	EXPRESSION
BL_IC	EQU	BL_EX+1		;ICBLK	INTEGER
BL_NM	EQU	BL_IC+1		;NMBLK	NAME
BL_P0	EQU	BL_NM+1		;P0BLK	PATTERN, 0 args
BL_P1	EQU	BL_P0+1		;P1BLK	PATTERN, 1 arg
BL_P2	EQU	BL_P1+1		;P2BLK	PATTERN, 2 args
BL_RC	EQU	BL_P2+1		;RCBLK	REAL
BL_SC	EQU	BL_RC+1		;SCBLK	STRING
BL_SE	EQU	BL_SC+1		;SEBLK	EXPRESSION
BL_TB	EQU	BL_SE+1		;TBBLK	TABLE
BL_VC	EQU	BL_TB+1		;VCBLK	VECTOR (array)
BL_XN	EQU	BL_VC+1		;XNBLK	EXTERNAL, non-relocatable contents
BL_XR	EQU	BL_XN+1		;XRBLK	EXTERNAL, relocatable contents
BL_BC	EQU	BL_XR+1		;BCBLK	BUFFER CONTROL
BL_PD	EQU	BL_BC+1		;PDBLK	PROGRAM DEFINED DATATYPE

BL__D	EQU	BL_PD+1		;NUMBER OF BLOCK CODES FOR DATA
;
;
;	OTHER BLOCK CODES
;
;	These blocks will never appear in an argument list, but are
;	listed here for completeness.
;
BL_TR	EQU	BL_PD+1		;TRBLK	TRACE
BL_BF	EQU	BL_TR+1		;BFBLK	STRING BUFFER
BL_CC	EQU	BL_BF+1		;CCBLK	CODE CONSTRUCTION
BL_CM	EQU	BL_CC+1		;CMBLK	COMPILER TREE NODE
BL_CT	EQU	BL_CM+1		;CTBLK	CHARACTER TABLE
BL_DF	EQU	BL_CT+1		;DFBLK	DATATYPE FUNCTION
BL_EF	EQU	BL_DF+1		;EFBLK	EXTERNAL FUNCTION
BL_EV	EQU	BL_EF+1		;EVBLK	EXPRESSION VARIABLE
BL_FF	EQU	BL_EV+1		;FFBLK	FIELD FUNCTION
BL_KV	EQU	BL_FF+1		;KVBLK	KEYWORD VARIABLE
BL_PF	EQU	BL_KV+1		;PFBLK	PROGRAM-DEFINED FUNCTIN
BL_TE	EQU	BL_PF+1		;TEBLK	TABLE ELEMENT
;
;
;
;  	Structure of common SPITBOL blocks:
;  	  Integer, Real, String, File and Buffer blocks.
;
;  	These structures can be applied to the result area to determine
;  	where to store required return information.
;
;
;  	Structure of ICBLK (integers)
;
icblk	struc
ictyp	dw	?		;type word
icval	dd	?		;integer value (16 or 32 bits)
icblk	ends
;
;
;  	Structure of RCBLK (reals)
;
rcblk	struc
rctyp	dw	?		;type word
rcval	dq	?		;real value
rcblk	ends
;
;  	Structure of SCBLK (strings)
;
scblk	struc
sctyp	dw	?		;type word
sclen	dw	?		;string length
scstr	db	?		;start of string
scblk	ends
;
;  	Structure for returning a far string
;
fsblk	struc
fstyp	dw	?		;type word
fslen	dw	?		;string length
fsptr	dd	?		;far pointer to string
fsblk	ends
;
;  	Structure for returning a far external block
;
fxblk	struc
fxtyp	dw	?		;type word
fxlen	dw	?		;external data length
fxptr	dd	?		;far pointer to external data
fxblk	ends
;
;
;	FILE CONTROL BLOCK
;
;	The user may provide the word "FILE" for any argument in
;	the LOAD function prototype.  When a call is made to the
;	external function with an I/O associated variable in this argument
;	position, SPITBOL will provide a pointer to the file control
;	block instead of the value of the variable.
;
;	The file control block (FCB) points to an I/O block with
;	additional information.  In turn, the I/O block points to any
;	buffer used by the file.
;
;	This block is obtained for every file except those associated
;	with INPUT, OUTPUT, or TERMINAL.  Note that these FCB's are
;	unrelated to MS-DOS FCB's.  File control blocks do not have
;	their own type word, but appear as XRBLKs with the following structure:

;
fcblk	struc
fcbtyp	dw	?		;type word (XRBLK)
fcblen	dw	?		;size of block, in bytes
fcbrsz	dw	?		;SPITBOL record size
fcbiob	dw	?		;near pointer to IOBLK
fcbmod	dw	?		;1 if text mode, 0 if binary
fcblk	ends
;
;	I/O BLOCK
;
;	An I/O block is pointed to by the fcbiob field of a file control block.
;
ioblk	struc
iobtyp	dw	?		;type word (XRBLK)
ioblen	dw	?		;size of IOBLK in bytes
iobfnm	dw	?		;near pointer to SCBLK holding filename
iobpid	dw	?		;process id for pipe (not used for DOS)
iobbfb	dw	?		;near pointer to BFBBLK
iobfdn	dw	?		;MS-DOS file descriptor number
iobflg1	dw	?		;flag word 1 (see below)
iobflg2	dw	?		;flag word 2 (see below)
iobeol1 dw	?		;end of line character 1
iobeol2	dw	?		;end of line character 2
ioblk	ends

;  	Bits in iobflg1 word:
;
IO_INP	equ	00000001h	;input file
IO_OUP	equ	00000002h	;output file
IO_APP	equ	00000004h	;append output to end of existing file
IO_OPN	equ	00000008h	;file is open
IO_COT	equ	00000010h	;console output to non-disk device
IO_CIN	equ	00000020h	;console input from non-disk device
IO_SYS	equ	00000040h	;system file descriptor used instead of name
IO_WRC	equ	00000080h	;output without buffering
IO_EOT	equ     00000100h       ;ignore end-of-text (control-Z) character

;  	Bits in iobflg2 word:
;
IO_PIP	equ	000001h		;pipe (not used in MS-DOS)
IO_DED	equ	000002h		;dead pipe (not used in MS-DOS)
IO_ILL	equ	000004h		;illegal I/O association
IO_RAW	equ	000008h		;binary I/O
IO_LF	equ	000010h		;ignore line feed if next character
IO_NOE	equ	000020h		;no echo input
IO_ENV	equ	000040h		;filearg1 maps to filename thru environment var


;
;	I/O BUFFER BLOCK
;
;	An I/O buffer block (BFBBLK) is pointed to by an IOBLK.
;
bfbblk	struc
bfbtyp	dw	?		;type word (XNBLK)
bfblen	dw	?		;size of BFBBLK, in bytes
bfbsiz	dw	?		;size of buffer in bytes
bfbfil	dw	?		;number of bytes currently in buffer
bfbnxt	dw	?		;offset of next buffer char to r/w
bfboff	dd	?		;file position of first byte in buf
bfbcur	dd	?		;physical file position
bfbbuf	db	?		;start of buffer
bfbblk	ends
;
;
;  	Structure of EFBLK (external function).  A pointer to this block
;  	is passed to the external function in the miscellaneous area
;	in pefblk.
;
efblk	struc
fcode	dw	?		;type word
fargs	dw	?		;number of arguments
eflen	dw	?		;block length
efuse	dw	?		;usage count
efcod	dw	?		;near pointer to XNBLK, see below
efvar	dw	?		;near pointer to VRBLK with function name
efrsl	dw	?		;result type  (see below)
eftar	dw	?		;argument types, one word per arg (see below)
efblk	ends
;
;  efrsl and eftar[] words contain small integer type codes as follows:
;  0 - unconverted
;  1 - convert to string
;  2 - convert to integer
;  3 - convert to real
;  4 - produce as file control block associated with variable
;
;
;  	Structure of XNBLK allocated for external function
;  	A pointer to this structure is passed to the external function
;  	in the miscellaneous area in pxnblk.
;
xnblk	struc			;keep as multiple of words
xntyp	dw	?		;type word
xnlen	dw	?		;length of this block
xncall	dd	?		;entry point of function
xnmemp	dd	?		;base address of function memory
xnsiz	dd	?		;size of function in bytes
xnip	dw	?		;transfer IP
xncs	dw	?		;transfer CS
xnsp	dw	?		;transfer SP, 0 to use SPITBOL's stack
xnss	dw	?		;transfer SS, 0 to use SPITBOL's stack
xnds	dw	?		;transfer DS
xnes	dw	?		;transfer ES
xn1st	dw	?		;non-zero = first-ever call
xnsave	dw	?		;non-zero = first call after reload of save file
xncbp	dd	?		;call back address prior to saving or exiting
xnblk	ends
xndta	equ	word ptr xncall	;start of user data region in XNBLK
;

;
;  Other selected blocks of interest:
;
;
;	ARRAY BLOCK
;
;	An array block (ARBLK) represents an array value other than one
;	with one dimension whose lower bound is one (see VCBLK).
;
arblk	struc
artyp	dw	?		;type word (ARBLK)
aridv	dw	?		;identifier value
arlen	dw	?		;length of ARBLK in bytes
arofs	dw	?		;offset in arblk to arpro field
arndm	dw	?		;number of dimensions
arlbd	dw	?		;low bound (first subscript)
ardim	dw	?		;dimension (first subscript)
; above two words are repeated for each additional dimension
arpro	dw	?		;array prototype string
arvls	dw	?		;start of values in row-wise order
arblk	ends

;	BUFFER CONTROL BLOCK
;
;	A buffer control block (BCBLK) is created by the BUFFER
;	function, and serves as an indirect control header for the
;	buffer. It contains the number of characters currently
;	stored in the buffer.
;
bcblk	struc
bctyp	dw	?		;type word (BCBLK)
bcidv	dw	?		;identifier value
bclen	dw	?		;number of chars in use in bfblk
bcbuf	dw	?		;pointer to bfblk
bcblk	ends

;	STRING BUFFER BLOCK
;
;	A string buffer block (BFBLK) contains the actual buffer
;	memory area. It specifies the largest string that can be
;	stored in the buffer.
;
bfblk	struc
bftyp	dw	?		;type word (BFBLK)
bfalc	dw	?		;allocated size of buffer
bfchr	db	?		;characters of string
bfblk	ends

;
;	NAME BLOCK
;
;	A name block (NMBLK) is used whereever a name must be stored as
;	a value following use of the unary dot operator.
;
nmblk	struc
nmtyp	dw	?		;type word (NMBLK)
nmbas	dw	?		;base pointer for variable
nmofs	dw	?		;offset for variable
nmblk	ends

;
;	TABLE BLOCK
;
;	A table block (TBBLK) is used to represent a table value.
;	It comprises a list of buckets, each of which may point to
;	a chain of TEBLKs.  TBBUK entries either point to the first
;	TEBLK on the chain or they point to the TBBLK itself to
;	indicate the end of the chain.  The number of buckets can
;	be deduced from tblen.
;
tbblk	struc
tbtyp	dw	?		;type word (TBBLK)
tbidv	dw	?		;identifier value
tblen	dw	?		;length of TBBLK in bytes
tbinv	dw	?		;default initial lookup value
tbbuk	dw	?		;start of hash bucket pointers
tbblk	ends

;
;	TABLE ELEMENT BLOCK
;
;	A table element (TEBLK) is used to represent a single entry in
;	a table.
;
teblk	struc
tetyp	dw	?		;type word (TEBLK)
tesub	dw	?		;subscript value
teval	dw	?		;table element value
tenxt	dw	?		;link to next TEBLK or TBBLK if end of chain
teblk	ends

;	VECTOR BLOCK
;
;	A vector block (VCBLK) is used to represent an array value which has
;	one dimension whose lower bound is one. All other arrays are
;	represented by ARBLKs.  The number of elements can be deduced
;       from vclen.
;
vcblk	struc
vctyp	dw	?		;type word (VCBLK)
vcidv	dw	?		;identifier value
vclen	dw	?		;length of vcblk in bytes
vcvls	dw	?		;start of vector values
vcblk	ends

.list
;---------------------  end of blocks88.inc -----------------------
