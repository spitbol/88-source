	page	80,132
	title	PATHNAME - External Function to Emulate SNOBOL4+ PATHNAME()
	name	PATHNAME

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;  Returns file name associated with a channel.
;
;	LOAD("PATHNAME(FILE)STRING")
;
;	Call with RESULT = PATHNAME(channel)
;
;  Bug: Does not return the name of a file that has been specified on
;	the command line or via an environment variable.
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

	entry	PATHNAME

	push	bp
	mov	bp,sp			;set up stack frame

	cld
	sload	ds,si,[bp].pfcb		;pointer to fcb
	mov	si,[si].fcbiob		;pointer to ioblock
	mov	si,[si].iobfnm		;pointer to filename fcb
	sload	es,di,[bp].presult	;pointer to result area
	movsw				;copy type word
	lodsw				;load length
	stosw				;and copy it
	cmp	ax,buflen		;check against max permissible
	jle	short pn1
	mov	ax,buflen		;truncate
pn1:	add	ax,3			;round to words and truncate
	shr	ax,1
	mov	cx,ax
   rep	movsw				;transfer name
	mov	ax,bl_sc		;return string
	mov	sp,bp
	pop	bp
	ret

PATHNAME endp

_TEXT	ends
	end	PATHNAME
