	page	80,132
	title	TRUNCATE - External Function to emulate SNOBOL4+ TRUNCATE()
	name	PATHNAME

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;  Truncates an output file at its current file position.
;
;	LOAD("TRUNCATE(FILE)")
;
;	Call with TRUNCATE(channel)
;
;  Uses little-known DOS feature that a 0-length write truncates a file.
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
presult	dd	?			;far result pointer
pinfo	dd	?			;far misc info pointer
pfcb	dd	?			;pointer to file control block
stk	ends

	entry	TRUNCATE

	push	bp
	mov	bp,sp			;set up stack frame

	sload	ds,si,[bp].pfcb		;pointer to fcb
	mov	si,[si].fcbiob		;pointer to ioblock
	mov	bx,[si].iobfdn		;file handle
	mov	si,[si].iobbfb		;pointer to buffer block
	mov	dx,word ptr [si].bfboff	;load logical offset corresponding
	mov	cx,word ptr [si].bfboff+2; to start of buffer
	add	dx,[si].bfbnxt		;fine-tune with next position
	adc	cx,0			;to be written in the buffer
	mov	ax,4200h			;set absolute file position here
	int	21h
	mov	word ptr [si].bfbcur, ax;update current physical file
	mov	word ptr [si].bfbcur+2,dx; position

	xor	cx,cx			;zero length write
	mov	ah,40h			;will truncate the file
	int	21h
	sload	es,di,[bp].presult	;pointer to result area
	mov	es:[di].sclen,cx	;null string
	mov	ax,bl_sc		;return string
	mov	sp,bp
	pop	bp
	ret

TRUNCATE endp

_TEXT	ends
	end	TRUNCATE
