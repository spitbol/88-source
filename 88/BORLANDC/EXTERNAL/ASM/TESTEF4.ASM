	page	80,132
	title	TESTEF4 - TEST External Function Routines
	name	TESTEF4

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;  Test access to arguments and return of far string:
;	LOAD("TESTEF4(INTEGER,REAL,STRING)")
;
;	Call with TESTEF4(100, 1000.0, "ABCD")
;
;	Returns far string result if arguments were found as expected,
;	otherwise a message about which argument was bad.
;
;
;
INCLUDE system.inc			; select MS-DOS or OS/2 conventions
INCLUDE	extrn88.inc

	preamble _TEXT

;
;  Describe entry stack
;
stk	struc
stkbp	dw	?			;save ebp
stkret	dd	?			;return pointer
presult	dd	?			;far result pointer
pmisc	dd	?			;far misc info pointer
parg3	dd	?			;pointer to arg 3 string
larg3	dw	?			;length of arg 3 string
rarg2	dq	?			;real arg 2
iarg1	dd	?			;long integer arg 1
stk	ends

	entry	TESTEF4

	push	bp
	mov	bp,sp			;set up stack frame

	cld
	mov	si, offset cgroup:bad1	;prepare bad arg1 message
	mov	ax,word ptr [bp].iarg1	;load arg 1 integer
	mov	dx,word ptr [bp].iarg1+2
	cmp	ax,100			;test for correct value
	jne	short test1		;jump if not as expected
	cmp	dx,0
	jne	short test1		;jump if not as expected

	push	ss			;set up ES:DI to point to arg2
	pop	es
	lea	di,[bp].rarg2
	mov	si,offset cgroup:onethou
	mov	cx,4
   repe	cmpsw
	mov	si, offset cgroup:bad2	;prepare bad arg2 message
	jne	short test1

	mov	si, offset cgroup:bad3a;prepare bad arg3 message
	sload	es,di,[bp].parg3	;pointer to arg3 string
	mov	cx,[bp].larg3		;length of arg3 string
	cmp	cx,zabcd		;check length
	jne	short test1
	mov	si,offset cgroup:abcd	;test characters
   repe	cmpsb				;compare string with expected
	mov	si, offset cgroup:bad3b;prepare bad arg3 message
	jne	short test1		;mismatch

	sload	es,di,[bp].presult	;pointer to result area
	mov	ax,zfarstr		;size of string
	mov	ses:[di].fslen,ax	;size of string
	lea	ax, farstr 		;far string offset
	sstore	ses:[di].fsptr,ax,ds	;far string pointer
	mov	ax,bl_fs		;return far string result
	mov	sp,bp
	pop	bp
	ret

;
; Return ASCIIZ string at DS:SI.  Copy up to zero.
;
test1:	sload	es,di,[bp].presult	;point to result area
	mov	bx,di			;save
	lea	di,[di].scstr		;point to string data in result area
	xor	dx,dx			;count characters moved

test2:	lodsb				;get next character
	or	al,al			;test for end of string
	je	short test3		;jump if so
	stosb				;move to result area
	inc	dx			;count it
	jmp	test2			;get next character

test3:	mov	es:[bx].sclen,dx	;store length in result area
  	mov	ax,bl_sc		;string result
	mov	sp,bp
	pop	bp
	ret

TESTEF4	endp

onethou	dq	1000.0
farstr	db	"This is a far string result."
zfarstr	equ	$-farstr
abcd	db	"ABCD"
zabcd	equ	$-abcd
bad1	db	"First argument not integer 100",0
bad2	db	"Second argument not real 1000.0",0
bad3a	db	"Third argument string length wrong",0
bad3b	db	"Third argument not string 'ABCD'",0
_TEXT	ends
	end	TESTEF4
