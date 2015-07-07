	page	80,132
	title	XGET - Get Integer from an External Block
	name	XGET
	JUMPS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;	Usage:
;
;	LOAD("XGET(INTEGER,EXTERNAL)INTEGER")
;
;	Argument 1 is the subscript of the desired slot, 1 to N.
;	Argument 2 is the external block produced by XCREATE.
;
;	Call with:
;		I = XGET(456, X)	;* retrieve integer X[456]
;
;	Returns integer if successful, fails if incoming
;	argument 1 is out of range, or argument 2 not an external block.
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
iarg	dd	?			;subscript
stk	ends

	entry	XGET

	push	bp
	mov	bp,sp			;set up stack frame

; Test if parg is an external block.
;
	lds	di,[bp].parg		;pointer to external block
	les	si,[bp].pinfo		;pointer to misc info area
	les	si,es:[si].ptyptab 	;pointer to type table
	mov	ax,[di].xntyp		;type word
	cmp	ax,xn			;external block?
	jne	short xfail		;fail if not

	mov	bx,word ptr [bp].iarg	;subscript
	dec	bx			;scale 0 to N-1
	mov	ax,[di].xnlen		;size of block in bytes
	shr	ax,1			;convert to words
	sub	ax,3			;max legal subscript
	cmp	bx,ax
	ja	short xfail		;too large

	shl	bx,1
	add	di,bx
	mov	ax,[di].xndta		;load value

	lds	di,[bp].presult		;point to result area
	mov	word ptr [di].icval, ax	;return integer
	cwd
	mov	word ptr [di].icval+2, dx
	mov	ax, BL_IC
	mov	sp,bp
	pop	bp
	ret

xfail:	mov	ax,FAIL
	mov	sp,bp
	pop	bp
	ret

XGET	endp

_TEXT	ends
	end	XGET
