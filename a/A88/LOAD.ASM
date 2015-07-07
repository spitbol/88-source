	title	LOAD - Load External Function Routines
	page	80,132
; Copyright (C)  1988-1998  Mark Emmer

; SPITBOL is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.


	name	load
	include	systype.ah
	include	rules.asi
	include osint.inc
;
;  Routines used to load external functions for SPITBOL-88:
;
;	callextfun	call external function
;	callback	call external function's callback routine
;
;
;  REVISION HISTORY
;  ================
;
;  1.00 18 May 92
;	Initial version -- derived from 386 version
;
	Header@

;
; Macros and equates to access various structures.  With HighC, 
; these are in distinct segments.  Under Intel, they are all
; part of the flat memory model, and segment operations are omitted.


	Data_Seg@

include extrn88.inc

	ext	miscinfo,dword

	Data_EndS@
	page
	Code_Seg@
;----------------------------------------------------------------------
;
;  callextfun - call external function
;
;	result = callextfun(efptr, xsp, nargs)
;
;	   efptr	pointer to efblk
;	   xsp		pointer to arguments
;	   nargs	number of arguments
;	   result	 0 - function should fail
;			-1 - insufficient memory to convert arg (not used)
;			     or function not found.
;			-2 - improper argument type (not used)
;			other - block pointer to function result
;

cefstk	struc
	dw	?			;saved bp
	cPtr@	cef_rip,?,__C__		;return ip
	dPtr@	cef_efp,?,__C__		;pointer to efblk
	dPtr@	cef_xsp,?,__C__		;pointer to arguments
cef_n	dw	?			;number of arguments
cefstk	ends

callextfun proc	near
	publab	callextfun
	push	bp
	mov	bp,sp
	sub	sp,4			;room for ss:sp below
	push	bx			;save C registers
	push	si
	push	di
	push	ds
	push	es
	LES_	si,[bp].cef_efp@	;load pointer to efblk
;
; Convert arguments from SPITBOL internal form to something more
; palatable.
;
	mov	[bp-4],sp		;save SP
	mov	[bp-2],ss		;save SS
	mov	bx,ES_[si].efcod	;load pointer to xnblk
	LES_	dx,[bp].cef_xsp@	;pointer to last argument
	mov	cx,[bp].cef_n		;number of arguments
	mov	ax,ES_[bx].xnsp		;does function use its own stack?
	or	ax,ES_[bx].xnss		;sp is zero if we use SPITBOL's stack
	je	short call0
	loadstk	ES_[bx].xnss,ES_[bx].xnsp ; switch to function's stack
;
; Now push arguments left to right, extracting data from SPITBOL's
; data blocks where specified by the type info table in the efblk.
;
call0:	mov	bx,dx			;ES_bx = ptr to last argument
	mov	ax,cx			;number of arguments
	lea	si,[si].eftar		;pointer to arg types
	shl	ax,1
	add	bx,ax			;point past first argument
	jcxz	call2
call1:
if LDATA
	db	26h			;ES override
endif
	lodsw				;get next argument type to ax
	dec	bx			;point to next argument
	dec	bx
	mov	di,ES_[bx]		;get next argument
	dec	ax
	js	short calln		;0=no convert
	je	short calls		;1=string
	dec	ax
	je	short calli		;2=integer
	dec	ax
	jne	short callf		;4=file
; 					;3=real
callr:	push	word ptr ES_[di].rcval+6;real - push msh
	push	word ptr ES_[di].rcval+4
	push	word ptr ES_[di].rcval+2
	push	word ptr ES_[di].rcval	;push lsh
	loop	call1
	jmp	short call2
calli:
if IA32
	push	word ptr ES_[di].icval+2;push msh
	push	word ptr ES_[di].icval	;push lsh
else
	mov	ax,word ptr ES_[di].icval;convert from 16 to 32 bits
	cwd
	push	dx
	push	ax
endif
	loop	call1
	jmp	short call2
calls:	push	word ptr ES_[di].sclen	;string - push length
	lea	di,[di].scstr		;get starting address of string
					;fall through to no-convert case
calln:					;no conversion
callf:	pushES_				;fcb or no conversion or string
	push	di
	loop	call1
	page
;
; After pushing arguments, we push a pointer to the miscinfo area
; and a pointer to the result area.
;
call2:	pushDS_				;push pointer to misc area
	mov	ax, offset DGROUP:miscinfo
	push	ax
					;push pointer to result area
	GETOFF	ax,TSCBLK
	pushES_
	push	ax			; "     "         "      "
;
;  Here to make the actual call.
;
	LES_	si,[bp].cef_efp@	;load pointer to efblk
	mov	bx,ES_[si].efcod	;load pointer to xnblk
	mov	ds,ES_[bx].xnds
	push	cs
	lea	ax,call3
	push	ax
	push	word ptr ES_[bx].xncall+2
	push	word ptr ES_[bx].xncall
	mov	es,ES_[bx].xnes
	db	0CBh			;call function.  Callee preserves bp.

call3:	loadstk	[bp-2],[bp-4]		;reload SS,SP (SS had better not change!)
	pop	es
	pop	ds
	cld
	pop	di
	pop	si
	pop	bx
	add	sp,4
	pop	bp
	retc	((size cefstk) - 2 - cPtrSize)	; Return result in ax

callextfun endp
	page
;----------------------------------------------------------------------
;
;  callback - perform callback to external function if setup
;
;  Input: pointer to xnblk
;
cbstk	struc
	dw	?			;saved bp
	cPtr@	cb_rip,?,__C__		;return ip
	dPtr@	cb_xnp,?,__C__		;pointer to xnblk
cbstk	ends


callback proc	near
	publab	callback
	push	bp
	mov	bp,sp
	push	si
	push	di
	push	bx
	LES_	di,[bp].cb_xnp@		;pointer to xnblk
	cmp	word ptr ES_[di].xncbp+2,0
	je	short cbck2		;if no callback
	cmp	ES_[di].xnsave,0	;check if already did callback
	js	short cbck2
	mov	cx,sp			;save sp
	mov	dx,ss			;save SS
	mov	ax,ES_[di].xnsp		;does function use its own stack?
	or	ax,ES_[di].xnss		;sp is zero if we use SPITBOL's stack
	je	short cbck2b
	loadstk	ES_[di].xnss,ES_[di].xnsp ;switch to function's stack
cbck2b:	push	di
	push	cx			;save our ss:sp
	push	dx
	push	ds
	push	es
	mov	ds,ES_[di].xnds
	push	cs
	lea	ax,cbck2d
	push	ax
	push	word ptr ES_[di].xncbp+2
	push	word ptr ES_[di].xncbp
	mov	es,ES_[di].xnes
	db	0CBh			;call callback routine

cbck2d:	pop	es
	pop	ds
	pop	di			;saved SS
	pop	bx			;saved SP
	loadstk	di,bx
	cld
	pop	di
	mov	ES_[di].xnsave,-1	;flag callback done
cbck2:	pop	bx
	pop	di
	pop	si
	pop	bp
	retc	0
callback endp

	Code_EndS@
	end
