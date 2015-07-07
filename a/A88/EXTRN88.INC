; Copyright (C)  1988-1998  Mark Emmer

; SPITBOL is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.


;--------------------------  EXTRN88.INC -------------------------------------
; Equates of routines and data available to external function from SPITBOL-88


.xlist
.lall

; Macros for code that must be assembled differently for different systems.
;------------------------------------------------------------------
;
; Macro Glossary:
; entry     entry name		Define main procedure for this function.
;
; fstore    fstore arg		Store floating point result after call to
;				SPITBOL floating point function.
;
; preamble  preamble segname	Define group and seg name for 1 seg program.
;	    preamble seg1,seg2  Define group and segs for 2 segment program.
;
;
; procx	    proc name		Define near or far procedure.
;
; sload	    sload sreg,reg,arg	Load pointer into registers.
;
; sstore    sstore arg,reg,sreg Store pointer into memory.
;
; spop	    spop sreg		Pop segment register.
;
; spush	    spush sreg		Push segment register.
;
;
; Equate Glossary:
;
; Name	    Usage               Purpose
; --------  -----------------	-------------------------------------------
;
; rptr	    op rptr arg		Pointer size override.
;
; cbrptr    cbrptr		Size of rptr in bytes.
;
; scs	    op  reg, scs:arg	Code segment override.
;
; ses	    op  reg, ses:arg	ES segment override.
;
;
; Structure Glossary:
;
; Name	    Usage               Purpose
; --------  -----------------	-------------------------------------------
;
; dpt	    dpt <?>		Pointer data declaration.
;
; dps	    dps <?>		Pointer data declaration (on stack, padded
;				to even word boundary).


fstore	macro	arg
	fstp	arg
	fwait
	endm

preamble macro		segname1,segname2
ifb <segname2>
CGROUP	group		segname1
else
CGROUP	group		segname1,segname2
endif
	assume		cs:CGROUP,ds:CGROUP,es:CGROUP,ss:CGROUP
segname1 segment	word 'CODE'
	endm

rptr	equ	dword ptr
cbrptr	equ	4

scs	equ	DS
ses	equ	ES

dpt	struc
o	dw	?
s	dw	?
dpt	ends

dps	struc
	dd	?
dps	ends

procx	macro		procnm
procnm	proc		far
	endm

entry	macro		procnm
	org		100h
	procx		procnm
	endm

sload	macro	sreg,reg,arg
	l&sreg	reg,arg
	endm

sstore	macro	arg,reg,sreg
	mov	word ptr arg,reg 
	mov	word ptr arg+2,sreg
	endm

spop	macro	sreg
	pop	sreg
	endm

spush	macro	sreg
	push	sreg
	endm
.list


INCLUDE blocks88.inc
.xlist
;	V1.01	09-Mar-90 Initial 80386 version.
;	V1.02	30-May-92 Create 8088 version.
;	V1.02	10-Sep-92 Add string buffer blocks.
;
;	(c) Copyright 1992 - Catspaw, Incorporated
;
;
; At function entry, the stack looks like this:
;
;		+-----------------------+
; SS:SP ----->	|     return offset	|
;		|-----------------------|
;		|     return segment	|
;		|-----------------------|
;		|  result area offset	|
;		|-----------------------|
;		|  result area segment	|
;		|-----------------------|
;		| miscinfo area offset	|
;		|-----------------------|
;		| miscinfo area segment	|
;		|-----------------------|
;		|     last argument	|
;		|-----------------------|
;		|	. . . .		|
;		|-----------------------|
;		|    first argument	|
;		+-----------------------+
;
;
; Generally, the items in the miscinfo area can be ignored, and are provided
; only for the benefit of those wishing to operate directly on SPITBOL's
; internal data structures.
;
; However, the FAR result area pointer *must* be used by the external
; function to locate the area in which results are returned.
;
; Note that all FAR pointers are 4-byte quantities.
;
; Definition of structure of the miscinfo area.  A FAR pointer to this
; area is placed on the stack below the result area FAR pointer.
;
misc	struc
vers	dw	?		;internal version number
env	db	?		;host environment (see below)
	db	?		;reserved
nargs	dw	?		;number of arguments to function
ptyptab	dd	?		;pointer to table of data types
pxnblk	dd	?		;pointer to xnblk describing function
pefblk	dd	?		;pointer to efblk describing function
pflttab	dd	?		;pointer to floating point table
spds	dw	?		;SPITBOL's DS
spcs	dw	?		;SPITBOL's CS
misc	ends
;
; values in env word of misc structure.  Executing under:
t_pharlap equ	0		;PharLap DOS Extender
t_intel	equ	1		;Intel DOS Extender
t_os2	equ	2		;OS/2 2.0
t_tc16  equ	3		;MS-DOS TurboC with 16-bit IA
t_tc32	equ	4		;MS-DOS TurboC with 32-bit IA
t_w1616 equ	5		;16-bit Windows, 16-bit SPITBOL
t_w1632 equ	6		;16-bit Windows, 32-bit SPITBOL
t_wnt8	equ	7		;Windows NT on 386/486
t_sparc	equ	8		;Sun 4 / SPARC
t_mac	equ	9		;Apple Macintosh
t_mips  equ    10               ;MIPS R3000
;
; Sample usage.  Example stack frame for function arguments, assuming
;		 the execution of PUSH BP; MOV BP,SP to put the stack
;		 pointer in BP.  This example assumes the function is
;		 called with three arguments of type (INTEGER, REAL, STRING):
;
;	  	struc
;	stk_bp	dw	?		;save ebp
;	stk_ret	dd	?		;return pointer
;	presult	dd	?		;pointer to result area
;	pmisc	dd	?		;pointer to miscinfo area
;	parg3	dd	?		;pointer to arg 3 string
;	larg3	dw	?		;length of arg 3 string
;	rarg2	dq	?		;real arg 2
;	iarg1	dd	?		;long integer arg 1
;		ends
;
;		The pointer to the result area could be loaded with:
;
;		sload	es,di,[bp].presult
;
;		The pointer to the type table would be loaded by
;		double indexing:
;
;		sload	es,si,[bp].pmisc
;		sload	es,si,ses:[si].ptyptab
;
;  Note that regardless of whether the 16-bit or 32-bit integer
;  version of SPITBOL is in use, integer arguments are always passed
;  as 32-bit long values.


;
; Simple names for datatypes.  Performs a lookup in SPITBOL's type
; table to fetch a 16-bit type word for specific data types.
;
; The address of SPITBOL's type table is passed to the function on the
; stack in ptyptab (see above).  Prior to using these symbols, a FAR
; pointer to this table must be loaded into the registers.  These equates
; assume that the pointer has been loaded into ES:[SI].  There is nothing
; special about these registers, and any other segment:index pair can be used
; provided the equates are modified.
;
					;data types in type table at ES:[SI]
ar	equ	ses:[si+BL_AR*2]	; Array
bc	equ	ses:[si+BL_BC*2]	; Buffer Control
bf	equ	ses:[si+BL_BF*2]	; String Buffer
cd	equ	ses:[si+BL_CD*2]	; Code
ex	equ	ses:[si+BL_EX*2]	; Expression
ic	equ	ses:[si+BL_IC*2]	; Integer
nm	equ	ses:[si+BL_NM*2]	; Name
rc	equ	ses:[si+BL_RC*2]	; Real
sc	equ	ses:[si+BL_SC*2]	; String
tb	equ	ses:[si+BL_TB*2]	; Table
vc	equ	ses:[si+BL_VC*2]	; Vector
xn	equ	ses:[si+BL_XN*2]	; External

;
; Non-standard block-type values that may be returned in register AX.
;
FAIL	EQU	-1	       		; Signal function failure
BL_NC	EQU	100			; Unconverted result
BL_FS	EQU	101			; Far string
BL_FX	EQU	102			; Far external block

;
; Length of string area in result buffer
;
buflen	equ	256

; Vectors to real number functions.
;
; Reals are 8-byte, double-precision reals in IEEE format.
; Integers (for fix and float) are 32-bit two's complement numbers.
;
; Function arguments are pushed onto the stack left-to-right.  With 8-byte
; reals, the most-significant half (MSH) is pushed first.  Note that this
; differs from standard C calling conventions.
;
; Integer results are returned in DX:AX.  Real results are returned on
; the top of the floating point stack (real or emulated 8087 stack).
;
; The caller is responsible for removing arguments from the stack
; after the function returns.
;
; Functions taking two arguments, such as r_sub(r1,r2) order their
; operations as r1-r2.
;
; The address of a table of dispatch vectors is passed to the external
; function on the stack in pflttab (see above).  These equates assume
; that a pointer to the table has been loaded into ES:[EBX] by your code.
;
; Using the example above, this might be done with the following:
;
;			sload	es,bx,[bp].pmisc
;			sload	es,bx,ses:[bx].ptyptab
;
; The real functions preserve BX, SI, DI and BP, and may destroy
; AX, CX, and DX.
;
; Example.  Divide the real number in location r1 by the number in r2, and
;	    store the result in r3:
;
;	push	word ptr r1+6		;push first arg, msh
;	push	word ptr r1+4		;push first arg, ...
;	push	word ptr r1+2		;push first arg, ...
;	push	word ptr r1		;push first arg, lsh
;	push	word ptr r2+6		;push second arg, msh
;	push	word ptr r2+4		;push second arg, ...
;	push	word ptr r2+2		;push second arg, ...
;	push	word ptr r2		;push second arg, lsh
;	sload	ses,bx,[bp].pmisc	;get address of miscinfo area
;	sload	ses,bx,ses:[bx].pflttab	;get address of vector table
;	call	r_div			;perform division
;	add	sp, 2*8			;remove arguments
;	fstore	r3			;store result
;
;
;
fix	equ	dword ptr ses:[bx+ 0*4]	; real to integer	 i = r_2_i(r)
float	equ	dword ptr ses:[bx+ 1*4]	; integer to real	 r = i_2_r(i)
r_add	equ	dword ptr ses:[bx+ 2*4]	; real addition  	 r = r_add(r1,r2)
r_sub	equ	dword ptr ses:[bx+ 3*4]	; real subtraction	 r = r_sub(r1,r2)
r_mul	equ	dword ptr ses:[bx+ 4*4]	; real multiplication	 r = r_mul(r1,r2)
r_div	equ	dword ptr ses:[bx+ 5*4]	; real division  	 r = r_div(r1,r2)
r_neg	equ	dword ptr ses:[bx+ 6*4]	; real negation  	 r = r_neg(r)
r_atn	equ	dword ptr ses:[bx+ 7*4]	; real arc tangent	 r = r_atn(r)
r_chp	equ	dword ptr ses:[bx+ 8*4]	; real chop  		 r = r_chp(r)
r_cos	equ	dword ptr ses:[bx+ 9*4]	; real cosine  		 r = r_cos(r)
r_etx	equ	dword ptr ses:[bx+10*4]	; real e to the x	 r = r_etx(r)
r_lnf	equ	dword ptr ses:[bx+11*4]	; real natural logarithm r = r_lnf(r)
r_sin	equ	dword ptr ses:[bx+12*4]	; real sine 		 r = r_sin(r)
r_sqr	equ	dword ptr ses:[bx+13*4]	; real square root	 r = r_sqr(r)
r_tan	equ	dword ptr ses:[bx+14*4]	; real tangent  	 r = r_tan(r)

.list
;-------------------------- End of EXTRN88.INC -----------------------------
