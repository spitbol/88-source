	page	80,132
	title	FILENAME - External Function to search disk directory.     
	name	FILENAME
	JUMPS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
; Lookup DOS pathname in disk directory.
; Allows ? and * as arbitrary character matches. 
;
; Useful for reading a disk file directory.  Load from SPITBOL with:
;
;	LOAD('FILENAME(STRING)STRING')
;
;	Input:	A pathname string with optional drive specifier.  Pathnames
;		may be up to 66 (MS-DOS) characters long, of the form:
;			 D:\DIR\DIR\...\DIR\NNNNNNNN.EEE,
;		If the filename contains wildcard character(s) (? or *),
;		successive calls may be made with a null string argument to
;		obtain additional matching file directory entries.  The
;		function fails if there is no matching filename, or when
;		the list of wildcard matches is exhausted, or if the
;		pathname string is too long or invalid.
;
;	Output:	A pathname string, with drive specifier.  The string is
;		in the correct format for use in the fourth argument
;		position of the INPUT and OUTPUT functions.
;
;	Sample program to print a file directory from directory C:\TOOLS\BIN:
;
;		LOAD('FILENAME(STRING)STRING')
;		OUTPUT = FILENAME('C:\TOOLS\BIN\*.*')		:F(NONE)
;	LOOP	OUTPUT = FILENAME()				:S(LOOP)
;
;
; v1.1 March 23, 1992 -- modified to operate with Intel DOS Extender.
; v1.2 July 18, 1992 -- modified to operate with 8088 SPITBOL.
;
INCLUDE system.inc
INCLUDE	extrn88.inc

	preamble _TEXT,_DATA

;
;  Describe entry stack
;
stk	struc
stkbp	dw	?			;save bp
stkret	dd	?			;return pointer
presult	dd	?			;far result pointer
pinfo	dd	?			;far misc info pointer
argp	dd	?			;pointer to arg string
argl	dw	?			;length of arg string
stk	ends

z_path	equ	63			; maximum path length
z_fn	equ	12			; file name length with extension

; DTA layout for find first and next match
match	struc
	db	21 dup(?)		; DOS reserved area
 m_atr	db	?			; Result attributes
 m_time	dw	?			; File creation time
 m_date	dw	?			; File creation date
 m_size	dd	?			; File size
 m_fn	db	z_fn+1 dup(?)		; Found file name with zero terminator
match	ends

colon	equ	':'
bslash	equ	'\'

path	struc
 drv	db	?			; drive letter
 col	db	?			; colon
	db	?			; backslash
 dir	db	z_path dup (?)		; directory
	db	0			; zero terminator
path	ends


	entry	FILENAME

	push	bp
	mov	bp,sp

	push	es
	mov	ah,2fh			; Get DTA to ES:BX 
	int	21h
	cld
	mov	word ptr dtaadr, bx	; system DTA
	mov	word ptr dtaadr+2,es
	pop	es
	mov	cx,[bp].argl		; argument string length
	jcxz	short fname3		; jump if null, perform Find Next
	cmp	cx, size path - 1	; check if too large
	ja	fnamex

	mov	byte ptr loaded, 0 	; mark dta empty
	sload	ds,si,[bp].argp		; get pointer to arg in ds:[si]
	lea	di, p_in.drv		; ES:DI = pathname buffer
   rep	movsb				; Copy filename string over
	xor	al, al
	stosb				; Append zero terminator

	spush	es
	spop	ds			; restore function's DS
	lea	dx, p_in.drv		; DS:DX = pathname with terminator
	mov	ah, 4Eh			; search for first directory entry
	jmp	short lookup

fname3:	mov	ah, 4Fh			; search for next directory entry
	cmp	byte ptr loaded, 0	; check if called with non-null string
	je	fnamex			;  earlier.
	spush	es			; copy dta info from our private buffer
	les	di,dtaadr		; system DTA
	lea	si,dta			; this is needed for operation under Windows
	mov	cx,size match
   rep	movsb				; Copy filename string over
	spop	es

lookup:	xor	cx, cx		 	; Attributes = 0
	int	21h			; lookup filename
	jc	short fnamex		; Jump if no match

	mov	byte ptr loaded,1	; dta loaded with search info

	spush	ds			; copy dta info to our private buffer
	lds	si,dtaadr		; system DTA
	lea	di,dta			; this is needed for operation under Windows
	mov	cx,size match
   rep	movsb				; Copy filename string over
	spop	ds

	lea	di, p_in		; ES:DI = p_in
	mov	cx, size path
	cld
	xor	al, al			; Find zero terminator at end
 repne	scasb				;  of input pathname
	neg	cx
	add	cx, size path		; CX = length including zero terminator
	std				; Now scan backward to find last \
	dec	di			; Start reverse scan at zero terminator
	mov	al, bslash		; Object is to decrease CX by length of
 repne	scasb				;  file name component of input string
	jne	short wild_0		; J. if no backslash in string
	inc	cx			; Present, include it in count
	jmp	short wild_1

wild_0:	cmp	byte ptr ses:[di+1].col, colon	; No backslash, check that we
	jne	short wild_1	 	; didn't scan off drive prefix in error
	add	cx, 2			; We did.  Include it in count

wild_1:	cld				; CX = size of input prefix to maintain
	lea	si, p_in
	sload	es,di,[bp].presult	; point to result area
	mov	bx,di			; save for length setting later
	lea	di,[di].scstr		; point to string field of result area
	push	cx
  rep	movsb				; Copy and drive and path prefix
	pop	cx
	lea	si, dta.m_fn		; Result file name from find op
wild_2:	lodsb
	or	al, al			; Copy result name to zero terminator
	je	short wild_3
	stosb
	inc	cx			; Accumulate count of output length
	jmp	short wild_2

wild_3: mov	es:[bx].sclen,cx
	mov	ax,bl_sc		;string result
	
	mov	sp,bp
	pop	bp
	ret

fnamex:	mov	ax,fail
	mov	sp,bp
	pop	bp
	ret


FILENAME endp


_TEXT	ends

_DATA	segment	word 'DATA'

dtaadr	dd	0			; Address of system DTA
loaded	db	0			; Non-zero when dta valid
dta	match	<>			; Abbreviated DTA

p_in	path	<,colon,bslash>		; Path name buffers

_DATA	ends
	end	FILENAME
