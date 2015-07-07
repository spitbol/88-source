	page	80,132
	title	LTRIM External Function
	name	LTRIM

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
; Perform a left end TRIM function
;
; Load from SPITBOL with statement:
;	LOAD('LTRIM(STRING,STRING)STRING')
;
;	Input:	First Argument = String to trim
;		Second Argument= Character to trim (defaults to
;				 blank and horizontal tab)
;
;	Output:	Left trimmed copy of first argument
;
;	(c) Copyright 1991, 1992 - Catspaw, Inc.
;	

INCLUDE system.inc
INCLUDE extrn88.inc

	preamble _TEXT

; Upon entry, the stack contains (all doublewords):
;
;	 SP  -->return pointer
;		arg2 pointer
;		arg1 pointer
;
ltstk	struc
lt_bp	dw	?			; saved EBP
lt_rip	dd	?			; return EIP
lt_resp	dd	?			; pointer to result area
lt_misp	dd	?			; pointer to misc info area
lt_a2p	dd	?			; arg 2 pointer (offset:segment)
lt_a2s	dw	?			; arg 2 length
lt_a1p	dd	?			; arg 1 pointer (offset:segment)
lt_a1s	dw	?			; arg 1 length
ltstk	ends

	entry	LTRIM

	push	bp
	mov	bp,sp			;set up stack frame

	sload	es,bx,[bp].lt_resp	; pointer to response area
	sload	ds,si,[bp].lt_a1p	; pointer to arg 1 string
	mov	word ptr ses:[bx].fsptr,si ; copy to far string
	mov	word ptr ses:[bx].fsptr+2,ds ;  result area
	mov	cx,[bp].lt_a1s		; copy arg 1 string length to
	mov	ses:[bx].fslen,cx	;  result area as well

	jcxz	ltrim4			; return if arg 1 is null

	cld
	mov	dx,cx			; preserve count
	cmp	[bp].lt_a2s,0		; is arg 2 null?
	je	short ltrim2
;
; Here to process with trim character given in arg 2
;
	sload	es,di,[bp].lt_a2p	; pointer to arg 2 string
	mov	al,ses:[di]		; get character to trim
	mov	di,si			; scasb uses es:di
	spush	ds
	spop	es
  rep	scasb				; search for the character
	je	short ltrim1
	inc	cx			; if not found, put back in count
	jmp	short ltrim1
	
;
; Here to process default trim characters of blank and tab
;
ltrim2:	lodsb				; load next char
	cmp	al,9
	je	short ltrim3		; jump if tab
	cmp	al,' '
	jne	short ltrim1		; jump if not blank
ltrim3:	loop	ltrim2			; keep searching
ltrim1:	sub	dx,cx			; compute number of chars removed
	sload	es,bx,[bp].lt_resp	; pointer to response area
	mov	ses:[bx].fslen,cx	; update output count
	add	word ptr ses:[bx].fsptr,dx ; and starting string offset
;
; Here to return
;
ltrim4:	mov	ax,BL_FS               ; return far string result
	mov	sp,bp
	pop	bp
	ret
LTRIM	endp

_TEXT	ends
	end	LTRIM
