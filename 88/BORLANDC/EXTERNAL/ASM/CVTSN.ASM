	page	80,132
	title	CVTSN Convert String to Numeric
	name	CVTSN

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.


; Convert 2, 4 or 8 character string to 16-bit integer, 32-bit integer
; or real number respectively.
; Useful for reading numeric values in an internal form from a binary
; (raw mode) file.
;
; This function is the inverse of CVTNS(), which converts internal
; numeric values to 2, 4 or 8 character strings.
;
; Load from SPITBOL with statement:
;	LOAD('CVTSN(STRING)')
;
;	Input:	Two, four or eight character string with internal representation
;		of number.
;	Output:	Integer or double precision real in internal form.
;
;		Failure if input is not a 2, 4 or 8 character string.
;
;	Internal representation is little-endian.  That is, the
;	integer 1 could be created (in SPITPC32) using:
;
;		CVTSN(CHAR(1) CHAR(0) CHAR(0) CHAR(0))
;
;	(c) Copyright 1984, 1992 - Catspaw, Incorporated

INCLUDE system.inc
INCLUDE extrn88.inc

	preamble _TEXT

; Upon entry, the stack contains:
;
;	 SP  -->return pointer
;		result area pointer
;		miscellaneous info pointer
;		argument pointer
;
cvstk	struc
cv_bp	dw	?			; saved EBP
cv_rip	dd	?			; return EIP
cv_resp	dd	?			; pointer to result area
cv_misp	dd	?			; pointer to misc info area
cv_argp	dd	?			; pointer to arg string
cv_argl	dw	?			; length of arg string
cvstk	ends

	entry	CVTSN

	push	bp
	mov	bp,sp			;set up stack frame

	sload	es,di,[bp].cv_resp	; pointer to result area
	lea	di,[di].icval		; point to numeric result area
	mov	cx,[bp].cv_argl		; string length
	sload	ds,si,[bp].cv_argp	; string characters
	mov	ax,BL_IC
	cmp	cx,4			; ok if 4
	je	short cvts1		; it represents a 32-bit integer
	cmp	cx,2			; ok if 2
	je	short cvts1		; it represents a 16-bit integer
	mov	ax,BL_RC
	cmp	cx,8			; check if it represents a real
	jne	short cvtfl		; fail if not 8
cvts1:	shr	cx,1			; convert bytes to words
	cld				; direction up
    rep	movsw				; transfer numeric
	mov	sp,bp
	pop	bp
	ret

cvtfl:	mov	ax,FAIL			; return fail
	mov	sp,bp
	pop	bp
	ret
CVTSN	endp

_TEXT	ends
	end	CVTSN
