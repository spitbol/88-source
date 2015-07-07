	page	80,132
	title	XCREATE - Convert Vector to an External Block
	name	XCREATE
	JUMPS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;	Usage:
;
;	LOAD("XCREATE(VECTOR)")
;
;	Call with:
;		A = ARRAY(1000)
;		XCREATE(A)		;* convert to 1001 slot external block
;
;	Converts the incoming one-dimensional array to an external
;	block with one or more 2-byte slots.  These slots can be used to store
;	arbitrary data.  Use XPUT and XGET to save and load data from
;	the external block.
;
;	Returns the null string if successful, fails if incoming arg
;	is not a vector.
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
stk	ends

	entry	XCREATE

	push	bp
	mov	bp,sp			;set up stack frame

; Test if arg is a one-dimensional array (vector).
;
	lds	di,[bp].parg		;pointer to external block
	les	si,[bp].pinfo		;pointer to misc info area
	les	si,es:[si].ptyptab 	;pointer to type table
	mov	ax,[di].vctyp		;type word
	cmp	ax,vc			;vector?
	jne	short xfail		;fail if not
	mov	ax,xn     		;XNBLK block type
	mov	[di].xntyp,ax		;convert to an external block
	mov	ax,[di].vclen		;size of vector block
	mov	[di].xnlen,ax		;size of external block

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

XCREATE	endp

_TEXT	ends
	end	XCREATE
