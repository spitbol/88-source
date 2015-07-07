	page	80,132
	title	CVTNS Convert Numeric to String
	name	CVTNS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.



; Convert integer or real number to 2, 4 or 8 character string respectively.
; Useful for writing numeric values in an internal form to a binary
; (raw mode) file.
;
; This function is the inverse of CVTSN(), which converts 2, 4 or 8 character
; strings to internal numeric values.
;
; Load from SPITBOL with statement:
;	LOAD('CVTNS(NUMERIC)STRING')
;
;	Note:	We use the unrecognized keyword "NUMERIC" so that
;		SPITBOL passes a pointer to the unconverted argument.
;		The function will then figure out if it was passed an
;		integer or real number.  DO NOT SUBSTITUTE A SPECIFIC
;		DATATYPE (SUCH AS "INTEGER" OR "REAL").
;
;	Input:	Integer or double precision real in internal form.
;	Output:	Two (if integer & SPITPC16), four (if integer & SPITPC32)
;		or eight (if real) byte string with internal representation
;		of number.
;
;		Failure if input is any datatype except real or integer.
;		Note that CVTNS("1.23") will fail because the input
;		argument is a string.  Use CVTNS(+"1.23") to convert a
;		string to a true numeric before calling function.
;
;	(c) Copyright 1984, 1992 - Catspaw, Incorporated

INCLUDE system.inc
INCLUDE extrn88.inc

	preamble _TEXT

; Upon entry, the stack contains:
;
;	 SP  -->return pointer
;		return cs
;		result area pointer
;		miscellaneous info pointer
;		argument pointer
;
cvstk	struc
cv_bp	dw	?			; saved EBP
cv_rip	dd	?			; return EIP
cv_resp	dd	?			; pointer to result area
cv_misp	dd	?			; pointer to misc info area
cv_argp	dd	?			; arg pointer (offset:segment)
cvstk	ends

	entry	CVTNS

	push	bp
	mov	bp,sp			;set up stack frame
	cld

	sload	es,si,[bp].cv_misp	; pointer to miscellaneous area
	mov	dl,ses:[si].env		; load environment type
	sload	es,si,ses:[si].ptyptab	; ES:SI = pointer to type table
	sload	ds,bx,[bp].cv_argp	; fetch pointer to argument
	mov	ax,[bx]			; load argument type
	lea	bx,[bx].icval		; point to value
	cmp	ax,ic			; integer?
	je	short cvtis		; if so, convert integer to string
	cmp	ax,rc			; real?
	jne	short cvtfl		; if not, return failure
	mov	ax,8			; 8 bytes in a real
	sload	es,di,[bp].cv_resp	; pointer to result area
	lea	di,[di].sclen		; point to result length word
	mov	si,bx
	stosw				; store in sclen word
	movsw				; transfer lsh of real
	movsw				; transfer lsh of real
	jmp	short cvtret		; transfer msh of real and return

cvtis:	mov	ax,4			; 4 bytes in an integer if SPITPC32
	cmp	dl,t_tc32
	je	short cvtis1		; jump if 32-bit
	mov	ax,2			; SPITPC16 uses 16-bit integers
cvtis1:	sload	es,di,[bp].cv_resp	; pointer to result area
	lea	di,[di].sclen		; point to result length word
	mov	si,bx
	stosw				; store in sclen word
	cmp	dl,t_tc16
	je	short cvtrt1
cvtret:	movsw				; transfer integer
cvtrt1:	movsw				; transfer last word
	mov	ax,BL_SC		; return numeric as a string
	mov	sp,bp
	pop	bp
	ret

cvtfl:	mov	ax,FAIL		; return fail
	mov	sp,bp
	pop	bp
	ret
CVTNS	endp

_TEXT	ends
	end	CVTNS
