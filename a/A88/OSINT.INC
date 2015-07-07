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


;   File:  OSINT.INC		Version:  01.00
;   -------------------------------------------
;
;   This header file defines the interface between the Macro SPITBOL compiler
;   and any assembly-language OSINT modules.
;   Communication between the two is handled via a set of global variables
;   defined as externals below.

;  Copyright 1991 Robert Goldberg and Catspaw, Inc.

;   Set up externals for all the compiler's registers.

	Data_Seg@
ifndef	globals
	ext		reg_cp,word
	ext		reg_wa,word
	ext		reg_wb,word
	ext		reg_wc,word
	ext		reg_xr,word
	ext		reg_xl,word
	ext		reg_xs,word
	ext		reg_ia,word
endif
	Data_EndS@

;   Macros to load a compiler register into a machine register

GET_CP	macro		reg
	mov		reg, reg_cp
	endm

GET_IA	macro		reg
	mov		reg, reg_ia
	endm

GET_WA	macro		reg
	mov		reg, reg_wa
	endm

GET_WB	macro		reg
	mov		reg, reg_wb
	endm

GET_WC	macro		reg
	mov		reg, reg_wc
	endm

GET_XR	macro		reg
	mov		reg, reg_xr
	endm

GET_XL	macro		reg
	mov		reg, reg_xl
	endm

GET_XS	macro		reg
	mov		reg, reg_xs
	endm


;   Macros to store a machine register into a compiler register.

SET_CP	macro		val
	mov		reg_cp, val
	endm

SET_IA	macro		val
	mov		reg_ia, val
	endm

SET_WA	macro		val
	mov		reg_wa, val
	endm

SET_WB	macro		val
	mov		reg_wb, val
	endm

SET_WC	macro		val
	mov		reg_wc, val
	endm

SET_XR	macro		val
	mov		reg_xr, val
	endm

SET_XL	macro		val
	mov		reg_xl, val
	endm

SET_XS	macro		val
	mov		reg_xs, val
	endm


;   Return values to take exit N from interface

EXIT_1	equ		0
EXIT_2	equ		4
EXIT_3	equ		8
EXIT_4	equ		12
EXIT_5	equ		16
EXIT_6	equ		20
EXIT_7	equ		24
EXIT_8	equ		28
EXIT_9	equ		32

;    Return value to do a normal return from interface.

NORMAL_RETURN	equ	-1

	Code_Seg@

;	Function and macro to call into MINIMAL code.
;	The argument is an ordinal number defined below.
ifndef	globals
	ext		minimal,near
endif
MINIMAL	macro		callno
	mov		ax, offset callno
	push		ax
	callc		minimal,2
	endm

;   Ordinals for MINIMAL calls from assembly language.
;
;   The order of entries here must correspond to the order of
;   calltab entries in the INTER assembly language module.
;
;   The structure cords is merely a device to produce
;   an enumeration similar to an enum structure to C.
;
cords	struc
RELAJ	db	?
RELCR	db	?
RELOC	db	?
ALLOC	db	?
ALOCS	db	?
ALOST	db	?
BLKLN	db	?
INSTA	db	?
RSTRT	db	?
START	db	?
FILNM	db	?
DTYPE	db	?
ENEVS	db	?
ENGTS	db	?
cords	ends

; Because Windows runs on a 286 or better machine, we don't have to worry
; about bracketing loading the stack registers with CLI/STI.  Loading
; SS turns off interrupts for one cycle.
;
ifdef __WINDOWS__
loadstk	  macro	newss,newsp
	  mov	ss,newss
	  mov	sp,newsp
	  endm
else
loadstk	  macro	newss,newsp
	  cli
	  mov	ss,newss
	  mov	sp,newsp
	  sti
	  endm
endif


;	Function and macro to get/set value from/to MINIMAL dataspace.
;	The argument is an ordinal number defined below.
;	The macro GETOFF returns the address of the value.
if direct
GETOFF	macro		reg,valno
	lea		reg,valno
	endm

GETMIN	macro		reg,valno
	mov		reg,valno
	endm

SETMIN	macro		valno,value
	mov		valno,value
	endm

access	macro		name
	ext		name,word
	endm
	Data_Seg@
else
ifndef	globals
	ext		minoff,near
endif

; Clobbers AX.  If LDATA, clobers DX, ES as well.
GETOFF	macro		reg,valno
	mov		reg, offset valno
	push		reg
	callc		minoff,2
ifdif	<reg>,<ax>
	mov		reg,ax
endif
	endm


; Clobbers AX.  If LDATA, clobers DX, ES as well.
GETMIN	macro		reg,valno
	mov		reg, offset valno
	push		reg
	callc		minoff,2
ifidn	<reg>,<si>
	mov		si,ax
	mov		si,ES_[si]
else
 ifidn	<reg>,<di>
	mov		di,ax
	mov		di,ES_[di]
 else
  ifidn	<reg>,<bx>
	mov		bx,ax
	mov		bx,ES_[bx]
  else
   ifidn <reg>,<ax>
	 xchg		ax,si
	 mov		si,ES_[si]
	 xchg		ax,si
   else
	 xchg		ax,si
	 mov		reg,ES_[si]
	 xchg		ax,si
   endif
  endif
 endif
endif
	endm

; Clobbers AX.  If LDATA, clobers DX, ES as well.
SETMIN	macro		valno,value,value2
ifnb <value2>
	push		value2
endif
	push		value
	mov		ax, offset valno
	push		ax
	callc		minoff,2
	xchg		ax,si
	pop		ES_[si]
ifnb <value2>
	pop		ES_[si]+2
endif
	xchg		ax,si
	endm

access	macro		name
name	db		?
	endm

if LDATA
pushES_	macro
	push		es
	endm

popES_	macro
	pop		es
	endm

pushSS_	macro
	push		ss
	endm

popSS_	macro
	pop		ss
	endm
else
pushES_	macro
	endm

popES_	macro
	endm

pushSS_	macro
	endm

popSS_	macro
	endm
endif

;   Ordinals for transferring MINIMAL values to/from assembly language.
;
;   The order of entries here must correspond to the order of
;   valtab entries in the INTER assembly language module.
;
;   The structure vords and the access macro is merely a device to
;   produce an enumeration similar to an enum structure to C.
;
vords	struc
endif

	access	GBCNT
	access	HEADV
	access	MXLEN
	access	STAGE
	access	TIMSX
	access	DNAMB
	access	DNAMP
	access	STATE
	access	B_EFC
	access	B_ICL
	access	B_SCL
	access	B_VCT
	access	B_XNT
	access	B_XRT
	access	STBAS
	access	STATB
	access	POLCT
	access	TYPET
	access	DFFNC
	access	LOWSPMIN
	access	FLPRT
	access	FLPTR
	access	GTCEF
	access	HSHTB
	access	PMHBS
	access	R_FCB
	access	C_AAA
	access	C_YYY
	access	G_AAA
	access	W_YYY
	access	S_AAA
	access	S_YYY
	access	R_COD
	access	KVSTN 
	access	KVDMP
	access	KVFTR
	access	KVCOM
	access	KVPFL
	access	CSWFL
        access  STMCS
        access  STMCT
        access  TICBLK
	access	TSCBLK
	access	ID1
	access	ID2BLK
	access	INPBUF
	access	TTYBUF
        access  END_MIN_DATA

if direct
	Data_EndS@
else
vords	ends
endif


	Code_EndS@
