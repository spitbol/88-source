	page	80,132
	title	XPUT - Put Integer into an External Block
	name	XPUT
	JUMPS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;	Usage:
;
;	LOAD("XPUT(INTEGER,INTEGER,EXTERNAL)")
;
;	Argument 1 is the integer to be stored in the external block.
;	Argument 2 is the subscript of the desired slot, 1 to N.
;	Argument 3 is the external block produced by XCREATE.
;
;	Call with:
;		XPUT(123, 456, X)	;* store 123 in X[456]
;
;	Returns the null string if successful, fails if incoming
;	argument 2 is out of range, or argument 3 not an external block.
;
;
;
INCLUDE system.inc
INCLUDE	extrn88.inc

	preamble _TEXT

;
;  Describe entry stack
;
stk	struc
stkbp	dw	?			;save bp
stkret	dd	?			;return pointer
presult	dd	?			;result pointer
pinfo	dd	?			;misc info pointer
parg	dd	?			;pointer to array
iarg2	dd	?			;argument 2
iarg1	dd	?			;argument 1
stk	ends

	entry	XPUT

	push	bp
	mov	bp,sp			;set up stack frame

; Test if parg is an external block.
;
	lds	di,[bp].parg		;pointer to external block
	les	si,[bp].pinfo		;pointer to misc info area
	les	si,es:[si].ptyptab 	;pointer to type table
	mov	ax,[di].xntyp		;block's type word
	cmp	ax,xn			;external block?
	jne	short xfail		;fail if not

	mov	bx,word ptr [bp].iarg2	;subscript 
	dec	bx			;scale 0 to N-1
	mov	ax,[di].xnlen		;size of block in bytes
	shr	ax,1			;convert to words
	sub	ax,3			;max legal subscript
	cmp	bx,ax
	ja	short xfail		;too large

	mov	ax,word ptr [bp].iarg1	;value to store
	shl	bx,1
	add	di,bx
	mov	[di].xndta,ax		;store it

	lds	di,[bp].presult		;point to result area
	mov	[di].sclen, 0		;return null string
	mov	ax, BL_SC
	mov	sp,bp
	pop	bp
	ret

xfail:	mov	ax,FAIL
	mov	sp,bp
	pop	bp
	ret

XPUT	endp

_TEXT	ends
	end	XPUT
