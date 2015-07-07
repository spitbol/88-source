	page	80,132
	title	SCOPS - Screen Operations
	name	SCOPS

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
;
;
;
;
;---------------------------------------------------------------;
;								;
;	     D	E  W  A  R  S	 T  O  O  L  S			;
;								;
;	       TOOLS FOR USE WITH IBM PC/DOS			;
;								;
;			 S C O P S				;
;								;
;			 Written By				;
;								;
;		     Robert B. K. Dewar 			;
;								;
;		       Version 6.02				;
;		 November 7, 1989, 23:00			;
;								;
;	     Copyright (c) 1983, Realia Inc.			;
;								;
;---------------------------------------------------------------;
;
;
;  REVISION HISTORY
;  ================
;
;  6.03   29-December 1996, 8:22 (MBE)
;         Removed code from sc_setl that would exit immediately if attribute
;         of column 1 character is the same as the attribute being set.
;         Add sc_read_attr, sc_cpyln, sc_cpysc, sc_scroll.
;
;  6.02	  14 November 1990, 6:00 (MBE)
;	  Convert to 8088 version.
;
;  6.01	  07 November 1989, 23:00 (MBE)
;	  Converted to 80386 32-bit segments using Phar Lap DOS Extender.
;	  Parameterize number of lines.
;	  Be more selective when determining video adapter type, and
;	  only wait for horizontal retrace if a CGA card is present.
;
;  5.01	  07 August 1983, 10:00 (RBKD)
;	  Add handling for COMPAQ (no need to flicker screen, since it is
;	   really a monochrome display, even though it looks like a color one!)
;
;  4.01	  04 July 1983, 10:00 (RBKD)
;	  Add full handling of 24th line
;
;  2.03	  06 June 1983, 17:00 (RBKD)
;	  Add routine sc_locate
;
;  2.02	  01 June 1983, 09:00 (RBKD)
;	  Add routines sc_con and sc_coff to turn cursor on and off
;	  Add routine sc_ctype to set cursor type
;	  Fix error of moving one too many characters on a delete
;
;  2.01	  27 May 1983, 23:00 (RBKD)
;	  Complete rewrite to use screen buffer instead of refresh buffer
;
;  1.01	  14 April 1983, 07:00 (RBKD)
;	  Initial version
	  page
;

;
;  This module provides screen handling routines:
;
	  public  sc_clr_scrn		;clear screen
	  public  sc_coff		;turn cursor off
	  public  sc_con		;turn cursor on
          public  sc_cpyln              ;copy display line to screen buffer
          public  sc_cpysc              ;copy full display to screen buffer
          public  sc_ctype              ;set cursor type
	  public  sc_delete_char	;delete character in line
	  public  sc_erase_to_eol	;erase to end of line
	  public  sc_init		;initialize
	  public  sc_insert_char	;insert character in line
	  public  sc_offset		;set screen offset
	  public  sc_read		;read string
          public  sc_read_attr          ;read attribute string
          public  sc_read_char          ;read character
	  public  sc_roll_down		;roll screen down
	  public  sc_roll_up		;roll screen up
          public  sc_scroll             ;scroll display buffer
	  public  sc_seg		;compute screen segment
	  public  sc_seta		;set attributes
	  public  sc_setl		;set line attribute
	  public  sc_term		;terminate
	  public  sc_updln		;update line
	  public  sc_updsc		;update screen
	  public  sc_write		;write string
          public  sc_write_attr         ;write attribute string
	  public  sc_write_char		;write character
;
;  Attribute locations for use with SC_SETA
;
	  public  sc_anorm		;normal attribute
	  public  sc_arev		;reversed attribute
;
;
;  These routines use a screen buffer allocated by SC_INIT.
;  This screen buffer accomodates a data area dlines long with the lines
;  having any swidth from dwidth to 255, together with mlines message lines
;  which are always dwidth characters long.
;
;  For a 25x80 screen, dwidth is 80, and dlines+mlines is 25.
;
;  Most of the routines in this module only affect the contents of the screen
;  buffer, The actual displayed area consists of an dwidth character wide slice
;  of the screen buffer, the left offset of the slice being zero by default,
;  and modifiable by calling the routine sc_offset.
;
;  These routine are assembled as NEAR procedures for use in a COMPACT mode
;  program where the single code segment is called scops and the data
;  segment is called @scops.  DS, ES, SS segment registers point to the
;  data segment. All routines assume DF is reset (autoincrement) on entry
;  and preserve this setting for exit.
;
	Header@

	Data_Seg@

	  page
;
; Standard video types
;
none	  equ	0			;no display
mda	  equ	1			;monochrome display adapter
cga	  equ	2			;color graphics adapter
egavga	  equ	3			;start of EGA/VGA types
egamono	  equ	3			;EGA monochrome
egacolor  equ	4			;EGA color
vgamono	  equ	5			;VGA monochrome
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
;  Data locations
;
	  public sc_dtype
	  public sc_vtype
	  public sc_soff

scseg	  dw	 ?			;segment of display buffer (set by sc_seg)
sc_buf	  dw	 0			;segment of private buffer and tables
sc_dtype  db	 ?			;M/C for IBM mono/IBM color
crtmod	  db	 ?			;value of crt_mode_set from ROM data area
visible   db     0                      ;cursor visibility
ctype_old db     0                      ;previous cursor type
          even
swidth	  dw	 0			;width of screen buffer
dwidth	  dw	 0			;width of physical display
sbsiz	  dw	 0			;bytes in buffer
					;  (dlines*swidth + mlines*dwidth)
sc_soff	  dw	 0			;display offset for screen buffer
;
;  An allocated segment holds data structures that must be dynamically
;  allocated.  sc_buf contains the segment selector that points to the
;  allocated memory.  It is laid out as follows:
;
;
;		+-------------------------+
;  sbtbl ------>|  table of word offsets  |  (indexed by line number)
;		|  of line starts in this |
;		|  this segment		  |
;  dbtbl ------>+-------------------------+
;		|  table of word offsets  |  (indexed by line number)
;		|  into video memory      |
;		+-------------------------+
;		|			  |
;		|     screen buffer	  |
;		|			  |
;		+-------------------------+
;
;
;  Pointer to a table of words giving the offsets
;  of the start of lines in the screen buffer.
;
sbtbl	  dw	 ?
;
;  Pointer to a table used to calculate display buffer offset positions
;
dbtbl	  dw	 ?
;
;  Attribute locations. These locations are set by sc_init, which uses
;  the first character of the last line of the screen on entry to determine
;  the normal attribute, and its inverse for the reverse video entry.
;
sc_anorm  db	 ?			;normal attribute
sc_arev	  db	 ?			;video reversed attribute
;
;  Number of lines in display and message portions of screen.
;  The sum of these two numbers must be less than or equal to 255.
;  (It is allowed to have mlines equal 0)
;
dlines	  db	 ?			;number of lines in display area
mlines	  db	 ?			;number of lines in message area
;
sc_vtype  db	 none			;true adapter type from vm_type
;
	Data_EndS@
	  page
;
;  Locations in BIOS ROM
;
romdata	     equ  0040h			;location of ROM BIOS data segment
crt_cur_mode equ  0060h			;cursor starting/end lines
addr_6845    equ  0063h			;address of CRT controller address reg
crt_mode_set equ  0065h			;current setting of the 3x8 message
crt_info     equ  0087h			;video display area info byte
crt_status   equ  03dah			;status port for color card

;
;  Selected registers within 6845 controller:
;
cs_6845	     equ  0ah			;cursor start register
ce_6845	     equ  0bh			;cursor end register

;
;  Segment selectors hardwired by Phar Lap DOS Extender
;
mem64k       equ  0000h			;first 64k bytes of memory

;
;  Macro used to generate sequence to wait for horizontal retrace. On exit,
;  dx and al are destroyed and interrupts are disabled ready for a lodsb or
;  stosb operation to be performed (possibly preceded by a register mov).
;
retrace	  macro
	  local  l1,l2
l1:	  in	 al,dx			;wait for status to go low
	  test	 al,1
	  jnz	 l1
	  cli				;stop interrupts
l2:	  in	 al,dx			;wait for status to go high
	  test	 al,1
	  jz	 l2
	  endm
;
;  Macros to push and pop all registers.  Like 286 pusha, but
;  does not push sp.
;
pushall	  macro
	  push	 ax
	  push	 cx
	  push	 dx
	  push	 bx
	  push	 bp
	  push	 si
	  push	 di
	  endm

popall	  macro
	  pop	 di
	  pop	 si
	  pop	 bp
	  pop	 bx
	  pop	 dx
	  pop	 cx
	  pop	 ax
	  endm


;
;  Suppress listing of this macro when it is generated
;
.sall
	Code_Seg@

	  extrn	 _allocs	:near	;allocate memory
	  extrn	 _frees		:near	;free memory
	  extrn	 vm_fcode	:near	;get current font code
	  extrn	 vm_fsize	:near	;get current font size
	  extrn	 vm_type	:near	;get adapter/display type
	  page
;
;  SC_CLR_SCRN - Clear Screen
;
;  This procedure is intended to be called at the end of execution, before
;  returning to DOS, it clears the screen to all blanks with normal attributes,
;  and resets the cursor to the home position.  Video page zero is selected.
;  Both the screen buffer and the display buffer are affected by the call
;  to this routine.
;
sc_clr_scrn proc near
	  pushall			;save registers
	  sub	 dx,dx			;home cursor
	  mov	 ah,2
	  mov	 bh,0
	  int	 10h
	  mov	 bl,sc_anorm		;set normal attribute
	  mov	 al,dlines
	  add	 al,mlines
	  mul	 byte ptr dwidth
	  mov	 cx,ax			;entire screen
	  mov	 al,' '			;blank
	  mov	 ah,9
	  int	 10h
	  mov	 di,sbtbl		;clear screen buffer
	  push	 es
	  mov	 es,sc_buf
	  mov	 di,es:[di]
	  mov	 cx,sbsiz
	  mov	 al,' '
	  rep	 stosb
	  pop	 es
	  popall			;restore registers
	  ret				;return to caller
sc_clr_scrn endp
	  page
;
;  SC_COFF - Turn Cursor Off
;
;	  call	 sc_coff		;call to turn cursor off
;
;  This routine extinguishes the cursor by moving it to a line beyond the
;  end of the visible screen.
;
sc_coff	  proc	 near
          cmp    visible,1              ;test if forced visible
          je     short con3a            ;ignore cursor off request
          pushall                       ;save registers
	  mov	 ah,0fh			;fetch current display page to bh
	  int	 10h
	  mov	 dh,dlines		;set cursor off screen
	  add	 dh,mlines
	  sub	 dl,dl
	  mov	 ah,2
	  int	 10h
	  popall			;restore registers
con3a:    ret                           ;return to caller
sc_coff	  endp
	  page
;
;  SC_CON - Turn Cursor On
;
;	  (dh,dl)			;cursor line/character
;	  call	 sc_con 		;call to turn cursor on
;
;  This line/character position given is relative to the screen buffer, and
;  is appropriately translated to a display buffer address, honoring the
;  current screen offset. If this resulting logical address is off the
;  physical screen, then the cursor is extinguished by moving it beyond
;  the last physical line.
;
sc_con	  proc	 near
          pushall                       ;save registers
	  cmp	 dh,dlines		;jump if on message line, col is right
	  jae	 con2
	  sub	 dl,byte ptr sc_soff	;else subtract screen offset
	  jb	 con1			;jump if off left side of screen
	  cmp	 dl,byte ptr dwidth	;else OK if not off right edge
	  jb	 con2
;
;  Here if cursor position is off physical screen
;
con1:	  mov	 dh,dlines		;set last line + 1, col 0 and merge
	  add	 dh,mlines
	  sub	 dl,dl
;
;  Merge here to set cursor from (DH,DL)
;
con2:	  mov	 ah,0fh			;get page number to bh
	  int	 10h
	  mov	 ah,2			;set cursor
	  int	 10h
	  popall			;restore registers
con3b:    ret                           ;return to caller
sc_con	  endp
	  page
;
;  SC_CTYPE - Set Cursor Type
;
;	  (al)				cursor type code
;         (dl)                          cursor visibility
;         call   sc_ctype               call to set cursor type
;         (al)                          previous cursor type code
;
;  The code is one of the following:
;
;    00h   Normal DOS underline cursor
;    01h   Full block cursor
;    02h   Half block low on line
;    03h   Half block high on line
;    04h   Original cursor at system startup.
;
;  The visibility is one of the following:
;
;    00h   Cursor on during input operations
;    01h   Cursor on all the time
;
sc_ctype  proc	 near
          mov    visible,dl             ;save the visibility code
          pushall                       ;save registers
	  xor	 ah,ah
	  mov	 si,ax			;save the code
	  mov	 al,sc_vtype		;adapter type
	  call	 vm_fcode		;get font code (font8, 14, 16)
	  shl	 ax,1			;font code * 8
	  shl	 ax,1
	  shl	 ax,1
	  shl	 si,1			;cursor type * 2
	  add	 si,ax
	  mov	 cx,ctable[si]		;load cursor parameters from table
	  cmp	 sc_vtype,egavga	;check if EGA or higher
	  jae	 ctype1			;process EGA/VGAs if MDA/CGA
;
;  Set cursor using BIOS on non-EGA/VGA systems
;
	  mov	 ah,1			;set cursor type
	  int	 10h
ctype0:   popall                        ;restore registers
          xchg   al,ctype_old           ;return previous cursor type
          ret
;
;  EGA/VGA.  Deal with low-memory video adapter data area
;
ctype1:	  push	 es			;save ES
	  mov	 ax,mem64k		;map first 64k of memory
	  mov	 es,ax
	  mov	 bx,romdata*10h		;offset to rom bios data area
;
;  On EGA, have to bump the cursor end line by one.  If new value
;  is same as font size, reset end line to 0.
;  Then, if start and end lines are both 0, set end line to 1Eh.
;  Also, if final start line is 0Ch, and end line is 0 (normal cursor),
;  use 0Bh for start, 0Dh for end.
;
	  xor	 ah,ah			;(redundant)
	  mov	 al,sc_vtype		;adapter type
	  cmp	 al,egacolor
	  ja	 ctype4			;jump if not EGA
	  call	 vm_fsize		;get font size
	  inc	 cl			;bump end line
	  cmp	 cl,al			;is end line at max?
	  jb	 ctype2			;no, use bumped value
	  xor	 cl,cl			;at max, set to zero
ctype2:	  or	 cx,cx			;now check for both at zero
	  jne	 ctype3			;no
	  mov	 cl,30			;both are zero, set end to 30
ctype3:   cmp	 cx,0c00h		;check for single line cursor
	  jne	 ctype4			;no
	  mov	 cx,0b0dh		;yes, shift line up on EGA
;
;  Set the cursor start/end lines
;
ctype4:   cli
	  mov	 dx,es:[bx+addr_6845]	;address of CRT controller adr reg
	  mov	 al,cs_6845		;cursor start register
	  out	 dx,al
	  inc	 dx
	  mov	 al,ch
	  out	 dx,al			;put out start line value
	  dec	 dx			;address register
	  mov	 al,ce_6845		;cursor end register
	  out	 dx,al
	  inc	 dx
	  mov	 al,cl
	  out	 dx,al			;put out end line value
	  mov	 es:[bx+crt_cur_mode],cx ;save scan lines in BIOS data area
	  sti
	  pop	 es
          jmp    ctype0                 ;return 
sc_ctype  endp
;
;  Table of start and end line locations for cursor types
;
	Data_Seg@
ctable	  dw	 0607h			;font8 - underline
	  dw	 0007h			;  "   - full block
	  dw	 0407h			;  "   - half block low
	  dw	 0003h			;  "   - half block high
  	  dw	 0607h			;font8 - underline

	  dw	 0c0dh			;font14 - underline
	  dw	 000dh			;  "    - full block
	  dw	 070dh			;  "    - half block low
	  dw	 0006h			;  "    - half block high
	  dw	 0c0dh			;font14 - underline

	  dw	 0e0fh			;font16 - underline
	  dw	 000fh			;  "    - full block
	  dw	 080fh			;  "    - half block low
	  dw	 0007h			;  "    - half block high
	  dw	 0e0fh			;font16 - underline

	Data_EndS@
	  page
;
;  SC_DELETE_CHAR - Delete character in line
;
;	  (dh,dl)			;line/char position of char to delete
;	  call	 sc_delete_char		;call to delete character in line
;
;  Characters are moved to the left starting with the character just after the
;  specified insertion point. A blank is written to the last character position.
;  This routine operates on the screen buffer, the display buffer is unaffected.
;
sc_delete_char proc near
	  pushall			;save registers
	  push	 es
	  mov	 cx,dwidth		;CX = count of characters to move over
	  cmp	 dh,dlines		;jump if on message line
	  jae	 dele1
	  mov	 cx,swidth		;display line, use full screen buffer width
dele1:	  cmp	 dl,cl			;see if beyond line
	  jae	 dele2
	  sub	 cl,dl
	  dec	 cx
	  mov	 es,sc_buf
	  mov	 di,sbtbl		;es:di points to sbtbl in allocated seg
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]		;load offset of line start from sbtbl
	  xor	 bh,bh			;(probably redundant)
	  mov    bl,dl			;bx contains column number
	  add	 di,bx			;es:di points to character at dh,dl
	  mov	 si,di			;buffer position of following char
	  inc	 si
	  db	 26h			;es seg override on source
	  rep	 movsb			;perform the move
	  mov	 al,' '			;store blank at end of line
	  stosb
dele2:	  pop	 es
	  popall
	  ret				;exit
sc_delete_char endp
	  page
;
;  SC_ERASE_TO_EOL - Erase to End of Line
;
;	  (dh,dl)			;start line/char for erase
;	  call	 sc_erase_to_eol	;call to erase to end of line
;
;  This routine operates on the screen buffer, the display buffer is unaffected.
;
sc_erase_to_eol proc near
	  pushall			;save registers
	  push	 es
	  mov	 cx,dwidth		;get screen width
	  cmp	 dh,dlines		;jump if on message line
	  jae	 ereol0
	  mov	 cx,swidth		;display line, get full width
ereol0:	  cmp	 dl,cl			;immediate return if at or past eol
	  jae	 ereol1
	  sub	 cl,dl			;count of characters to blank to CX
	  mov	 es,sc_buf
	  mov	 di,sbtbl		;es:di points to sbtbl in allocated seg
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]		;load offset of line start from sbtbl
	  mov    bl,dl			;bx contains column number
	  xor	 bh,bh
	  add	 di,bx			;es:di points to character at dh,dl
	  mov	 al,' '			;store required number of blanks
	  rep	 stosb
;
;  Exit point
;
ereol1:	  pop	 es			;restore reg
	  popall
	  ret				;exit
sc_erase_to_eol endp
	  page
;
; Map standard video types to 'M' or 'C', and to screen segment.
;
	  Data_Seg@
dtype	  db	'M'			;no display
	  db	'M'			;monochrome display adapter
	  db	'C'			;color graphics adapter
	  db	'M'			;EGA monochrome
	  db	'C'			;EGA color
	  db	'M'			;VGA monochrome
	  db	'C'			;VGA color
	  db	'M'			;MultiColor Graphics Array monochrome
	  db	'C'			;MultiColor Graphics Array color

; map display mode to starting segment
;
sc_dseg	  db	0b8h			; 0 - 40x25 alpha
	  db	0b8h			; 1 - 40x25 alpha
	  db	0b8h			; 2 - 80x25 alpha
	  db	0b8h			; 3 - 80x25 alpha
	  db	0b8h			; 4 - 320x200 graphics
	  db	0b8h			; 5 - 320x200 graphics
	  db	0b8h			; 6 - 640x200 graphics
	  db	0b0h			; 7 - 80x25 alpha
	  db	0b8h			; 8 - not used (PCjr only)
	  db	0b8h			; 9 - not used (PCjr only)
	  db	0b8h			; A - not used (PCjr only)
	  db	0b8h			; B - (used by EGA video BIOS)
	  db	0b8h			; C - (used by EGA video BIOS)
	  db	0a0h			; D - 320x200 graphics
	  db	0a0h			; E - 640x200 graphics
	  db	0a0h			; F - 640x350 graphics
	  db	0a0h			;10 - 640x350 graphics
	  db	0a0h			;11 - 640x480 graphics
	  db	0a0h			;12 - 640x480 graphics
	  db	0a0h			;13 - 320x200 graphics
sc_dsegm  equ	$-sc_dseg-1		;max value

	  Data_EndS@

;
;  SC_SEG - Compute Screen Segment
;
;  Input:  None
;	   call	 sc_seg
;  Output: None				variable scseg set to segment number
;	   (ax)				screen segment (a000, b000, or b800)
;	   (bx)				bashed
;
sc_seg	  proc   near
	  mov	 ah,0fh			;get current display mode
	  int	 10h
	  cmp	 al,sc_dsegm		;compare with largest we know about
	  jbe	 short sc_seg1		;jump if in range
	  mov	 al,3			;unknown types treated like simple text
sc_seg1:  xor	 ah,ah			;convert to index
	  mov	 bx,ax
 	  mov	 ah,sc_dseg[bx]		;get upper byte of screen segment
	  xor	 al,al			;clear lower byte
	  mov	 scseg,ax		;set screen segment
	  ret
sc_seg	  endp
	  page
;
;  SC_INIT - Initialize Screen Operations Module
;
;	  (ch)				physical screen width
;	  (cl)				logical screen width (ch to 255)
;	  (dl)				number of display lines of width (cl)
;	  (dh)				number of message lines of width (ch)
;	  call	 sc_init
;	  (ax)				non-zero: segment selector allocated
;	 				zero: allocation failed
;
;  This procedure must be called before any other routines are used.
;  It will allocate a segment large enough to accommodate the screen buffer
;  as well as the auxiliary tables needed.
;
;  The screen buffer requires (# display lines) * (logical screen width) +
;  (# message lines) * (physical screen width).
;
;  Other tables allocated from the same segment are:
;    sbtbl : (# display lines + # message lines) words
;    dbtbl : (# display lines + # message lines) words
;
;
sc_init	  proc	 near
	  pushall			;save registers
	  call	 sc_term		;release any buffer previously acquired
;
;  Save parameters and compute size of segment needed.
;
	  mov	 dlines,dl		;save number of display lines
	  mov	 mlines,dh		;save number of message lines
	  mov	 al,cl			;logical display width
	  xor	 ah,ah
	  mov	 swidth,ax		;save in swidth
	  mul	 dl			;multiplied by number of display lines
	  mov	 bx,ax			;save product in BX
	  mov	 al,ch			;physical screen width
	  xor	 ah,ah
	  mov	 dwidth,ax		;save in dwidth
	  mul	 dh			;multiplied by number of message lines
	  add	 bx,ax			;bx = screen buffer size
	  mov	 sbsiz,bx		;save screen buffer save
	  mov	 al,dl			;add display and message lines
	  xor	 ah,ah
	  add	 al,dh			;total number of lines (less than 256)
	  shl	 ax,1			;times 2 for words of sbtbl
	  mov	 sbtbl,0		;sbtbl is at offset 0 of allocated segment
	  mov	 dbtbl,ax		;starting offset of dbtbl
	  add	 ax,ax			;size of sbtbl and dbtbl
	  mov	 si,ax		        ;offset of start of screen buffer
	  add	 ax,bx		        ;total size of segment needed
	  push	 ax
	  call	 _allocs			;allocate memory
	  inc	 sp			;pop argument
	  inc	 sp
	  or	 ax,ax			;successful?
	  jne	 init0			;j. if so
	  ret				;return if failure

;  Build tables in allocated memory
;
init0:	  mov	 sc_buf,ax		;save segment
	  push	 es			;save register
	  mov	 es,ax			;point to segment
	  mov	 bl,cl		 	;display line width
	  xor	 bh,bh
	  mov	 di,sbtbl		;point to screen buffer offset table
	  mov	 cl,dl			;set count for first dlines entries
	  xor	 ch,ch
	  mov	 ax,si			;screen buffer starting offset to ax
	  jcxz	 init1a			;skip if zero count
;
;  Loop to store first dlines entries of screen buffer address table
;
init1:	  stosw 			;store next address
	  add	 ax,bx			;point to next line
	  loop	 init1			;loop till all stored
;
;  Loop to store first mlines entries of screen buffer address table
;
init1a:	  mov	 cl,dh			;set count for next mlines entries
	  xor	 ch,ch
	  jcxz	 init2a			;skip if zero count

init2:	  stosw				;store next address
	  add	 ax,dwidth		;point to next line
	  loop	 init2			;loop till all stored
;
;  Initialize dbtbl pointing to offsets in display memory
;
init2a:	  mov	 cl,dlines		;number of display lines
	  xor	 ch,ch
	  add	 cl,dh			;plus message lines
	  mov	 di,dbtbl		;point to table
	  xor	 ax,ax
init3:	  stosw				;store next offset
	  add	 ax,dwidth		;next display line
	  loop	 init3
	  pop	 es			;restore ES
	  page
;
;  Get current screen attribute
;
	  add	 dh,dlines		;point to first char last message line
	  dec	 dh
	  sub	 dl,dl
	  mov	 ah,0fh			;fetch current display page to bh
	  int	 10h
	  mov	 ah,2
	  int	 10h
	  mov	 ah,8			;read attribute at this location
	  int	 10h
	  mov	 al,ah			;copy attribute
	  and	 al,7fh 		;reset blink attribute
	  mov	 sc_anorm,al		;store normal attribute
;
;  Invert screen attribute to get reverse video
;
	  rol	 al,1			;invert bytes
	  rol	 al,1
	  rol	 al,1
          rol    al,1
	  mov	 sc_arev,al		;store reversed attribute
	  mov	 sc_soff,0	        ;clear horizontal offset
;         call   sc_clr_scrn            ;clear screen, home cursor
          call   sc_cpysc               ;syncronise buffer with current video
;
;  Acquire data from BIOS area
;
	  push	 es			;save ES
	  mov	 ax,mem64k		;map first 64k of memory
	  mov	 es,ax
	  mov	 bx,romdata*10h		;offset to rom bios data area
	  mov	 al,es:[bx+crt_mode_set]
	  mov	 crtmod,al		;set crt mode value
	  pop	 es			;restore ES
	  call	 vm_type		;get display/adapter type
	  mov	 sc_vtype,al		;save true type
	  mov	 bx,ax
	  mov	 al,dtype[bx]		;get shorthand type ('C' or 'M')
	  mov	 sc_dtype,al		;save for use later
	  call	 sc_seg			;compute screen segment
	  popall			;restore registers
	  ret				;return to caller
sc_init	  endp
	  page
;
;  SC_INSERT_CHAR - Insert character in line
;
;	  (dh,dl)			;line/char of position for insert
;	  call	 sc_insert_char		;call to insert character in line
;
;  A blank is inserted at the indicated location and remaining characters
;  on the line are moved over. The last character on the line is lost.
;  This routine operates on the screen buffer, the display buffer is unaffected.
;
sc_insert_char proc near
	  pushall			;save registers
	  push	 es
	  mov	 cx,dwidth		;get screen width
	  cmp	 dh,dlines		;jump if on message line
	  jae	 inser1
	  mov	 cx,swidth		;display line, get full width
inser1:	  cmp	 dl,cl			;see if beyond end of line
	  jae	 inser2
	  dec	 cx
	  mov	 es,sc_buf
	  mov	 di,sbtbl		;es:di points to sbtbl in allocated seg
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]		;load offset of line start from sbtbl
	  add	 di,cx
	  mov	 si,di			;buffer position of preceding char
	  dec	 si
	  sub	 cl,dl			;number of characters to blank
	  std				;auto-decrement
	  db	 26h			;es seg override on source
	  rep	 movsb			;perform the move
	  mov	 byte ptr es:[di],' '	;store blank at current location
inser2:	  pop	 es
	  popall
	  cld				;restore auto-increment
	  ret				;exit
sc_insert_char endp
	  page
;
;  SC_OFFSET - Set Offset
;
;	  (al)				new offset value (0 to swidth-dwidth)
;	  call	 sc_offset		call to set new offset
;
;  This routine establishes a new left offset for display of the screen data.
;  It also updates the screen display so that the new offset takes effect.
;
sc_offset proc	 near
	  push	 bx			;save register
	  mov	 bx,swidth	        ;screen buffer width
	  sub	 bx,dwidth		;minus display buffer width
	  cmp	 al,bl		        ;compare with desired offset
	  pop	 bx			;restore register
	  ja	 offs1			;jump if argument too large
	  mov	 byte ptr sc_soff,al	;store new offset
	  jmp	 sc_updsc		;exit updating screen
offs1:	  ret
sc_offset endp
	  page
;
;  SC_READ - Read String
;
;	  (es:di)			pointer to string
;	  (al)				number of characters to read (0 to swidth)
;	  (dh,dl)			line/char of start position for read
;	  call	 sc_read		call to read string
;	  (dl)				incremented by length of string
;
;  The string to be read should not extend past the end of the current line.
;
sc_read	  proc	 near
	  pushall			;save registers
	  push	 ds
	  mov	 cl,al			;count of characters to CX
	  xor	 ch,ch
	  mov	 si,sbtbl
	  mov	 ds,sc_buf		;ds:si points to sbtbl in allocated segment
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 si,[si+bx]		;load offset of line start from sbtbl
	  mov    bl,dl			;bx contains column number
	  xor	 bh,bh
	  add	 si,bx			;ds:si points to character at dh,dl
	  rep	 movsb			;move characters to output string
	  pop	 ds
	  popall			;restore regs
	  add	 dl,al			;bump cursor
	  ret				;exit
sc_read	  endp
          page
;
;  SC_READ_ATTR - Read Attribute String
;
;	  (es:di)			pointer to string
;         (al)                          number of attributes to read (0 to swidth)
;	  (dh,dl)			line/char of start position for read
;         call   sc_read_attr           call to read string
;
;  The string to be read should not extend past the end of the current line.
;
sc_read_attr   proc   near
	  pushall			;save registers
	  push	 ds
	  mov	 cl,al			;count of characters to CX
	  xor	 ch,ch
          mov    si,dbtbl
          mov    ax,scseg               ;reading attributes from display
          mov    ds,sc_buf              ;ds:si points to dbtbl in allocated segment
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
          mov    si,[si+bx]             ;load offset of line start from dbtbl
	  mov    bl,dl			;bx contains column number
	  xor	 bh,bh
	  add	 si,bx			;ds:si points to character at dh,dl
          shl    si,1                   ;adjust for two bytes per char cell
          mov    ds,ax                  ;point to display buffer
scra1:    inc    si                     ;point to attribute
          movsb                         ;move character to output string
          loop   scra1
	  pop	 ds
	  popall			;restore regs
          ret                           ;exit
sc_read_attr   endp
          page
;
;  SC_READ_CHAR - Read Character
;
;	  (dh,dl)			line/char of character to read
;	  call	 sc_read_char		call to read character
;	  (dl)				incremented by one
;	  (al)				character read
;
sc_read_char proc near
	  push	 bx			;save registers
	  push	 si
	  push	 ds
	  mov	 si,sbtbl
	  mov	 ds,sc_buf		;ds:si points to sbtbl
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 si,[si+bx]		;load offset of line start from sbtbl
	  mov    bl,dl			;bx contains column number
	  xor	 bh,bh
	  mov	 al,[si+bx]		;load character at dh,dl
	  inc	 dl			;bump cursor
	  pop	 ds
	  pop	 si			;restore regs
	  pop	 bx
	  ret				;exit
sc_read_char endp
	  page
;
;  SC_ROLL_DOWN - Roll Screen Down
;
;	  (ah)				;top line of scroll area
;	  (al)				;bottom line of scroll area
;	  call	 sc_roll_down		;call to roll screen down
;
;  On exit, the indicated display lines are scrolled down, and the top line
;  is replaced by all blanks (with normal attribute). Note that it is possible
;  for ah to be one less than al, in which case no lines are scrolled, but
;  the top line is still blanked. This routine operates on the screen buffer,
;  the contents of the display buffer is not affected.
;
sc_roll_down proc near
	  pushall			;save registers
	  mov	 bl,al			;number of lines being scrolled to BX
          inc    bl
	  sub	 bl,ah
          xor    bh,bh
          push   es
	  mov	 es,sc_buf
	  mov	 si,sbtbl
	  shl	 bx,1
	  mov	 cx,es:[si+bx]		;number of characters being scrolled
	  sub	 cx,es:[si]
          mov    bl,ah
          shl    bx,1
          mov    di,es:[si+bx]          ;start of first line in case 0 line count
          jcxz   scrd1
	  mov	 bl,al			;last line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 si,es:[si+bx]
	  add	 si,swidth
	  dec	 si			;point to last char of region to scroll
	  mov	 di,si			;point to line below for move
	  add	 di,swidth
	  std				;auto-decrement
	  db	 26h			;es seg override on source
	  rep	 movsb			;move characters up screen
scrd1:    mov    al,' '                 ;set blank
	  mov	 cx,swidth		;store line of blanks at end
	  rep	 stosb
	  cld				;restore auto-increment
	  pop	 es
	  popall			;restore registers
	  ret				;exit
sc_roll_down endp
	  page
;
;  SC_ROLL_UP - Roll Screen Up
;
;	  (ah)				;top line of scroll area
;	  (al)				;bottom line of scroll area
;	  call	 sc_roll_up		;call to roll screen up
;
;  On exit, the indicated display lines are scrolled up, and the
;  bottom line is replaced by all blanks (with normal attribute). Note that
;  it is possible for ah to be one more than al, in which case no lines are
;  scrolled, but the bottom line is still blanked.
;
sc_roll_up proc near
	  pushall			;save registers
	  mov	 bl,al			;number of lines being scrolled to BX
          inc    bl
	  sub	 bl,ah
          xor    bh,bh
          push   es
	  mov	 es,sc_buf
	  mov	 si,sbtbl
	  shl	 bx,1
	  mov	 cx,es:[si+bx]		;number of characters being scrolled
	  sub	 cx,es:[si]
          mov    bl,al
          shl    bx,1
          mov    di,es:[si+bx]          ;start of first line in case 0 line count
          jcxz   scrd1
          mov    bl,ah                  ;first char of region to scroll
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 si,es:[si+bx]
	  mov	 di,si			;point to line above for move
	  sub	 di,swidth
	  db	 26h			;es seg override on source
          rep    movsb                  ;move characters up screen
          jmp    scrd1                  ;blank line and exit
sc_roll_up endp
	  page
;
;  SC_SCROLL - Scroll Display Buffer
;
;         (al)                          ;number of lines to scroll +=up, -=down
;         (bh)                          ;attribute for newly blanked lines
;         (ch)                          ;row, upper left corner
;         (cl)                          ;column, upper left corner
;         (dh)                          ;row, lower right corner
;         (dl)                          ;column, lower right corner
;         call   sc_roll_up             ;call to roll rectangular area up or down
;
;  On exit, the indicated display area is scrolled up or down the number of
;  lines indicated, and the vacated lines are replaced by blanks for the screen
;  attribute specified.  If the number of rows to scroll is zero, the entire
;  area is blanked.
;
sc_scroll proc near
          cmp   ch,dh
          ja    short sccr1
          cmp   cl,dl
          ja    short sccr1
          mov   ah,6                    ;assume scrolling up
          or    al,al
          jns   short sccr0
          neg   al                      ;scrolling down
          inc   ah                      ;use sub-function 7
sccr0:    int   10h
sccr1:    ret
sc_scroll endp
          page
;
;  SC_SETA - Set Attributes
;
;	  (cx)				character count
;	  (dh,dl)			line/char of starting attribute position
;	  (al)				attribute to be set
;	  call	 sc_seta		call to set attributes
;
;  Unlike most other calls to this module, sc_seta deals with the display buffer
;  directly, and the dh,dl values refer to locations on the screen itself, not
;  to logical locations in the screen buffer. The count may exceed dwidth characters
;  in which case the attribute wraps from line to line, but the count cannot
;  extend past the end of the screen.
;
;  For best handling of monochrome/color screens, the attribute in al should
;  be obtained from sc_snorm or sc_arev with the possible addition of 80h for
;  blinking and/or 08h for high intensity. The locations sc_snorm and sc_arev
;  are byte locations defined in this module and initialized by sc_init.
;
sc_seta	  proc	 near
	  jcxz	 seta3			;immediate exit if zero count
	  push	 es			;save registers
	  pushall
	  mov	 di,dbtbl
	  mov	 es,sc_buf		;es:di = dbtbl in allocated segment
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]		;load offset of line start from dbtbl
	  mov    bl,dl			;bx contains column number
	  xor	 bh,bh
	  add	 di,bx
	  mov	 es,scseg
	  shl	 di,1			;es:di points to character at dh,dl
	  cmp	 sc_vtype,cga		;jump if color board with flicker
	  je	 seta4
;
;  All adapters except CGA case
;
seta1:	  inc	 di			;point to next attribute character
	  stosb 			;store attribute
	  loop	 seta1			;loop till all written
;
;  Common exit points
;
seta2:	  popall 			;restore regs
	  pop	 es
seta3:	  ret				;exit
	  page
;
;  SC_SETA (continued)
;
;  CGA case
;
seta4:	  mov	 bl,al			;save attribute char
	  mov	 dx,crt_status		;set status port for color card
;
;  Loop to store attributes
;
seta5:	  inc	 di			;point to next attribute location
	  retrace			;wait for retrace
	  mov	 al,bl			;restore attribute character
	  stosb 			;store character in buffer
	  sti				;reenable
	  loop	 seta5			;loop till all written
	  jmp	 seta2			;then back to exit
sc_seta	  endp
	  page
;
;  SC_SETL - Set Line Attribute
;
;	  (dh)				number of line to set attribute on
;	  (al)				attribute to be set
;	  call	 sc_setl		call to set attributes
;
;  As with sc_seta, this routine operates directly on the display buffer.
;
;  For best handling of monochrome/color screens, the attribute in al should
;  be obtained from sc_snorm or sc_arev with the possible addition of 80h for
;  blinking and/or 08h for high intensity. The locations sc_snorm and sc_arev
;  are byte locations defined in this module and initialized by sc_init.
;
sc_setl	  proc	 near
	  push	 es			;save registers
	  pushall
	  mov	 di,dbtbl
	  mov	 es,sc_buf		;es:di = dbtbl in allocated segment
	  mov	 bl,dh			;bx contains line number
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]		;load offset of line start from dbtbl
	  shl	 di,1
	  inc	 di			;point to first attribute character
	  mov	 es,scseg		;set screen segment
	  cmp	 sc_vtype,cga		;jump if color board with flicker
	  je	 setl3
;
;  All adapters except CGA case
;
	  mov	 cx,dwidth		;else set dwidth chars
;
;  Loop to set attribute in entire line
;
setl1:	  stosb 			;store attribute
	  inc	 di			;point to next attribute character
	  loop	 setl1			;loop till all written
;
;  Exit after completing monochrome case
;
setl2:	  popall			;restore regs
	  pop	 es
	  ret				;exit
	  page
;
;  SC_SETL (continued)
;
;  CGA case
;
setl3:	  mov	 dx,crt_status		;set status port for color card
	  mov	 bl,al			;save attribute char
	  mov	 cx,dwidth		;set count for whole line
;
;  Loop through chars for color case
;
setl4:	  retrace			;wait for retrace
	  mov	 al,bl			;restore attribute character
	  stosb 			;store character in buffer
	  sti				;reenable
	  inc	 di			;bump to next attribute character
	  loop	 setl4			;loop till all written
	  jmp	 setl2			;exit
sc_setl	  endp
	  page
;  SC_TERM - Terminate screen operations
;
;  Releases the buffer allocated by SC_INIT
;
sc_term	  proc	near
	  push	sc_buf			;segment
	  call	_frees			;free segment
	  inc	sp
	  inc	sp
	  mov	sc_buf,0		;clear segment selector
	  ret
sc_term	  endp
	  page
;
;  SC_UPDLN - Update Line
;
;	  (dh)				line to update (0 to (dlines+mlines-1))
;	  call	 sc_updln		call to update line
;
;  This routine updates the display buffer so that the indicated line is set
;  from the screen buffer, respecting the current value of the display offset.
;
sc_updln  proc	 near
	  push	 es			;save registers
	  push	 ds
	  pushall
;
;  Get source location for copy
;
	  mov	 si,sbtbl
	  mov	 es,sc_buf
	  mov	 bl,dh			;point to line in screen buffer
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 si,es:[si+bx]
	  cmp	 dh,dlines		;test message line
	  jae	 updln0 		;if so, no offset
	  add	 si,sc_soff		;else add start offset
;
;  es:si = source location
;
;  Set destination location for copy
;
updln0:	  mov	 di,dbtbl
	  mov	 di,es:[di+bx]		;set offset into display buffer
	  shl	 di,1
	  mov	 cx,dwidth		;set dwidth characters on line
	  push	 es
	  push	 scseg			;screen segment
	  pop	 es			;es:di = destination
	  cmp	 sc_vtype,cga		;jump if color board with flicker
	  pop	 ds			;ds:si = source
	  je	 updln3
;
;  Loop through characters, all adapters except CGA case
;
updln1:	  movsb 			;copy next char to buffer
	  inc	 di			;bump past attribute
	  loop	 updln1 		;loop till all written
	  page
;
;  SC_UPDLN (continued)
;
;  Common exit point
;
updln2:	  popall	 			;restore regs
	  pop	 ds
	  pop	 es
	  ret				;exit
;
;  CGA case
;
updln3:	  mov	 dx,crt_status		;set status port
;
;  Loop through chars for color case
;
updln4:	  lodsb 			;load next char
	  mov	 bl,al			;save character
	  retrace			;wait for retrace
	  mov	 al,bl			;restore character
	  stosb 			;store character in buffer
	  inc	 di			;bump past attribute
	  sti				;reenable
	  loop	 updln4 		;loop till all written
	  jmp	 updln2 		;exit
sc_updln  endp
	  page
;
;  SC_CPYLN - Copy Line
;
;	  (dh)				line to update (0 to (dlines+mlines-1))
;         call   sc_cpyln               copy line from display to screen buffer
;
;  This routine updates the screen buffer so that the indicated line is set
;  from the display buffer, respecting the current value of the display offset.
;
sc_cpyln  proc   near
	  push	 es			;save registers
	  push	 ds
	  pushall
;
;  Get source location for copy
;
          mov    di,sbtbl
	  mov	 es,sc_buf
	  mov	 bl,dh			;point to line in screen buffer
	  xor	 bh,bh
	  shl	 bx,1
          mov    di,es:[di+bx]
	  cmp	 dh,dlines		;test message line
          jae    cpyln0                 ;if so, no offset
          add    di,sc_soff             ;else add start offset
;
;  es:di = destination location
;
;  Set source location for copy
;
cpyln0:   mov    si,dbtbl
          mov    si,es:[si+bx]          ;set offset into display buffer
          shl    si,1
	  mov	 cx,dwidth		;set dwidth characters on line
          mov    ds,scseg               ;screen segment
;
;  Loop through characters
;
cpyln1:   movsb                         ;copy next char to buffer
          inc    si                     ;bump past attribute
          loop   cpyln1                 ;loop till all written
          popall                                ;restore regs
	  pop	 ds
	  pop	 es
	  ret				;exit
sc_cpyln  endp
	  page
;
;  SC_UPDSC - Update Screen
;
;	  call	 sc_updsc		call to update screen
;
;  This routine synchronizes the display buffer with the current contents of
;  the screen buffer, respecting the current offset value set by sc_offset.
;  Only the first dlines lines are synchronized, the remaining mlines must be
;  synchronized using sc_write_char or sc_updln.
;
sc_updsc  proc	 near
	  pushall			;save registers
	  push	 es
;
;  Turn off display if snowy color case
;
	  cmp	 sc_vtype,cga		;check if color board with flicker
	  jne	 updsc2 		;jump if not
	  mov	 dx,03dah		;point to color board status port
;
;  Wait for display enable
;
updsc1:	  in	 al,dx			;get port value
	  test	 al,8			;wait vertical retrace
	  jz	 updsc1
;
;  Disable color display
;
	  mov	 al,25h 		;set value to disable display
	  mov	 dx,03d8h
	  out	 dx,al			;turn off display during vertical retrace
;
;  Prepare to copy data to screen
;
updsc2:	  mov	 es,scseg		;point to start of display buffer
	  sub	 di,di
	  mov	 bx,dwidth		;display width to BL
	  mov	 ax,sc_soff		;load screen offset
	  mov	 dx,swidth		;set count of chars not written on right
	  sub	 dx,bx
	  sub	 dx,ax
	  mov	 bh,dlines		;count to set first dlines lines
	  mov	 si,sbtbl		;point to start of line table
	  push	 ds
	  mov	 ds,sc_buf
	  mov	 si,[si]		;point to start of screen buffer
;
;  Loop through lines.
;   ax=sc_soff, bl=dwidth, bh=dlines, dx=count of chars not written on each line
;   ds:si=screen buf ptr, es:di=display buf ptr
;
updsc3:	  add	 si,ax			;add offset for this line
	  mov	 cl,bl			;set count to move dwidth chars
	  xor	 ch,ch
	  page
;
;  Loop to copy dwidth chars
;
updsc4:	  movsb 			;move a byte
	  inc	 di			;bump past attribute
	  loop	 updsc4 		;loop till all dwidth chars moved
;
;  Here after moving one line
;
	  add	 si,dx			;bump past skipped chars at end of line
	  dec	 bh			;loop till all dlines completed
	  jnz	 updsc3
	  pop	 ds
;
;  Turn snowy color screen back on
;
	  cmp	 sc_vtype,cga		;check if color board with flicker
	  jne	 updsc5 		;jump if not
	  mov	 al,crtmod		;restore CRT mode setting if color board
	  mov	 dx,03d8h
	  out	 dx,al
;
;  Common return point
;
updsc5:	  pop	 es			;restore registers
	  popall
	  ret				;return to caller
sc_updsc  endp
	  page
;
;  SC_CPYSC - Copy Screen
;
;         call   sc_cpysc               copy display buffer to screen buffer
;
;  This routine synchronizes the screen buffer with the current contents of
;  the display buffer, respecting the current offset value set by sc_offset.
;  Only the first dlines lines are synchronized, the remaining mlines must be
;  synchronized using sc_write_char or sc_updln.
;
sc_cpysc  proc   near
	  pushall			;save registers
	  push	 es
          push   ds
;
;  Prepare to copy data to screen
;
          mov    bx,dwidth              ;display width to BL
	  mov	 ax,sc_soff		;load screen offset
	  mov	 dx,swidth		;set count of chars not written on right
	  sub	 dx,bx
	  sub	 dx,ax
	  mov	 bh,dlines		;count to set first dlines lines
          mov    si,dbtbl
          mov    di,sbtbl               ;point to start of line table
          mov    es,sc_buf
          mov    si,es:[si]             ;point to starting offset of display
          mov    di,es:[di]             ;point to start of display buffer
          mov    ds,scseg               ;point to start of display buffer
;
;  Loop through lines.
;   ax=sc_soff, bl=dwidth, bh=dlines, dx=count of chars not written on each line
;   ds:si=display buf ptr, es:di=screen buf ptr
;
cpysc3:   add    di,ax                  ;add offset for this line
	  mov	 cl,bl			;set count to move dwidth chars
	  xor	 ch,ch
	  page
;
;  Loop to copy dwidth chars
;
cpysc4:   movsb                         ;move a byte
	  inc	 si			;bump past attribute
          loop   cpysc4                 ;loop till all dwidth chars moved
;
;  Here after moving one line
;
          add    di,dx                  ;bump past skipped chars at end of line
	  dec	 bh			;loop till all dlines completed
          jnz    cpysc3
	  pop	 ds
          pop    es                     ;restore registers
	  popall
	  ret				;return to caller
sc_cpysc  endp
	  page
;
;  SC_WRITE - Write String
;
if LDATA
;	  (es:si)			pointer to string
else
;	  (si)				pointer to string
endif
;	  (al)				number of characters to write (0 to swidth)
;	  (dh,dl)			line/char of starting write position
;	  call	 sc_write		call to write string
;	  (dl)				incremented by length of string
;
;  The data should not extend past the end of the display line. This routine
;  operates on the screen buffer, the display buffer is not affected.
;
sc_write  proc	 near
	  pushall			;save registers
	  push	 es
if LDATA
	  push	 ds
	  push	 es			;string data segment
endif
	  mov	 cl,al			;count of characters to CX
	  xor	 ch,ch
	  mov	 es,sc_buf
	  mov	 di,sbtbl
	  mov	 bl,dh			;get buffer position
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]
	  mov	 bl,dl
	  xor	 bh,bh
	  add	 di,bx
if LDATA
	  pop	 ds
endif
	  rep	 movsb			;move characters to screen buffer
if LDATA
	  pop	 ds
endif
	  pop	 es			;restore regs
	  popall
	  add	 dl,al			;bump cursor
	  ret				;exit
sc_write  endp
	  page
;
;  SC_WRITE_ATTR - Write Attribute String
;
if LDATA
;	  (es:si)			pointer to string
else
;	  (si)				pointer to string
endif
;	  (al)				number of characters to write (0 to swidth)
;	  (dh,dl)			line/char of starting write position
;         call   sc_write_attr          call to write attribute string
;
;  The data should not extend past the end of the display line. This routine
;  operates on the display buffer, the screen buffer is not affected.
;
sc_write_attr proc   near
	  pushall			;save registers
	  push	 es
if LDATA
	  push	 ds
	  push	 es			;string data segment
endif
	  mov	 cl,al			;count of characters to CX
	  xor	 ch,ch
	  mov	 es,sc_buf
          mov    di,dbtbl
	  mov	 bl,dh			;get buffer position
	  xor	 bh,bh
          shl    bx,1                   ;line times 2
	  mov	 di,es:[di+bx]
	  mov	 bl,dl
	  xor	 bh,bh
	  add	 di,bx
          shl    di,1                   ;two bytes per display cell
          mov    es,scseg               ;point to display memory
if LDATA
	  pop	 ds
endif
scwra1:   inc    di                     ;point to attribute
          movsb                         ;move attributes to display buffer
          loop   scwra1
if LDATA
	  pop	 ds
endif
	  pop	 es			;restore regs
	  popall
          ret                           ;exit
sc_write_attr endp
	  page
;
;  SC_WRITE_CHAR - Write Character
;
;	  (al)				character to be written
;	  (dh,dl)			line/char of position for write
;	  call	 sc_write_char		call to write character
;	  (dl)				incremented by one
;
;  This routine changes the corresponding character in the screen buffer. It
;  also updates the character in the display buffer if the offset is such
;  that the position is on the screen. It is unlike most other routines in
;  this module in this respect (the other routines do not update the screen).
;
sc_write_char proc near
	  push	 es			;save registers
	  pushall
	  mov	 es,sc_buf
	  mov	 di,sbtbl
	  mov	 bl,dh			;get address in screen buffer
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,es:[di+bx]
	  mov	 bl,dl
	  xor	 bh,bh
	  add	 di,bx
	  stosb 			;store character in screen buffer
;
;  See if character is currently displayed
;
	  mov	 bl,dh			;get offset of line start in display buf
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 di,dbtbl
	  mov	 di,es:[di+bx]
	  mov	 bl,dl			;set bx = column
	  xor	 bh,bh
;
;  Check off left side of screen
;
	  cmp	 dh,dlines		;test on message line
	  jae	 writec0		;jump if so
	  sub	 bl,byte ptr sc_soff	;else adjust column by left offset
	  jb	 writec1		;jump if character is off to left
;
;  Check off right side of screen, if ok set pointers for move
;
writec0:  cmp	 dl,byte ptr dwidth	;jump if character is off to right
	  ja	 writec1
	  add	 di,bx			;es:di points to screen buffer char
	  shl	 di,1
	  mov	 es,scseg
	  cmp	 sc_vtype,cga		;jump if color board with flicker
	  je	 writec2
	  page
;
;  Non-CGA case
;
	  stosb 			;store character in buffer
;
;  Common return point
;
writec1:  popall			;restore regs
	  pop	 es
	  inc	 dl			;bump cursor
	  ret				;exit
;
;  CGA case
;
writec2:  mov	 dx,crt_status		;set status port
	  mov	 bl,al			;save character
	  retrace			;wait for retrace
	  mov	 al,bl			;restore character
	  stosb 			;store character in buffer
	  sti				;reenable
	  jmp	 writec1		;return
sc_write_char endp
;
	Code_EndS@
	  end
