	title	VMODE
	page	80,132
	name	VMODE

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


	include	systype.ah
	include	rules.asi
;
;
;
;------------------------------------------------------------;
;							     ;
;     I  B  M     P  C     V  I  D  E  O     M O D E S	     ;
;							     ;
;			 Written By			     ;
;							     ;
;			Mark B. Emmer			     ;
;							     ;
;			Version 1.01			     ;
;							     ;
;	      Copyright (c) 1989, Catspaw, Inc.		     ;
;							     ;
;------------------------------------------------------------;
;
;  This module provides the routines to identify various video options
;  available in the IBM PC family.
	page
;
;  REVISION HISTORY
;  ================
;
;  1.00   06 December 1989, 20:00 (MBE)
;	  Initial version
;
;  1.01   13 November 1990, 08:00 (MBE)
;	  Convert to 8088 version.
;
	Header@

	Data_Seg@

;
; Standard video types
;
none	  equ	0			;no display
mda	  equ	1			;monochrome display adapter
cga	  equ	2			;color graphics adapter
egavga	  equ	3			;start of EGA/VGA/MCGA types
egamono   equ	3			;EGA monochrome
egacolor  equ	4			;EGA color
vgamono   equ	5			;VGA monochrome
vgacolor  equ	6			;VGA color
mcgamono  equ	7			;MultiColor Graphics Array monochrome
mcgacolor equ	8			;MultiColor Graphics Array color

;
; Character sizes based upon height of character cell in pixels
;
font8	  equ	0			;8 pixels high
font14	  equ	1			;14 pixels high
font16	  equ	2			;16 pixels high

;
;  Map font code to number of scan lines in font
;
fnttab	  db	 8			;CGA
	  db	 14			;EGA and MDA
	  db	 16			;VGA and MCGA

;
; Map output of PS/2 Identify Adapter Function to video type
;
vgaiaf	db	none			;0
	db	mda			;1
	db	cga			;2
	db	cga			;3
	db	egacolor		;4
	db	egamono			;5
	db	cga			;6
	db	vgamono			;7
	db	vgacolor		;8
	db	cga			;9
	db	mcgacolor		;10
	db	mcgamono		;11
	db	mcgacolor		;12
vgaiafm	equ	$-vgaiaf		;max value

;
; Map equipment status video bits to video type
;
eqstyp	db	none			;0
	db	cga			;1
	db	cga			;2
	db	mda			;3

;
; Map adapter type to font size for adapters with only one font size
; -1 means have to investigate further
;
adtfsz	db	font8			;no display
	db	font14			;monochrome display adapter
	db	font8			;color graphics adapter
	db	-1			;EGA monochrome
	db	-1			;EGA color
	db	-1			;VGA monochrome
	db	-1			;VGA color
	db	font16			;MultiColor Graphics Array monochrome
	db	font16			;MultiColor Graphics Array color

	Data_EndS@

	Code_Seg@

	public	vm_cols			;return number of columns on screen
	public	vm_fcode		;get font code
	public	vm_fsize		;get font size
	public	vm_lines		;return number of lines on screen
	public	vm_mode			;return current video mode
	public	vm_page			;return active video page
	public	vm_setlines		;set number of physical lines
	public	vm_type			;determine type of video adapter

;
; VM_COLS - Get number of columns on screen
;
;	(none)
;	call	vm_col
;	(ax)	number of columns on screen

vm_cols	proc	near
	push	bx
	mov	ah,0fh			;return current video state
	int	10h
	mov	al,ah			;number of columns on screen
	xor	ah,ah
	pop	bx
	ret
vm_cols	endp


;
; VM_FCODE - Get font code
;
;	(ax)	adapter type
;	call	vm_fcode
;	(ax)	font code (see above)  font8, font14, or font16

vm_fcode proc	near
	push	bx
	mov	bx,ax
	mov	al,adtfsz[bx]		;map adapter type to font size
	cmp	al,-1			;fixed font adapter
	jne	short vfsizx		;if so, just exit
	push	cx
	mov	ax,1130h		;return information from char generator
	push	bp			;preserve es:ebp
	push	es
	xor	bh,bh			;INT 1Fh pointer
	int	10h			;get font height to cx
	pop	es			;restore es:ebp
	pop	bp
	xor	ax,ax			;eax = font8
	cmp	cl,8
	je	short vfsiz1		;exit if font8
	inc	al			;eax = font14
	cmp	cl,14
	je	short vfsiz1		;exit if font14
	inc	al			;eax = font16
vfsiz1:	pop	cx
vfsizx:	pop	bx
	ret
vm_fcode endp


;
; VM_FSIZE - Get font size
;
;	(ax)	adapter type
;	call	vm_fsize
;	(ax)	font size (number of scan lines in character box)

vm_fsize proc	near
	call	vm_fcode		;get font code
	xchg	ax,bx			;(bh = 0)
	mov	bl,fnttab[bx]		;get number of scan lines
	xchg	ax,bx
	ret
vm_fsize endp


;
; VM_LINES - Get number of lines on screen
;
;	(ax)	adapter type
;	call	vm_lines
;	(ax)	number of lines on screen

vm_lines proc	near
	cmp	al,egavga		;check if not EGA/VGA
	jb	short vline1		;it isn't, default to 25 lines
	push	bx			;save registers
	push	cx
	push	dx
	push	bp
	push	es
	mov	ax,1130h		;get character generator info
	xor	bx,bx
	int	10h			;get info
	mov	al,dl			;number of lines - 1
	xor	ah,ah
	inc	ax
	pop	es
	pop	bp
	pop	dx
	pop	cx
	pop	bx
	ret
vline1:	mov	ax,25			;MDA and CGA are always 25 lines
	ret
vm_lines endp

;
; VM_MODE - Get current video mode
;
;	(none)
;	call	vm_mode
;	(ax)	current video mode

vm_mode	proc	near
	push	bx
	mov	ah,0fh			;fetch current video state
	int	10h
	xor	ah,ah			;video mode in al
	pop	bx
	ret
vm_mode	endp

;
; VM_PAGE - Get current video page
;
;	(none)
;	call	vm_page
;	(ax)	current video page

vm_page	proc	near
	push	bx
	mov	ah,0fh			;fetch current video state
	int	10h
	mov	al,bh			;active page
	xor	ah,ah
	pop	bx
	ret
vm_page	endp

;
; VM_SETLINES - Set number of physical screen lines
;
;	(ax)	number of screen lines
;	call	vm_setlines
;	(ax)	unchanged
vm_setlines proc near
	ret
vm_setlines endp

;
; VM_TYPE - Get adapter type
;
;	(none)
;	call	vm_type
;	(ax)	type code (see above)  none through mcgacolor

vm_type	proc	near

	push	bx
	push	cx
	mov	ax,1a00h		;VGA identify adapter function
	int	10h			;BIOS video call
	cmp	al,1ah			;PS/2 BIOS present if AH returned in AL
	jne	short vtype2		;jump if not PS/2 BIOS
;
; PS/2 BIOS means VGA or MCGA
;
	xor	bh,bh			;use adapter indentify to index array
	cmp	bl,vgaiafm		;compare with largest we know about
	jbe	short vtype1		;jump if in range
	mov	bl,2			;unknown types treated like CGA
vtype1:	mov	al,vgaiaf[bx]		;map to video type
	xor	ah,ah
	jmp	short vtypex
;
; Here if not PS/2 BIOS.  See if it is an EGA BIOS.
;
vtype2:	mov	ah,12h			;alternate function select
	mov	bx,10h			;return EGA information
	int	10h			;BIOS video call
	cmp	bx,10h			;no EGA if BX unchanged
	je	short vtype3		;not EGA
	mov	ax,egacolor		;assume color monitor
	sub	al,bh			;change to mono if bh=1
	jmp	short vtypex		;exit
;
; Hereif CGA or MDA
;
vtype3:	int	11h			;get equipment status bits
	and	ax,30h			;video bits
	mov	cl,4
	shr	ax,cl			;create index
	xchg	ax,bx
	mov	al,eqstyp[bx]		;load type
;
; Exit with type in ax
;
vtypex:	pop	cx
	pop	bx
	ret
vm_type	endp

	Code_EndS@
	end
