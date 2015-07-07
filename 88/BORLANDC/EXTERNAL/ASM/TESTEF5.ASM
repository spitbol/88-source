	page	80,132
	title	TESTEF5 - TEST External Function Routines
	name	TESTEF5

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;  Test access to arguments and return of external data block:
;	LOAD("TESTEF5(INTEGER,REAL,STRING,OTHER)")
;
;	Call with:
;		X = TESTEF5(100, 1000.0, "ABCD",)
;		TERMINAL = TESTEF5(100, 1000.0, "ABCD", X)
;
;	Returns external data block if first three arguments were found
;	as expected on first call, "OK" on second call.
;	Otherwise a message about which argument was bad.
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
parg4	dd	?			;pointer to arg 3 string
parg3	dd	?			;pointer to arg 3 string
larg3	dw	?			;length of arg 3 string
rarg2	dq	?			;real arg 2
iarg1	dd	?			;long integer arg 1
stk	ends

	entry	TESTEF5

	push	bp
	mov	bp,sp			;set up stack frame

	cld
	mov	si, offset cgroup:bad1	;prepare bad arg1 message
	mov	ax,word ptr [bp].iarg1	;load arg 1 integer
	mov	dx,word ptr [bp].iarg1+2
	cmp	ax,100			;test for correct value
	jne	short test1a		;jump if not as expected
	cmp	dx,0
	jne	short test1a		;jump if not as expected

	push	ss			;set up ES:DI to point to arg2
	pop	es
	lea	di,[bp].rarg2
	mov	si,offset cgroup:onethou
	mov	cx,4
   repe	cmpsw
	mov	si, offset cgroup:bad2	;prepare bad arg2 message
	jne	short test1a

	mov	si, offset cgroup:bad3a;prepare bad arg3 message
	sload	es,di,[bp].parg3	;pointer to arg3 string
	mov	cx,[bp].larg3		;length of arg3 string
	cmp	cx,zabcd		;check length
	jne	short test1
	mov	si,offset cgroup:abcd	;test characters
   repe	cmpsb				;compare string with expected
	mov	si, offset cgroup:bad3b;prepare bad arg3 message
test1a:	jne	short test1		;mismatch
;
; Test if arg4 is the null string (first call).  Note that arg4
; is unconverted, so we work with raw SPITBOL data blocks.
;
	sload	es,di,[bp].parg4	;pointer to arg4 block
	mov	ax,ses:[di].sctyp	;type word
	mov	cx,ses:[di].sclen	;type word
	sload	es,si,[bp].pmisc	;pointer to misc info area
	sload	es,si,ses:[si].ptyptab	;pointer to type table
	cmp	ax,sc			;string?
	jne	short test0		;jump if not
	mov	si, offset cgroup:bad4	;presume bad arg4
	or	cx,cx			;null string?
	jne	short test1		;if not, send error message

;
; This is the first call, made with null string fourth argument.
; Create buflen-byte external block.
;
	sload	es,di,[bp].presult	;pointer to result area
	mov	cx,buflen		;size of data
	mov	ses:[di].xnlen,cx	;size of data
	lea	di,[di].xndta		;start of dta
	xor	al,al
testa:	stosb				;store next byte
	inc	al			;increment it
	loop	testa			;continue until done
	mov	ax,bl_xn		;return external data
	mov	sp,bp
	pop	bp
	ret

;
; Fourth argument is not null string.  Is this the external data
; we provided on the first call?
;
test0:	cmp	ax,xn			;external data?
	mov	si, offset cgroup:bad5	;prepare error message
	jne	short test1		;jump if not external
	mov	si, offset cgroup:bad6	;prepare error message
	sload	es,di,[bp].parg4	;pointer to arg4 block
	mov	cx,ses:[di].xnlen	;length of data
	cmp	cx,buflen+(xndta-xntyp)	;check size (+4 for type/length words)
	jne	short test1		;jump if size wrong
	lea	di,[di].xndta		;es:edi points to data
	sub	cx,(xndta-xntyp)	;adjust count to exclude type/length words
	xor	al,al			;first data value
	mov	si,offset cgroup:bad7	;prepare error message
testb:	scasb				;compare
	jne	short test1		;if data error
	inc	al			;next data value
	loop	testb			;continue compare

	mov	si, offset cgroup:ok	;looking good

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

TESTEF5	endp

onethou	dq	1000.0
abcd	db	"ABCD"
zabcd	equ	$-abcd
ok	db	"OK",0
bad1	db	"First argument not integer 100",0
bad2	db	"Second argument not real 1000.0",0
bad3a	db	"Third argument string length wrong",0
bad3b	db	"Third argument not string 'ABCD'",0
bad4	db	"Fourth argument was not null string",0
bad5	db	"Fourth argument not external data",0
bad6	db	"Fourth argument (external data) size incorrect",0
bad7	db	"Fourth argument (external data) contents bad",0
_TEXT	ends
	end	TESTEF5
