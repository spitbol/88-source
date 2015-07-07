
;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

DGROUP	group	_TEXT,_DATA,_BSS

	assume	cs:DGROUP

_TEXT	segment byte public 'CODE'
_TEXT	ends

_DATA	segment word public 'DATA'
_DATA	ends

_BSS	segment word public 'BSS'
_BSS	ends

_TEXT   segment byte public 'CODE'
	assume	cs:_text
	extrn       _mainslf:near
        ORG     100h
STARTX:	jmp	_mainslf
_TEXT   ENDS

        END     STARTX
