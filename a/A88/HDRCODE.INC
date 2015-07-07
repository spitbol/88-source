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


;  HDRCODE.INC
;
;  Table of MINIMAL entry points that can be accessed from C
;  via the minimal function (see inter.asm).  Inter makes a
;  FAR jump to callmin, which then makes a near call to
;  the desired function. Each table entry is 2 bytes long.
;
;  Note that the order of entries in this table must correspond
;  to the order of entries in the CALL enumeration in osint.h
;  and osint.inc.
;
mincall	macro	routine
	dw	offset MINCODE:routine
	endm

nocall	macro	routine
	dw	offset MINCODE:dummy
	endm

calltab	label	word
	nocall	RELAJ
	nocall	RELCR
	nocall	RELOC
	mincall	ALLOC
	mincall	ALOCS
	mincall	ALOST
	mincall	BLKLN
	mincall	INSTA
	mincall	RSTRT
	mincall	START
	mincall	FILNM
	mincall	DTYPE
;	mincall	ENEVS			; Engine calls not used
;	mincall	ENGTS			;  in this implementation

callmin	proc	far
	publab	callmin
	shl	bp,1
	mov	bp,cs:calltab[bp]
	call	bp
	pop	ax
	mov	bp,sp
	xor	[bp+2],ax		; set segment back to even if needed
	ret
callmin	endp

dummy	proc	near			; stub for routines not in this version
	ret
dummy	endp
