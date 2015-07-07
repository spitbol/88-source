	page	80,132
	title	LOGIC External Function Routines
	name	LOGIC
	JUMPS

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
; Perform logical or arithmetic operations on integers and strings
;
; Load from SPITBOL with statement:
;	LOAD('LOGIC(INTEGER,ARG2,ARG3)')
;
;	Input:	First Argument = Operation Specifier:
;			1 = Return Logical NOT of ARG2
;			2 = Return ARG2 AND  Arg3
;			3 = Return ARG2 OR   Arg3
;			4 = Return ARG2 XOR  Arg3
;			5 = Return ARG2 NAND Arg3
;			6 = Return ARG2 NOR  Arg3
;			7 = Return ARG2 + ARG3
;			8 = Return ARG2 - ARG3
;			9 = Return ARG2 * ARG3 (unsigned)
;		       10 = Return ARG2 / ARG3 (unsigned)
;		       11 = Return ARG2 shifted left ARG3 bits
;		       12 = Return ARG2 shifted right ARG3 bits (unsigned)
;		       13 = Return ARG2 shifted right ARG3 bits (sign extension)
;		       14 = Return ARG2 rotated left ARG3 bits
;		       15 = Return ARG2 rotated right ARG3 bits
;		       16 = Return integer from digit string in ARG2, base ARG3
;		       17 = Return digit string from integer in ARG2, base ARG3
;			      (conversion base defaults to 16 if null string)
;
;	ARG2 and ARG3 may be two strings, two integers, or an integer
;			and a string.
;
;	Output:	Result of operation.
;		Function fails if:
;		1. Operation specifier out of range
;		2. Arg2 and Arg3 are strings of different length
;		3. Arg2 or Arg3 are strings longer than 512 characters.
;		4. The arguments are not strings or integers
;
;	(c) Copyright 1985, 1992 - Catspaw, Inc.
;

INCLUDE system.inc
INCLUDE extrn88.inc

	preamble _TEXT

; Upon entry, the stack contains:
;
;	 SP  -->return pointer
;		result area pointer
;		miscellaneous info pointer
;		arg3 pointer
;		arg2 pointer
;		arg 1 integer type code
;
lgstk	struc
lg_bp	dw	?			; saved BP
lg_rip	dd	?			; return IP
lg_resp	dd	?			; pointer to result area
lg_misp	dd	?			; pointer to misc info area
lg_a3p	dd	?			; arg 3 pointer (offset:segment)
lg_a2p	dd	?			; arg 2 pointer (offset:segment)
lg_op	dw	?			; arg 1 = opcode requested (integer)
lgstk	ends

locvars	struc
base	dw	?			; conversion base
temp	dw	?
envtp	db	?
	db	?
cvbuf	db	32 dup(?)		; conversion buffer
locvars	ends
temps	equ	[bp-size locvars]	; access to locvars structure

	entry	LOGIC

	push	bp
	mov	bp,sp			;set up stack frame
	sub	sp,size locvars		; create stack temp area

	mov	bx,[bp].lg_op		; get operation requested
	dec	bx			; op - 1
	js	near ptr logicf	 	; j. if bad
	cmp	bx,17-1
	ja	near ptr logicf	  	; j. if bad

	sload	es,si,[bp].lg_misp	; pointer to miscellaneous area
	mov	al,ses:[si].env
	mov	temps.envtp,al		; record whether 16 or 32-bit ints
	sload	es,si,ses:[si].ptyptab	; ES:SI = pointer to type table
					; check for conversion special cases:
	je	cvdig			; convert integer to digit string
	cmp	bl,16-1
	je	cvint	 		; convert digit string to integer

	shl	bx,1			; op times 2

	sload	ds,di,[bp].lg_a3p	; DS:DI = pointer to third argument
	mov	dx,[di].sctyp		; load arg 3 type word to DX
	sload	ds,di,[bp].lg_a2p	; DS:DI = pointer to second argument
	mov	ax,[di].sctyp		; load arg 2 type word to AX

	cmp	ax,ic			; access SPITBOL's type table
	jne	logic16			; j. if arg 2 not integer

	cmp	dx,ic			; arg 2 is integer. Check arg 3's type
	jne	short logic5		; j. if arg 2 integer, arg 3 is not

					; integer op integer
	sload	es,si,[bp].lg_a3p	; ES:SI = pointer to third argument
	mov	cx,word ptr ses:[si].icval	; arg 3 value
	mov	si,word ptr ses:[si].icval+2	; no-harm in presuming 32-bit
logic4:	mov	ax,word ptr [di].icval	; arg 2 value
	mov	dx,word ptr [di].icval+2; no-harm in presuming 32-bit

	cmp	temps.envtp,t_tc16
	je	short logic21
	add	bx,offset i_call32 - offset i_call16
logic21:
	call	cs:i_call16[bx]		; dispatch on operation
	sload	es,di,[bp].lg_resp	; result area pointer
	mov	word ptr ses:[di].icval,ax ; save integer result
	mov	word ptr ses:[di].icval+2,dx
	mov	ax,BL_IC		; signal integer result
	mov	sp,bp
	pop	bp
	ret

logic5:	cmp	dx,sc			; arg2 is integer, test arg3
	jne	near ptr logicf		; fail if not I-S
	or	bx,bx			; Test for NOT operator
	je	logic4			;  and treat like I-I

	sload	es,si,[bp].lg_a3p	; ES:SI = pointer to third argument
logic6:	mov	cx,ses:[si].sclen	; integer-string.  Load string length.
	mov	dx,word ptr [di].icval	; load integer value (possibly lsh)
	jcxz	short logic3		; jump if null string
	cmp	cx,buflen		; check size
	ja	near ptr logicf		; fail if too large
	lea	si,[si].scstr	 	; point to first char of string in gs:esi
	spush	es			; create source pointer
	spop	ds			; in DS:[SI]
	sload	es,di,[bp].lg_resp	; point to result area
	mov	ses:[di].sclen,cx	; save resultant string length
	lea	di,[di].scstr		; point to resultant string buffer

logic14:				; perform string-integer ops
	lodsb				; next char
	call	cs:s_call[bx]		; perform op from dl to al
	stosb				; put into buffer
	loop	logic14			; integer in dl is constant
	mov	ax,BL_SC	        ; return string result
	mov	sp,bp
	pop	bp
	ret

logic3:	sload	es,di,[bp].lg_resp	; return null string result
	mov	ses:[di].sclen,0
	mov	ax,BL_SC
	mov	sp,bp
	pop	bp
	ret

logic16:				; arg 2 is not an integer
	cmp	ax,sc			; is it a string?
	jne	near ptr logicf		; fail if not arg 2 string or integer
	cmp	dx,sc			; arg 2 (DS:DI) is string, check arg 3
	je	short logic17		; jump if string-string
	mov	si,di			; assume must be string-integer and swap
	spush	ds			; swap segments as well as offsets
	spush	es
	sload	ds,di,[bp].lg_a3p	; DS:DI = pointer to third argument
	cmp	dx,ic			; check assumption
	je	logic6			; jump if string-integer
	jmp	near ptr logicf		; illegal arg 3, fail

logic17:				; here for string-string
	sload	es,si,[bp].lg_a3p	; ES:SI = pointer to third argument
	or	bx,bx			; if NOT, dup arg2 to arg3
	jne	short logic19
	mov	si,di
	spush	ds
	spop	es

logic19:				; perform string-string ops
	mov	cx,ses:[si].sclen	; compare string lengths.
	cmp	cx,[di].sclen
	jne	near ptr logicf		; fail if different legnths
	jcxz	logic3			; if null string, return null string
	cmp	cx,buflen		; check size
	ja	near ptr logicf		; fail if too large
	lea	si,[si].scstr	 	; point to first char of arg 3 string
	lea	di,[di].scstr		; point to first char of arg 2 string
	push	bp			; need one more register
					; cheat here:  know that arg 2 segment
					; will be the same as arg 3 segment = DS
	sload	es,bp,[bp].lg_resp	; point to result area
	xchg	di,si
	xchg	di,bp			; es:di = destination, ds:bp = arg 3
	mov	ses:[di].sclen,cx	; save resultant string length
	lea	di,[di].scstr		; point to resultant string buffer

logic20:
	lodsb				; next char from arg 2
	mov	dl,ds:[bp]		; next char from arg 3
	inc	bp
	call	cs:s_call[bx]		; perform op from dl to al
	stosb				; put into buffer
	loop	logic20			; do more
	pop	bp
	mov	ax,BL_SC                ; return string result in buffer
	mov	sp,bp
	pop	bp
	ret

LOGIC	endp

;
; Convert digits in second argument (string) to an integer.
; The third (integer) argument provides the conversion base.  If it is 0,
; base 16 (hexidecimal) is assumed.  Otherwise it must be an integer
; between 2 and 36.
;
	procx	cvint

	sload	ds,di,[bp].lg_a3p	; load pointer to third argument
	mov	ax,[di].ictyp		; load type word
	cmp	ax,ic			; check for integer
	mov	bx,word ptr [di].icval	; presume integer value
	je	short cvint0		; jump if so
	cmp	ax,sc			; check for null string
	jne	near ptr logicf		; illegal third argument
	mov	bx,[di].sclen		; string.  Is it the null string?
	or	bx,bx			;
	jne	near ptr logicf		; fail if arg 3 is non-null

cvint0:	or	bx,bx			; default to 16 if base is 0
	jne	short cvint1
	mov	bl,16

cvint1:	cmp	bx,2			; base must be between 2 and 36
	jl	near ptr logicf
	cmp	bx,36
	ja	near ptr logicf
	mov	temps.base,bx		; save base

	sload	ds,di,[bp].lg_a2p	; load pointer to second argument
	mov	ax,[di].sctyp		; load type word
	cmp	ax,sc			; verify that this is a string
	jne	near ptr logicf		; fail if not
	mov	cx,[di].sclen		; load string length
	lea	si,[di].scstr		; load pointer to string
	xor	bx,bx			; initialize output accumulator DI:BX
	xor	di,di
	jcxz	short cvint8		; beware null string
	xor	ax,ax			; clear upper part of character reg

cvint2:	lodsb
	cmp	al,'a'			; fold lower case to upper case
	jb	short cvint3
	cmp	al,'z'
	ja	short cvint3
	sub	al,'a'-'A'              ; fold

cvint3:	cmp	al,'0'			; check for legal digit
	jl	near ptr logicf		; fail if invalid
	cmp	al,'9'
	jbe	short cvint4		; digit between 0 and 9
	cmp	al,'A'
	jb	near ptr logicf		; invalid digit
	cmp	al,'Z'
	ja	near ptr logicf		; invalid digit
	sub	al,'A'-'9'-1		; map letters to range above digits

cvint4:	sub	al,'0'			; convert digit to integer
	cmp	al,byte ptr temps.base	; check if legal
	jae	near ptr logicf
	xchg	bx,ax			; exchange with accumulator
	mul	temps.base 		; multiply by base
	or	di,di			; have to do upper half?
	je	short cvint7		; jump if not
	xchg	ax,di			; save lower result
	mov	temps.temp,dx
	mul	temps.base		; multiply upper half
	adc	ax,temps.temp
	mov	dx,ax
	mov	ax,di

cvint7:	add	ax,bx			; add in new digit
	adc	dx,0
	mov	di,dx			; save upper half of result
	xchg	ax,bx			; restore accumulator to di:bx
	loop	cvint2			; get next digit

cvint8:	sload	ds,si,[bp].lg_resp	; load result pointer
	mov	word ptr [si].icval,bx  ; store result
	mov	word ptr [si].icval+2,di
	mov	ax,BL_IC		; return integer
	mov	sp,bp
	pop	bp
	ret
cvint	endp


;
; Convert number in second argument (integer) to a digit string.
; The third (integer) argument provides the conversion base.  If it is 0,
; base 16 (hexidecimal) is assumed.  Otherwise it must be an integer
; between 2 and 36.
;
	procx	cvdig

	sload	ds,di,[bp].lg_a3p	; load pointer to third argument
	mov	ax,[di].ictyp		; load type word
	cmp	ax,ic			; check for integer
	mov	bx,word ptr [di].icval	; presume integer value
	mov	dx,word ptr [di].icval+2
	je	short cvdig0		; jump if so
	cmp	ax,sc			; check for null string
	jne	near ptr logicf		; illegal third argument
	mov	bx,[di].sclen		; string.  Is it the null string?
	xor	dx,dx
	or	bx,bx
	jne	near ptr logicf		; fail if arg 3 is non-null

cvdig0:	cmp	temps.envtp,t_tc16
	je	short cvdg0A		; jump if 16-bit
	or	dx,dx
	jne	near ptr logicf		; too large
cvdg0A:	or	bx,bx			; default to 16 if base is 0
	jne	short cvdig1		
	mov	bl,16

cvdig1:	cmp	bx,2			; base must be between 2 and 36
	jl	near ptr logicf
	cmp	bx,36
	ja	short near ptr logicf
	mov	temps.base,bx		; save base

	sload	ds,di,[bp].lg_a2p 	; load pointer to second argument
	mov	ax,[di].ictyp		; load type word
	cmp	ax,ic			; verify that this is an integer
	jne	short near ptr logicf	; fail if not
	mov	ax,word ptr [di].icval	; load integer value
	xor	dx,dx
	cmp	temps.envtp,t_tc32
	jne	short cvdg1A
	mov	dx,word ptr [di].icval+2 ; get top half of 32-bit integer
cvdg1A:	mov	temps.temp,0		; count digits produced
	spush	ss
	spop	es
	lea	di,temps.cvbuf+31	; produce digits backwards
	std

cvdig2:	or	dx,dx			; arg 2(msh) == 0?
	je	short cvd10
	mov	cx,32
	xor	si,si			; remainder developed here
cvd1:	shl	ax,1
	rcl	dx,1
	rcl	si,1
	cmp	si,bx
	jb	short cvd3
	sub	si,bx
	inc	ax
cvd3:	loop	cvd1
	mov	cx,si			; get remainder
	jmp	short cvd2

cvd10:	div	bx
	mov	cx,dx			; remainder
	xor	dx,dx
	
cvd2:	cmp	cl,9			; remainder more than fits in digits?
	jle	short cvdig3		; j. if ok
	add	cl,'A'-'9'-1		; add in bias to letters
cvdig3:	add	cl,'0'
	xchg	ax,cx			; prepare to store
	stosb				; store digit in buffer
	xchg	ax,cx			; restore quotient in dx:ax
	inc	temps.temp		; count digit
	mov	si,ax			; all done?
	or	si,dx
	jne	cvdig2
	mov	si,di			; now dump into result area
	inc	si			; first digit to output
	spush	es
	spop	ds			; source pointer in ds:esi
	cld
	sload	es,di,[bp].lg_resp	; result pointer
	mov	cx,temps.temp
	mov	ses:[di].sclen,cx	; string length
	lea	di,[di].scstr		; starting position of string
   rep	movsb				; copy string
	mov	ax,BL_SC		; return string result
	mov	sp,bp
	pop	bp
	ret

cvdig	endp


;
; Here to return failing.
;
	procx	logicf

	mov	ax,FAIL			; return failure.
	mov	sp,bp
	pop	bp
	ret				; far return
logicf	endp


int_ops	proc	near

; 32-bit integer-integer operates from si:cx to dx:ax
i_and32: and	ax, cx
	and	dx, si
	ret

i_or32:	or	ax, cx
	or	dx, si
	ret

i_xor32: xor	ax, cx
	xor	dx, si
	ret

i_nand32: and	ax, cx
	and	dx, si
i_not32: not	ax
	not	dx
	ret

i_nor32: or	ax, cx
	or	dx, si
	not	ax
	not	dx
	ret

i_plus32: add	ax,cx
	adc	dx,si
	ret

i_sub32: sub	ax,cx
	sbb	dx,si
	ret

i_mult32: or	si,si
	jne	short toobig
	mul	cx		; 16-bit only!
	ret

i_div32: or	si,si
      	jne	short toobig	
	div	cx		; 16-bit only!
	ret
toobig:	pop	ax
	jmp	near ptr logicf


i_shl32: and	cx,1fh
i_shl1:	shl	ax,1
	rcl	dx,1
	loop	i_shl1
	ret

i_shr32: and	cx,1fh
i_shr1:	shr	dx,1
	rcr	ax,1
	loop	i_shr1
	ret

i_sar32: and	cx,1fh
i_sar1:	sar	dx,1
	rcr	ax,1
	loop	i_sar1
	ret

i_rol32: and	cx,1fh
i_rol1:	rcl     ax,1
	rcl     dx,1
	adc     ax,0
	loop	i_rol1
	ret

i_ror32: and	cx,1fh
i_ror1:	mov     bx,dx
	shr     bx,1
	rcr     ax,1
	rcr     dx,1
	loop	i_ror1
	ret

; 16-bit integer-integer operates from cx to ax
i_and16: and	ax, cx
	ret

i_or16:	or	ax, cx
	ret

i_xor16: xor	ax, cx
	ret

i_nand16: and	ax, cx
i_not16:  not	ax
	ret

i_nor16: or	ax, cx
	not	ax
	ret

i_plus16: add	ax,cx
	ret

i_sub16: sub	ax,cx
	ret

i_mult16: mul	cx
	ret

i_div16: xor	dx,dx
	div	cx
	ret

i_shl16: shl	ax,cl
	ret

i_shr16: shr	ax,cl
	ret

i_sar16: sar	ax,cl
	ret

i_rol16: rol	ax,cl
	ret

i_ror16: ror	ax,cl
	ret

int_ops	endp


s_ops	proc	near

; string-string and integer-string operate from dl to al
s_and:	and	al, dl
	ret

s_or:	or	al, dl
	ret

s_xor:	xor	al, dl
	ret

s_nand:	and	al, dl
s_not:	not	al
	ret

s_nor:	or	al, dl
	not	al
	ret

s_plus:	add	al, dl
	ret

s_sub:	sub	al, dl
	ret

s_mult:	mul	dl
	ret

s_div:	xor	ah,ah
	div	dl
	ret

s_shl:	xchg	cl,dl
	shl	al,cl
	xchg	cl,dl
	ret

s_shr:	xchg	cl,dl
	shr	al,cl
	xchg	cl,dl
	ret

s_sar:	xchg	cl,dl
	sar	al,cl
	xchg	cl,dl
	ret

s_rol:	xchg	cl,dl
	rol	al,cl
	xchg	cl,dl
	ret

s_ror:	xchg	cl,dl
	ror	al,cl
	xchg	cl,dl
	ret

s_ops	endp

	align   2

i_call16 dw	i_not16		; dispatch table for 16-bit integer ops (ax,cx)
	dw	i_and16
	dw	i_or16
	dw	i_xor16
	dw	i_nand16
	dw	i_nor16
	dw	i_plus16
	dw	i_sub16
	dw	i_mult16
	dw	i_div16
	dw	i_shl16
	dw	i_shr16
	dw	i_sar16
	dw	i_rol16
	dw	i_ror16
i_call32 dw	i_not32		; dispatch table for 32-bit integer ops (dx:ax,si:cx)
	dw	i_and32
	dw	i_or32
	dw	i_xor32
	dw	i_nand32
	dw	i_nor32
	dw	i_plus32
	dw	i_sub32
	dw	i_mult32
	dw	i_div32
	dw	i_shl32
	dw	i_shr32
	dw	i_sar32
	dw	i_rol32
	dw	i_ror32

s_call	dw	s_not			; dispatch table for string ops
	dw	s_and
	dw	s_or
	dw	s_xor
	dw	s_nand
	dw	s_nor
	dw	s_plus
	dw	s_sub
	dw	s_mult
	dw	s_div
	dw	s_shl
	dw	s_shr
	dw	s_sar
	dw	s_rol
	dw	s_ror

_TEXT	ends
	end	LOGIC
