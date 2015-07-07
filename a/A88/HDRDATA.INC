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


;  HDRDATA.INC
;
;
;  Table to recover type word from type ordinal
;
address	macro	name
if underscore AND direct
	dw	offset MINCODE:_&name
else
	dw	offset MINCODE:name
endif
	endm

	pubdef	TYPET,label,word
	dw	offset MINCODE:B_ART	;ARBLK type word - 0
	dw	offset MINCODE:B_CDC	;CDBLK type word - 1
	dw	offset MINCODE:B_EXL	;EXBLK type word - 2
	address	B_ICL			;ICBLK type word - 3
	dw	offset MINCODE:B_NML	;NMBLK type word - 4
	dw	offset MINCODE:P_ABA	;P0BLK type word - 5
	dw	offset MINCODE:P_ALT	;P1BLK type word - 6
	dw	offset MINCODE:P_ANY	;P2BLK type word - 7
	dw	offset MINCODE:B_RCL	;RCBLK type word - 8
	address	B_SCL			;SCBLK type word - 9
	dw	offset MINCODE:B_SEL	;SEBLK type word - 10
	dw	offset MINCODE:B_TBT	;TBBLK type word - 11
	address	B_VCT			;VCBLK type word - 12
	address	B_XNT			;XNBLK type word - 13
	address	B_XRT			;XRBLK type word - 14
	dw	offset MINCODE:B_BCT	;BCBLK type word - 15
	dw	offset MINCODE:B_PDT	;PDBLK type word - 16
	dw	offset MINCODE:B_TRT	;TRBLK type word - 17
	dw	offset MINCODE:B_BFT	;BFBLK type word   18
	dw	offset MINCODE:B_CCT	;CCBLK type word - 19
	dw	offset MINCODE:B_CMT	;CMBLK type word - 20
	dw	offset MINCODE:B_CTT	;CTBLK type word - 21
	dw	offset MINCODE:B_DFC	;DFBLK type word - 22
	address	B_EFC			;EFBLK type word - 23
	dw	offset MINCODE:B_EVT	;EVBLK type word - 24
	dw	offset MINCODE:B_FFC	;FFBLK type word - 25
	dw	offset MINCODE:B_KVT	;KVBLK type word - 26
	dw	offset MINCODE:B_PFC	;PFBLK type word - 27
	dw	offset MINCODE:B_TET	;TEBLK type word - 28

;
;  Table of MINIMAL variables points that can be accessed from C
;  and assembly-language via macros GETOFF, GETPTR, GETMIN, and SETMIN.
;
;  There are two ways of setting this up.  If the symbol "direct" is
;  non-zero, the symbols are declared public and accessed directly.
;  If direct is zero, access is via the function minadr in inter.asm.
;
;  The macros GETOFF, GETPTR, GETMIN, and SETMIN in osint.h and osint.inc
;  must be adjusted when direct is changed.
;
;  Note that the order of entries in this table must correspond to the
;  order of entries in the VAL enumeration in osint.h and osint.inc.
;
if direct
access	macro	name,grp
	pubname	name
	endm
else
access	macro	name,grp
	dw	offset grp:name
	endm

	ext	_TICBLK,WORD
	ext	_TSCBLK,WORD
	ext	_ID1,WORD
	ext	_ID2BLK,WORD
	ext	_INPBUF,WORD
	ext	_TTYBUF,WORD

	pubdef	valtab,label,word
endif
	access	GBCNT,MINDATA
	access	HEADV,MINDATA
	access	MXLEN,MINDATA
	access	STAGE,MINDATA
	access	TIMSX,MINDATA
	access	DNAMB,MINDATA
	access	DNAMP,MINDATA
	access	STATE,MINDATA
	access	B_EFC,MINCODE
	access	B_ICL,MINCODE
	access	B_SCL,MINCODE
	access	B_VCT,MINCODE
	access	B_XNT,MINCODE
	access	B_XRT,MINCODE
	access	STBAS,MINDATA
	access	STATB,MINDATA
	access	POLCT,MINDATA
	access	TYPET,DGROUP
	access	DFFNC,MINCODE
	access	LOWSPMIN,MINDATA
	access	FLPRT,MINDATA
	access	FLPTR,MINDATA
	access	GTCEF,MINDATA
	access	HSHTB,MINDATA
	access	PMHBS,MINDATA 
	access	R_FCB,MINDATA
	access	C_AAA,MINDATA
	access	C_YYY,MINDATA
	access	G_AAA,MINDATA
	access	W_YYY,MINDATA
	access	S_AAA,MINCODE
	access	S_YYY,MINCODE
	access	R_COD,MINDATA
	access	KVSTN,MINDATA
	access	KVDMP,MINDATA
	access	KVFTR,MINDATA
	access	KVCOM,MINDATA
	access	KVPFL,MINDATA
	access	CSWFL,MINDATA
        access  STMCS,MINDATA
        access  STMCT,MINDATA
        access  _TICBLK,MINDATA
	access	_TSCBLK,MINDATA
	access	_ID1,MINDATA
	access	_ID2BLK,MINDATA
	access	_INPBUF,MINDATA
	access	_TTYBUF,MINDATA
        access  END_MIN_DATA,MINDATA

