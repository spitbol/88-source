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


	page	80,132
	title	HOST88 - 8088-specific HOST Functions
	name	HOST88
	include	systype.ah
	include rules.asi
	include	osint.inc
	include extrn88.inc
	jumps
;
;
;
;
;
;
;
;------------------------------------------------------------;
;							     ;
;	   I  B  M     P  C	S  P  I  T  B  O  L	     ;
;							     ;
;		  H O S T   F U N C T I O N		     ;
;							     ;
;			 Written By			     ;
;							     ;
;		     Robert B. K. Dewar 		     ;
;		       David  Shields			     ;
;							     ;
;			Version 3.04			     ;
;							     ;
;	      Copyright (c) 1982, Realia Inc.		     ;
;							     ;
;------------------------------------------------------------;
;
;  This module provides the routines to implement the external procedure (EXP
;  definition) for the SPITBOL HOST function (SYSHS). See the main interface
;  module (SPITOSI) for full description of the interface environment.
	  page
;
;  REVISION HISTORY
;  ================
;
;  3.06	  14-Oct-1993 (MBE)
;	  Clear mscount when termhost is called, so a subsequently
;	  reloaded SAVE file causes the sound system to be recalibrated.
;
;  3.05   03-Sep-1993 (MBE)
;         Recalculate screen segment after
;	  video mode switch (HOST 210, Int 10, AH=0).
;
;  3.04   14 November 1990, 8:00 (MBE)
;	  Convert to 8088 version
;
;  3.03	  5 March 1990, 9:25 (MBE)
;	  Did not pop correct number of registers prior to jumping to exite
;	  upon bad character in play string.
;
;  3.02	  23 January 1990, 14:00 (MBE)
;	  Added host function 210 to provide BIOS/MS-DOS interrupt calls.
;
;  3.01   21 November 1989, 08:00 (MBE)
;	  Converted to 80386 32-bit segments using Phar Lap DOS Extender.
;	  Recode interface to get parameters and return results for
;	  Catspaw's OSINT written in C.  Function host386 is called by
;	  zyshs() for host functions greater than 4.
;	  Peek/Poke/Set Address functions do not initalize the screen.
;
;  2.14   28 January 1985, 18:25 (RBKD)
;	  Fix error of clobbering data area in function load
;	   (error is in SYSLD in SPITOSI module)
;
;  2.13   04 April 1984, 06:50 (RBKD)
;	  New version, no changes
;
;  2.12   03 April 1984, 11:30 (RBKD)
;	  Reset colctr to zero in termhost
;
;  2.11   01 April 1984, 13:20 (RBKD)
;	  New version number, no other changes
;	  Fix error in HOST(2) introduced in 2.08
;
;  2.08   01 April 1984, 08:10 (RBKD)
;	  Make use of new FOLDL procedure to fold lower case
;
;  2.07   31 March 1984, 19:00 (RBKD)
;	  Remove use of dseg (data seg addr), use ss instead
;
;  2.06   31 March 1984, 10:00 (RBKD)
;	  HOST(2,..) should signal failure in DOS 1.1
;
;  2.05   30 March 1984, 10:00 (RBKD)
;	  Require null string for host identification case
;
;  2.04   29 March 1984, 17:20 (RBKD)
;	  New allocb/freeb calling sequence
;
;  2.03   29 March 1984, 08:20 (RBKD)
;	  New screen operations module
;	  Allocate screen buffer dynamically
;
;  2.02   28 March 1984, 02:30 (RBKD)
;	  Fix bad version number in host message
;	  HOST(2,x) fails rather than returns null if no string
;	  Add peek/poke facility
;
;  2.01   22 March 1984, 06:00 (RBKD)
;	  Initial version with full support
	  page
;
;  External references to data segment locations in OSINT.
;
	Header@

	Data_Seg@
;
;  BIOS constants
;
timer	  equ	46ch		;32-bit BIOS-maintained clock counter
mem64k	  equ	0h		;segment for first 64Kb of RAM

;
;  Integer and string blocks that can be returned by host function
;
	  ext  	reg_wa	  ,word       ;register WA  (host arg 1)
	  ext  	reg_xl	  ,word       ;register XL  (host arg 2)
	  ext  	reg_xr	  ,word       ;register XR  (host arg 3)
	  ext  	reg_wb	  ,word       ;register WB  (host arg 4)
	  ext  	reg_wc	  ,word       ;register WC  (host arg 5)
	  ext	mindata_ds,word

	  extrn  sc_anorm	  :byte  ;normal attribute
	  extrn  sc_arev	  :byte  ;reverse video version
	  extrn  sc_dtype	  :byte  ;display type
	  extrn  sc_soff	  :word  ;screen offset
	  extrn  sc_vtype	  :byte  ;extended display type

;  Structure of a vector block used to pass registers to HOST(210)
;
vcblk	  struc
vctyp	  dw	 ?			;type word
vcidv	  dw	 ?			;block idval
vclen	  dw	 ?			;block length
vcax	  dw	 ?			;ax integer pointer
vcbx	  dw	 ?			;bx integer pointer
vccx	  dw	 ?			;cx integer pointer
vcdx	  dw	 ?			;dx integer pointer
vcsi	  dw	 ?			;si integer pointer
vcdi	  dw	 ?			;di integer pointer
vcbp	  dw	 ?			;bp integer pointer
vcds	  dw	 ?			;ds integer pointer
vces	  dw	 ?			;es integer pointer
vcflg	  dw	 ?			;flags integer pointer
vcblk	  ends


;
;  Special exit codes in AX
;
EXIT_1	  equ	 0			;erroneous argument
EXIT_2	  equ	 4			;execution error
EXIT_3	  equ	 8			;pointer to SCBLK or 0 in reg_xl
EXIT_4	  equ	 12			;return NULL string
EXIT_5	  equ	 16			;return result in reg_xr
EXIT_6	  equ	 20			;cause statement failure
EXIT_7	  equ	 24			;return string in reg_xl, length in reg_wa
EXIT_8	  equ    28			;return copy of result in reg_xr

;
;  Flag used to indicate if SCOPS has been initialized
;
scinit	  db	 0		       ;screen operations initialization flag
;
;  Current screen position
;
	  even
	  public linecol
linecol	  label  word		       ;line and column together
col	  db	 ?		       ;current column position
line	  db	 ?		       ;current line position
;
;  Record size of screen
;
ncols	  dw	 ?			;number of columns
nlines	  dw	 ?			;number of lines
	  page
;
;  Peek/poke control locations
;
	  even
peekptr	  label	 dword
peekofs	  dw	 0		       ;peek offset
peekseg	  dw	 0		       ;peek segment
;
;
;  Current settings for music output (initialized to default)
;
octave	  db	 4		       ;current octave (0-6)
nlength	  db	 4		       ;current note length (1-64)
tempo	  dw	 120		       ;current tempo (quarter notes/minute)
mscount   dw     0		       ;count used for 1 ms delay loop
mode	  db     'N'                   ;N/L/S for normal/legato/staccato
;
;  Table of note indexes by letter
;
notex	  db	 10		       ;A
	  db	 12		       ;B
	  db	 1		       ;C
	  db	 3		       ;D
	  db	 5		       ;E
	  db	 6		       ;F
	  db	 8		       ;G
;
;  Table of octave 6 frequencies
;
	  even
freq	  dw	 1976		       ;0 - c flat
	  dw	 2093		       ;1 - c
	  dw	 2217		       ;2 - c sharp or d flat
	  dw	 2349		       ;3 - d
	  dw	 2489		       ;4 - d sharp or e flat
	  dw	 2637		       ;5 - e or f flat
	  dw	 2794		       ;6 - e sharp or f
	  dw	 2960		       ;7 - f sharp or g flat
	  dw	 3136		       ;8 - g
	  dw	 3322		       ;9 - g sharp or a flat
	  dw	 3520		       ;10 - a
	  dw	 3729		       ;11 - a sharp or b flat
	  dw	 3951		       ;12 - b
	  dw	 4186		       ;13 - b sharp
;
	  page
;
;  Dispatch table for HOST sub-functions
;
	  even
hjump	  dw	 h00$		       ;0 - invalid function
	  dw	 h00$		       ;1 - performed by zyshs
	  dw	 h00$		       ;2 - performed by zyshs
	  dw	 h00$		       ;3 - performed by zyshs
	  dw	 h00$		       ;4 - performed by zyshs
	  dw	 h05$		       ;5 - set cursor type
	  dw	 h06$		       ;6 - set screen position
	  dw	 h07$		       ;7 - test keyboard data available
	  dw	 h08$		       ;8 - read keyboard
	  dw	 h09$		       ;9 - read field with echo
	  dw	 h10$		       ;10 - clear screen
	  dw	 h11$		       ;11 - delete character
	  dw	 h12$		       ;12 - erase to end of line
	  dw	 h13$		       ;13 - insert character
	  dw	 h14$		       ;14 - read string
	  dw	 h15$		       ;15 - read character
	  dw	 h16$		       ;16 - roll down
	  dw	 h17$		       ;17 - roll up
	  dw	 h18$		       ;18 - set attribute
	  dw	 h19$		       ;19 - set line attribute
	  dw	 h20$		       ;20 - update line
	  dw	 h21$		       ;21 - update screen
	  dw	 h22$		       ;22 - write string
	  dw	 h23$		       ;23 - write character
	  dw	 h24$		       ;24 - get normal attribute
	  dw	 h25$		       ;25 - get reversed attribute
	  dw	 h26$		       ;26 - get display type
	  dw	 h27$		       ;27 - set peek/poke address
	  dw	 h28$		       ;28 - peek
	  dw	 h29$		       ;29 - poke
          dw     h30$                  ;30 - read keyboard special
          dw     h31$                  ;31 - read attribute string
          dw     h32$                  ;32 - write attribute string
          dw     h33$                  ;33 - copy display line to buffer
          dw     h34$                  ;34 - copy full display to buffer
          dw     h35$                  ;35 - scroll display buffer
          dw     h36$                  ;36 - get keyboard state
          dw     h200$                 ;200 - sound tone
	  dw	 h201$		       ;201 - play tune
	  dw	 h202$		       ;202 - get number of columns
	  dw	 h203$		       ;203 - get extended type
	  dw	 h204$		       ;204 - get font size
	  dw	 h205$		       ;205 - get number of lines
	  dw	 h206$		       ;206 - get video mode
	  dw	 h207$		       ;207 - get current display page
	  dw	 h208$		       ;208 - set screen offset
	  dw	 h209$		       ;209 - set screen buffer size
	  dw	 h210$		       ;210 - perform BIOS/MS-DOS call
          dw     h211$                 ;211 - set number of physical lines
          dw     h212$                 ;212 - get cursor column number
          dw     h213$                 ;213 - get cursor row number
;
; Temp word used to hold converted integer argument
;
if IA32
itemp	  dd	 ?		       ;used by fetchi
else
itemp	  dw	 ?		       ;used by fetchi
endif
;
;
maxlow    equ    36                    ;end of low functions
highf	  equ	 200		       ;start of high functions
maxhigh	  equ	 213		       ;end of high functions
maxf	  equ	 maxlow+(maxhigh-highf)+1  ;maximum valid function code

	Data_EndS@
;
;  External references to main SPITBOL module
;
	Code_Seg@

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
;
;  External references to screen and keyboard operations module
;
	  ext	 cinread	  ,near  ;buffered keyboard read
	  ext  	 getint	  	  ,near  ;get integer with string conversion
	  extrn  kb_alarm	  :near  ;sound alarm
	  extrn  kb_nloff	  :near  ;turn Num Lock mode off
	  extrn  kb_read	  :near  ;read keyboard
          extrn  kb_state         :near  ;get state of shift/alt/ctrl keys
          extrn  kb_test          :near  ;test keyboard data
	  ext    pollevent	  ,near  ;allow control-C in
	  extrn  sc_clr_scrn	  :near  ;clear screen
	  extrn  sc_coff	  :near  ;turn cursor off
	  extrn  sc_con 	  :near  ;turn cursor on
          extrn  sc_cpyln         :near  ;copy line
          extrn  sc_cpysc         :near  ;copy screen
          extrn  sc_ctype         :near  ;set cursor type and visibility
	  extrn  sc_delete_char	  :near  ;delete character in line
	  extrn  sc_erase_to_eol  :near  ;erase to end of line
	  extrn  sc_init	  :near  ;initialize
	  extrn  sc_insert_char	  :near  ;insert character in line
	  extrn  sc_offset	  :near  ;set screen buffer offset
	  extrn  sc_read	  :near  ;read string
          extrn  sc_read_attr     :near  ;read attribute string
          extrn  sc_read_char     :near  ;read character
	  extrn  sc_roll_down	  :near  ;roll screen down
	  extrn  sc_roll_up	  :near  ;roll screen up
          extrn  sc_scroll        :near  ;scroll display buffer
          extrn  sc_seg           :near  ;compute screen segment
	  extrn  sc_seta	  :near  ;set attributes
	  extrn  sc_setl	  :near  ;set attribute for line
	  extrn  sc_term	  :near  ;terminate
	  extrn  sc_updln	  :near  ;update line
	  extrn  sc_updsc	  :near  ;update screen
	  extrn  sc_write	  :near  ;write string
          extrn  sc_write_attr    :near  ;write attribute string
          extrn  sc_write_char    :near  ;write character
	  extrn  vm_cols	  :near  ;get number of screen columns
	  extrn  vm_fsize	  :near  ;get font size
	  extrn  vm_lines	  :near  ;get number of screen lines
	  extrn  vm_setlines	  :near  ;set number of screen lines
	  extrn  vm_mode	  :near  ;get video mode
	  extrn  vm_page	  :near  ;get active video page
	  extrn	 vm_type	  :near  ;get video adapter type
	  page
;
;  host386 - Give access to host computer features
;
;	 -(sp)			       argument 1
;	  call	 host88
;
host88	  proc	 near
	  publab host88
	  push	 bp
	  mov	 bp,sp
	  mov	 ax,[bp+4]	       ;get function code
	  pop	 bp
	  push	 si		       ;save C registers
	  push	 di
	  push	 bx
	  cmp	 ax,highf	       ;does this number need to be adjusted?
	  jb     short syshs1		       ;jump if no adjustment needed
	  add    ax,maxlow+1-highf     ;place just above low range
syshs1:	  cmp	 ax,maxf+1	       ;set CF if 0 or in range
	  sbb    bx,bx	               ;1's if 0 or in range, else 0's
	  and	 bx,ax		       ;reset to 0 if out of range
	  shl	 bx,1
	  jmp	 hjump[bx]	       ;jump to proper routine
host88	  endp
	  page
;
;  EXITx - Exit points
;
;  Exit returning character in al as one character string
;
exitc:	  mov	 cl,al		       ;preserve character
	  GETOFF di,TSCBLK
	  mov	 ES_[di].scstr,cl      ;store character
	  mov	 ax,1		       ;length 1
	  jmp	 short exits	       ;exit returning string
;
;  Exit signalling error
;
exite:	  mov	 ax,EXIT_1	       ;erroneous argument return
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2
;
;  Exit signalling failure
;
exitf:	  mov	 ax,EXIT_6	       ;failure return
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2
	  page
;
;  EXITx Exit Points (continued)
;
;  Exit returning byte in al as unsigned integer
;
exitb:	  xor	 ah,ah		       ;convert to integer
;				       ;fall into exiti code
;  Exit returning integer in ax
;
exiti:	  xchg	 bx,ax		       ;save integer value
	  GETOFF di,TICBLK
	  mov	 reg_xr,di	       ;return ticblk in XR
	  GETOFF ax,B_ICL
	  stosw			       ;set block type
	  mov	 ax,bx		       ;store value
	  stosw
if IA32
	  cwd			       ;return 32-bit value
	  mov	 ax,dx
	  stosw
endif
	  mov	 ax,EXIT_8	       ;copy result in XR
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2

;
;  Exit returning null
;
exitn:	  mov	 ax,EXIT_4	       ;null string return
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2

;
;  Exit returning string in tscblk.str, length in ax
;
exits:	  mov	 bx,ax		       ;save length
	  GETOFF di,TSCBLK	       ;character buffer block
	  mov	 reg_xl,di	       ;point XL to block
	  GETOFF ax,B_SCL	       ;string block type word
	  stosw			       ;set into block
	  mov	 ES_[di],bx	       ;store length into block
	  mov	 ax,EXIT_3	       ;return 3, XL points to SCBLK
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2

;
;  Exit signalling execution error
;
exitx:	  mov	 ax,EXIT_2	       ;return 2
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2

;
;  Exit returning value in XR (reg_xr)
;
exitxr:	  mov	 ax,EXIT_5	       ;return 5
	  pop	 bx		       ;restore C register
	  pop	 di
	  pop	 si
	  retc	 2
	  page

;
;  HOST processing functions
;

;
;  HOST(0) or out of range argument (error)
;
h00$:	  jmp	 exite		       ;give error exit
	  page
;
;
;  HOST(5,type) - Set Cursor Type
;
;    Type = 0  Normal DOS underline cursor
;    Type = 1  Full block cursor
;    Type = 2  Half block low on line
;    Type = 3  Half block high on line
;    Type = 4  Original cursor at system startup.
;
;    Visibility = 0  Turn on only during input
;    Visibility = 1  Turn on all the time
;
h05$:     call   sgetii                ;get integer arguments
	  cmp	 ax,4		       ;must be 0-4
	  ja	 exite		       ;else error
          call   sc_ctype              ;if OK, set type and visibility
          mov    dx,linecol            ;set position
          call   sc_con                ;maybe turn cursor on
          call   sc_coff               ;maybe turn cursor off
          jmp    exitb                 ;and previous cursor type
;
;  HOST(6,line,col) - Set Screen Position
;
h06$:	  call	 sgetii 	       ;get two integer arguments
	  cmp	 ax,nlines	       ;line must be 0 to nlines-1
	  jae	 exite		       ;
	  cmp	 dx,ncols	       ;col must be 0 to ncols-1
	  jae	 exite		       ;
	  mov	 line,al	       ;else save line/col position
	  mov	 col,dl 	       ;
	  jmp	 exitn		       ;and return null
;
;  HOST(7) - Test Keyboard Status
;
h07$:	  call	 getn		       ;No parameters required
          mov    dx,linecol            ;get position
          call   sc_con                ;prompt user for input
          call   kb_test               ;test key available
;          pushf
;          call   sc_coff               ;cursor off
;          popf
          jnz    exitn                 ;if available, return null
	  jmp	 exitf		       ;else failure return
;
;  HOST(8) - Read Keyboard
;
h08$:	  call	 sgetn		       ;No parameters required
          mov    dx,linecol            ;get position
	  call	 kb_test	       ;test key available
	  jnz	 short h08$1	       ;turn cursor on if not
	  call	 sc_con 	       ;character not available
h08$1:	  call	 kb_read	       ;read key
	  call	 sc_coff	       ;cursor off
          jmp    exitb                 ;return byte result as integer
;
;  HOST(30) - Read Keyboard Special
;
h30$:     call   sgetn                 ;No parameters required
          mov    dx,linecol            ;get position
	  call	 kb_test	       ;test key available
          jnz    short h30$1           ;turn cursor on if not
	  call	 sc_con 	       ;character not available
h30$1:    call   kb_read               ;read key
          mov    bx,dx                 ;preserve scan/ASCII codes
	  call	 sc_coff	       ;cursor off
          or     bl,bl                 ;normal ASCII code?
          jnz    short h30$2           ;just return ASCII code as is
          cmp    bh,3
          je     short h30$2           ;special case for null
          xor    ah,ah                 ;bias result by 1000
          add    ax,1000
	  jmp	 exiti		       ;return result as integer
h30$2:    mov    al,bl                 ;return ASCII code
          jmp    exitb                 ;return byte result as integer
	  page
;
;  HOST(9,length) - Read Field
;
h09$:	  call	 sgeti		       ;get integer argument
	  mov	 cx,ax		       ;save argument
	  mov	 bl,col
	  xor	 bh,bh
	  add	 ax,bx		       ;check there is room
	  cmp	 ax,ncols
	  ja	 exite		       ;else error exit
;
;  First blank the field, both in the screen buffer and the screen itself
;
	  GETOFF di,TSCBLK
	  pushES_
	  lea	 di,[di].scstr
	  push	 di
	  push	 cx		       ;push length
	  mov	 si,di		       ;save in si
	  mov	 al,' '                ;store blanks
    rep   stosb
	  mov	 dx,linecol	       ;set cursor position
	  pop	 ax		       ;restore length
	  call	 sc_write	       ;write blanks to tscblk.str+2
	  call	 sc_updsc	       ;update screen
          mov    dx,linecol            ;get cursor position
	  call	 sc_con		       ;turn cursor on
	  pop	 di		       ;address of TSCBLK.scstr
	  push	 di
	  push	 ax		       ;buffer size
	  pushES_
	  push	 di		       ;buffer
	  xor	 ax,ax
	  push	 ax		       ;standard input
	  callc	 cinread,dPtrSize+4      ;ax = call(fd, buf, size)
	  call	 sc_coff	       ;turn cursor off
	  pop	 si		       ;address of TSCBLK.scstr
	  mov	 di,si		       ;prepare for SCASB
	  mov	 cx,ax		       ;number of chars returned by cinread
	  mov	 bx,ax
	  mov	 al,13		       ;carriage return
	  popES_
   repne  scasb			       ;search for it
	  jne	 short h09$1	       ;not found
          inc	 cx		       ;found CR.  Want to remove it.
	  sub	 bx,cx		       ;adjust count
h09$1:	  mov	 ax,bx		       ;number of good chars
	  mov	 dx,linecol	       ;position to start of field
	  call	 sc_write	       ;copy in the new data
	  mov	 linecol,dx	       ;update position past data read
;
;  Set string result and return
;
	  jmp	 exits		       ;return string
	  page
;
;  HOST(10) - Clear Screen
;
h10$:	  call	 sgetn		       ;no parameters
	  call	 sc_clr_scrn	       ;clear screen
	  mov	 linecol,0
	  jmp	 exitn		       ;return null
;
;  HOST(11) - Delete Character
;
h11$:	  call	 sgetn		       ;no parameters
	  mov	 dx,linecol	       ;set position
	  call	 sc_delete_char        ;delete character
	  jmp	 exitn		       ;return null
;
;  HOST(12) - Erase to End of Line
;
h12$:	  call	 sgetn		       ;no parameters
	  mov	 dx,linecol	       ;set position
	  call	 sc_erase_to_eol       ;erase to end of line
	  jmp	 exitn		       ;return null
;
;  HOST(13) - Insert Character
;
h13$:	  call	 sgetn		       ;no parameters
	  mov	 dx,linecol	       ;set position
	  call	 sc_insert_char        ;erase to end of line
	  jmp	 exitn		       ;return null
;
;  HOST(14,length) - Read String
;
h14$:	  call	 sgeti		       ;get length argument
	  mov	 bl,col
	  xor	 bh,bh
	  add	 bx,ax		       ;check there is room
	  cmp	 bx,ncols	       ;error if too long
	  ja	 exite		       ;
;
;  Here if arguments are valid
;
	  push	 ax
	  GETOFF di,TSCBLK
	  pop	 ax
	  lea	 di,[di].scstr	       ;point to char buffer
	  mov	 dx,linecol	       ;set position
	  call	 sc_read	       ;perform read
	  mov	 linecol,dx	       ;store resulting position
	  jmp	 exits		       ;exit returning string
	  page
;
;  HOST(15) - Read Character
;
h15$:	  call	 sgetn		       ;no parameters
	  mov	 dx,linecol	       ;set position
	  call	 sc_read_char	       ;perform read
	  mov	 linecol,dx	       ;store resulting position
	  jmp	 exitc		       ;exit returning character
;
;  HOST(16,top,bottom) - Scroll Down
;
h16$:	  call	 sgetii 	       ;get two integer arguments
	  cmp	 ax,dx		       ;error if null region
	  ja	 exite		       ;
	  cmp	 dx,nlines	       ;error if out of range
	  jae	 exite		       ;
	  mov	 ah,al		       ;copy top line
	  mov	 al,dl		       ;set bottom line
	  dec	 al		       ;adjust bottom - 1 for scroll call
	  call	 sc_roll_down	       ;do scroll down
	  jmp	 exitn		       ;exit returning null
;
;  HOST(17,top,bottom) - Scroll Up
;
h17$:	  call	 sgetii 	       ;get two integer arguments
	  cmp	 ax,dx		       ;error if null region
	  ja	 exite		       ;
	  cmp	 dx,nlines	       ;error if out of range
	  jae	 exite		       ;
	  mov	 ah,al		       ;copy top line
	  inc	 ah		       ;adjust for scroll call
	  mov	 al,dl		       ;set bottom line
	  call	 sc_roll_up	       ;do scroll up
	  jmp	 exitn		       ;exit returning null
;
;  HOST(18,count,attr) - Set Attribute
;
h18$:	  call	 sgetii 	       ;get integer arguments
	  mov	 cx,ax		       ;set count
	  mov	 al,dl		       ;set attribute
	  mov	 dx,linecol	       ;set position
	  call	 sc_seta	       ;set attributes
	  jmp	 exitn		       ;return null
	  page
;
;  HOST(19,line,attr) - Set Line Attribute
;
h19$:	  call	 sgetii 	       ;get integer arguments
	  cmp	 ax,nlines	       ;error if line out of range
	  jae	 exite		       ;
	  mov	 dh,al		       ;else set line
	  mov	 al,dl		       ;set attribute
	  call	 sc_setl	       ;
	  jmp	 exitn		       ;return null
;
;  HOST(20,line) - Update Line
;
h20$:	  call	 sgeti		       ;get integer arguments
	  cmp	 ax,nlines	       ;error if line out of range
	  jae	 exite		       ;
	  mov	 dh,al		       ;else set line
	  call	 sc_updln	       ;update line
	  jmp	 exitn		       ;return null
;
;  HOST(21) - Update Screen
;
h21$:	  call	 sgetn		       ;no parameters
	  call	 sc_updsc	       ;update screen
	  jmp	 exitn		       ;return null
;
;  HOST(22) - Write String
;
h22$:	  call	 sgets		       ;get string argument to es:si,ax
	  mov	 dx,linecol	       ;set position
	  mov	 cl,dl		       ;check length
	  xor	 ch,ch
	  add	 cx,ax		       ;
	  cmp	 cx,ncols	       ;error if string is too long
	  ja	 exite		       ;else error exit
	  call	 sc_write	       ;write string
	  mov	 linecol,dx	       ;store updated position
	  jmp	 exitn		       ;exit returning null
	  page
;
;  HOST(23) - Write Character
;
h23$:	  call	 sgets		       ;get string argument
	  cmp	 ax,1		       ;error if not 1 char string
	  jne	 exite		       ;
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb
	  mov	 dx,linecol	       ;set position
	  call	 sc_write_char	       ;write string
	  mov	 linecol,dx	       ;store updated position
	  jmp	 exitn		       ;exit returning null
;
;  HOST(24) - Get Normal Attribute
;
h24$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_anorm	       ;get normal attribute
	  jmp	 exitb		       ;exit with integer value
;
;  HOST(25) - Get Reverse Attribute
;
h25$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_arev	       ;get reverse attribute
	  jmp	 exitb		       ;exit with integer value
;
;  HOST(26) - Get Display Type
;
h26$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_dtype	       ;get display type
          cmp    al,'Q'                ;change COMPAQ to monochrome
	  jne	 short h26$1		       ;
          mov    al,'M'                ;
h26$1:	  jmp	 exitc		       ;exit returning character
;
;  HOST(27,seg,ofs) - Set Peek/poke Address
;
h27$:	  call	 getii 		       ;get two integer parameters
	  mov	 peekseg,ax	       ;store segment
	  mov	 peekofs,dx	       ;store offset
	  jmp	 exitn		       ;exit with null value
;
;  HOST(28) - Peek
;
;  Use DS for peeks.
;
h28$:	  call	 getn		       ;no parameters
	  push	 ds
	  lds	 si,peekptr	       ;point to peek address
	  lodsb			       ;load byte value
	  pop	 ds
	  mov	 peekofs,si	       ;save new pointer
	  jmp	 exitb		       ;exit with integer value
	  page
;
;  HOST(29,byte) - Poke
;
;  Use ES for pokes.
;
h29$:	  call	 geti		       ;get integer parameter
	  or	 ah,ah		       ;check MSByte
	  jnz	 exite		       ;error if non-zero
	  push	 es
	  les	 di,peekptr	       ;poke address
	  stosb			       ;store byte value
	  pop	 es
	  mov	 peekofs,di	       ;save new pointer
	  jmp	 exitn		       ;exit with null value
	  page
;
;  HOST(31,length) - Read Attribute String
;
h31$:     call   sgeti                 ;get length argument
	  mov	 bl,col
	  xor	 bh,bh
	  add	 bx,ax		       ;check there is room
	  cmp	 bx,ncols	       ;error if too long
	  ja	 exite		       ;
;
;  Here if arguments are valid
;
	  push	 ax
	  GETOFF di,TSCBLK
	  pop	 ax
	  lea	 di,[di].scstr	       ;point to char buffer
	  mov	 dx,linecol	       ;set position
          call   sc_read_attr          ;perform read
          jmp    exits                 ;exit returning string
;
;  HOST(32,s) - Write Attribute String
;
h32$:     call   sgets                 ;get string argument to es:si,ax
	  mov	 dx,linecol	       ;set position
	  mov	 cl,dl		       ;check length
	  xor	 ch,ch
	  add	 cx,ax		       ;
	  cmp	 cx,ncols	       ;error if string is too long
	  ja	 exite		       ;else error exit
          call   sc_write_attr         ;write attribute string
          jmp    exitn                 ;exit returning null
	  page
;
;  HOST(33,line) - Copy Display Line to Screen Buffer
;
h33$:     call   sgeti                 ;get integer arguments
	  cmp	 ax,nlines	       ;error if line out of range
	  jae	 exite		       ;
	  mov	 dh,al		       ;else set line
          call   sc_cpyln              ;copy line
	  jmp	 exitn		       ;return null
;
;  HOST(34) - Copy Display to Screen Buffer
;
h34$:     call   sgetn                 ;no parameters
          call   sc_cpysc              ;copy screen
	  jmp	 exitn		       ;return null
;
;  HOST(35,row*256+col,row*256+col,nrows,attribute) - Scroll display buffer
;
h35$:     call   sgetii                ;get two integer arguments
          mov    si,ax                 ;upper left row/column (arg1)
          mov    di,dx                 ;lower right row/column (arg2)
          mov    ax,reg_wb
          call   fetchi                ;get arg 3
          je     exite
          push   ax                    ;push nrows (+=up, -=down)
          mov    ax,reg_wc 
          call   fetchi                ;get arg 4
          mov    bh,al                 ;attribute for blanked lines
          pop    ax                    ;nrows
          je     exite
          mov    cx,si                 ;upper left corner
          mov    dx,di                 ;lower right corner
          call   sc_scroll             ;do scroll
	  jmp	 exitn		       ;exit returning null
;
;  HOST(36) - Read Keyboard state
;
h36$:     call   sgetn                 ;no parameters
          call   kb_state              ;get state
          jmp    exiti                 ;exit returning integer
;
;  HOST(200,freq,duration) - Sound Alarm
;
h200$:	  call	 getii		       ;get two integer arguments
	  call	 note		       ;sound the note
	  jmp	 exitn		       ;return null
;
;  HOST(201,string) - Play Music
;
h201$:	  call	 _pollevent	       ;allow control-C in before playing
	  call	 gets		       ;get string argument to si, ax
	  mov	 bx,si
	  add	 bx,ax		       ;point past end of string
	  mov	 al,ES_[bx]	       ;save character following string
	  mov	 ES_[bx], byte ptr 0dh    ;store CR at end of string
	  call	 play		       ;play the music
	  mov	 ES_[bx],al	       ;restore character following string
	  jmp	 exitn		       ;return null
;
;  HOST(202) - Get number of columns
;
h202$:	  call	 sgetn		       ;no parameters
	  call	 vm_cols	       ;get number of columns
	  jmp	 exiti		       ;exit with integer value
	  page
;
;  HOST(203) - Get extended display type
;
h203$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_vtype	       ;get display type
	  jmp	 exitb		       ;exit with integer value
;
;  HOST(204) - Get font size in scan lines
;
h204$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_vtype	       ;get display type
	  xor	 ah,ah
	  call   vm_fsize	       ;get font code
	  jmp	 exiti		       ;exit with integer value
;
;  HOST(205) - Get number of screen lines
;
h205$:	  call	 sgetn		       ;no parameters
	  mov	 al,sc_vtype	       ;get display type
	  xor	 ah,ah
	  call	 vm_lines	       ;get number of lines
	  jmp	 exiti		       ;exit with integer value
;
;  HOST(206) - Get hardware video mode
;
h206$:	  call	 sgetn		       ;no parameters
	  call	 vm_mode
	  jmp	 exiti		       ;exit with integer value
;
;  HOST(207) - Get hardware video page
;
h207$:	  call	 sgetn		       ;no parameters
	  call	 vm_page
	  jmp	 exiti		       ;exit with integer value
;
;  HOST(208) - Set screen buffer horizontal offset
;
h208$:	  call	 sgeti		       ;get integer argument
	  call	 sc_offset	       ;set offset
	  mov	 ax,sc_soff	       ;fetch offset
	  jmp	 exiti		       ;exit returning new offset
;
;  HOST(209) - Set screen buffer height and width
;
h209$:	  call	 getii		       ;get arguments without initializing
	  mov	 bx,ax		       ;save desired number of screen lines
	  mov	 cx,dx		       ;save desired screen buffer width
	  call	 getlc		       ;get number of physical lines and columns
	  cmp	 bx,dx		       ;compare lines desired vs. available
	  ja	 exite		       ;error if too large
	  cmp	 cx,ax		       ;compare columns desired vs. available
	  jae	 short h209$1		       ;ok if desired exceeds available
	  mov	 cx,ax		       ;enlarge desired amount
h209$1:	  or	 ch,ch		       ;desired columns must be one byte
	  jnz	 exite		       ;error if too large
	  call	 _termhost	       ;terminate any previous screen usage
	  mov	 linecol,0	       ;set line and column to 0
	  mov	 ncols,cx	       ;virtual number of columns
	  mov	 nlines,dx	       ;physical number of lines
	  mov	 ch,al		       ;set physical screen width
	  mov	 dh,dl		       ;number of physical lines
	  sub	 dh,bl		       ;physical-display lines=message lines
	  mov	 dl,bl		       ;display lines
	  call	 sc_init	       ;initialize for new screen buffer
	  or	 ax,ax		       ;test if initialize was successful
	  je	 exitx		       ;initialization failed
	  inc	 scinit 	       ;set flag
	  jmp	 exitn		       ;return with null string
;
;  HOST(210) - Perform BIOS/MS-DOS interrupt
;

h210$:	  call	 geti		       ;get first argument (interrupt number)
	  mov	 byte ptr cs:h210$2+1,al  ;save interrupt number
	  mov	 bx,reg_xr	       ;get second argument pointer
	  GETOFF ax,B_VCT	       ;vector type word
	  cmp	 ES_[bx].vctyp,ax     ;check type word for vector
	  jne	 exite		       ;fail if not vector
	  cmp	 ES_[bx].vclen,size vcblk;confirm length
	  jne	 exite		       ;fail if vector not correct length
	  push	 bp		       ;save other registers
	  push	 es
	  mov	 bp,sp		       ;save sp prior to pushes
	  lea	 si,ES_[bx].vcax       ;point to first register
	  GETOFF bx,B_ICL	       ;integer block type
if LDATA
	  db	 26h
endif
	  lodsw			       ;fetch ax register
	  mov	 di,ax
	  cmp	 ES_[di].ictyp,bx      ;make sure it is an integer
	  jne	 short h210$6	       ;fail if not an integer block
	  mov	 byte ptr itemp, ah    ;save ah for INT 10 check later
	  push	 word ptr ES_[di].icval;save the lsh of value
	  mov	 cx,8		       ;remaining eight registers
h210$1:
if LDATA
	  db	 26h
endif
	  lodsw			       ;fetch next register
	  mov	 di,ax
	  cmp	 ES_[di].ictyp,bx      ;make sure it is an integer
	  jne	 short h210$6	       ;fail if not an integer block
	  push	 word ptr ES_[di].icval;save the lsh of value
	  loop	 h210$1		       ;push all registers
	  mov	 cs:saveds,ds	       ;save DS
	  pop	 es
	  pop	 ds
	  pop	 bp
	  pop	 di
	  pop	 si
	  pop	 dx
	  pop	 cx
	  pop	 bx
	  pop	 ax
	  jmp	 h210$2		       ;do INT call elsewhere to assure
				       ; CPU cache is flushed

h210$4:	  pushf                        ;save flags
	  push	 es		       ;save all registers
	  push	 ds
	  push	 bp
	  push	 di
	  push	 si
	  push	 dx
	  push	 cx
	  push	 bx
	  push	 ax
	  mov	 ds,cs:saveds	       ;restore SPITBOL's DS
	  mov	 bx,reg_xr
if LDATA
	  mov	 es,_mindata_ds	       ;Minimal's DS
endif
	  lea	 si,[bx].vcax          ;point to first register
	  mov	 cx,10		       ;process nine registers and flags
h210$5:
if LDATA
	  db	 26h
endif
	  lodsw			       ;fetch next register
	  mov	 di,ax
if IA32
	  pop	 ax                    ;get value
	  cwd			       ;convert to 32-bits
	  mov	 word ptr ES_[di].icval,ax   ;and save
	  mov	 word ptr ES_[di].icval+2,dx
else
	  pop	 word ptr ES_[di].icval;save the value
endif
	  loop	 h210$5		       ;pop all registers
	  pop	 es		       ;restore other registers
	  pop	 bp		       ;

	  cmp	 byte ptr cs:h210$2+1,10h ;test for video interrupt (INT 10H)
	  jne	 short h210$3	       ;jump if not video BIOS call
	  cmp	 byte ptr itemp, 0     ;test for Set Video Mode
	  jne	 short h210$3
	  			       ;user changed video mode.  Have to
	  call	 sc_seg		       ;recompute the screen segment
h210$3:	  jmp	 exitxr		       ;exit returning register array
;
;  Here if non-integer block detected in the array
;
h210$6:	  mov	 sp,bp		       ;discard anything pushed so far
	  pop	 es		       ;restore other registers
	  pop	 bp
	  jmp	 exite		       ;return with erroneous argument error

h210$2:	  int	 0		       ;INT opcode
	  jmp	 h210$4		       ;return to host code

saveds	  dw	 ?		       ;save DS here during call

;
;  HOST(211) - Set number of physical screen lines.
;
h211$:	  call	 _termhost	       ;kill old screen
	  call	 sgeti		       ;get integer argument
	  or	 ax,ax
	  je	 exite		       ;zero not allowed
	  cmp	 ax,255		       ;must be less than 255
	  ja	 exite
	  call	 vm_setlines	       ;set lines
	  jmp	 exitn		       ;exit returning new offset
;
;  HOST(212) - Get Current Cursor Column
;
h212$:	  call	 sgetn		       ;no parameters
	  mov	 al,col		       ;get column number
          jmp    exitb                 ;exit with byte value
;
;  HOST(213) - Get Current Cursor Row
;
h213$:	  call	 sgetn		       ;no parameters
	  mov	 al,line	       ;get row number
          jmp    exitb                 ;exit with byte value

;
;  End of HOST processing functions
;
	  page
;
;  GETxx, SGETxx - Parameter Get Routines
;
;  These routines obtain the corresponding parameters, set the segment
;  registers to the code segment, and in the case of the sget routines,
;  initialize scops if not already done.
;
;  Case of no parameters to fetch
;
sgetn	  proc	 near
	  cmp	 scinit,0	       ;init screen if not done yet
	  jnz	 short getn		       ;
	  call	 inits		       ;
;
;  Entry for screen initialize not required
;
getn:	  ret			       ;return
sgetn	  endp
;
;  Get string argument.
;
;  Return pointer in es_si, length in ax.
;
sgets	  proc	 near
	  cmp	 scinit,0	       ;init screen if not done yet
	  jnz	 short gets		       ;
	  call	 inits		       ;
;
;  Entry for screen initialize not required
;
gets:	  mov	 si,reg_xl	       ;get argument pointer
	  GETOFF ax,B_SCL	       ;string block type word
	  cmp	 ax,ES_[si]	       ;test string
	  je	 short gets1	       ;jump if so
	  pop	 ax		       ;else discard return
	  jmp	 exite		       ;and give error
;
;  Here if we have a string argument
;
gets1:	  lea	 si,[si+(offset sclen)]; point to length word
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsw			       ;get length if OK string
	  ret			       ;return, si points to string start
sgets	  endp
;
;  Get one integer argument. The argument is in AX on exit
;
sgeti	  proc	 near
	  cmp	 scinit,0	       ;init screen if not done yet
          jnz    short geti            ;
	  call	 inits		       ;
;
;  Entry for screen initialize not required
;
geti:	  mov	 ax,reg_xl	       ;get argument pointer
	  call   fetchi		       ;get integer
	  je	 short geti1	       ;jump if failure
	  ret			       ;return integer in ax
geti1:	  pop	 ax		       ;else discard return
	  jmp	 exite		       ;and give error
sgeti	  endp
;
;  Get two integer arguments. The first argument is in AX on exit,
;  and the second is in DX
;
sgetii	  proc	 near
	  cmp	 scinit,0	       ;init screen if not done yet
          jnz    short getii           ;
	  call	 inits		       ;
	  page
;
;  GETxx, SGETxx (continued)
;
;  Entry for screen initialize not required
;
getii:	  mov	 ax,reg_xr	       ;get second argument pointer
	  call	 fetchi		       ;get integer
	  jne	 short getii1 	       ;jump if success
	  pop	 ax		       ;else discard return
	  jmp	 exite		       ;and give error
;
;  Here if second argument is integer
;
getii1:	  push	 ax		       ;save second integer argument
	  mov	 ax,reg_xl	       ;get first argument pointer
	  call	 fetchi		       ;get first argument as integer
	  je	 short getii2 	       ;jump if failed
	  pop	 dx		       ;restore second argument
	  ret			       ;first argument in ax
getii2:	  pop	 ax		       ;else discard return
	  jmp	 exite		       ;and give error
sgetii	  endp
;
;  Routine to fetch integer argument, with conversion of string to integer.
;
;	  (ax)	 pointer to icblk or scblk
;	  call	 fetchi
;	  (ax)  integer result if successful
;	  ccZ	 true if failure, false if success
;         (bx,cx,dx) destroyed
;
fetchi	  proc	 near
	  mov	 bx,offset itemp       ;offset of temp to hold result
	  pushDS_
	  push	 bx
if LDATA
	  push   _mindata_ds
endif
	  push	 ax		       ;pointer to ic or sc block

	  callc	 getint,dPtrSize*2     ;getint(icp,pword)
	  or	 ax,ax		       ;set result code
	  mov	 ax,word ptr itemp     ;integer result (lsh)
	  ret
fetchi	  endp
	  page
;
;  GETLC - return number of physical lines and columns on display
;
;	  (none)
;	  call	 getlc
;	  (dx)			number of lines
;	  (ax)			number of columns
;	  (ccz)			set if no adapter or could not initialize
;
getlc	  proc	 near
	  call	 vm_type	       ;get adapter type
	  or	 ax,ax		       ;error if no adapter
	  je	 short getlc1		       ;cannot initialize
	  call	 vm_lines	       ;get number of lines
	  or	 ax,ax		       ;test if illegal adapter type
	  je	 short getlc1		       ;cannot initialize
	  mov	 dx,ax		       ;0 message lines, N display lines
	  call	 vm_cols	       ;get number of columns
	  or	 ax,ax		       ;reset Z code
getlc1:	  ret
getlc	  endp
	  page
;
;  Routine to initialize screen
;
inits	  proc	 near		       ;
	  call	 getlc		       ;get number of columns and lines
	  je	 short inits1		       ;jump if bad adapter
	  mov	 ncols,ax	       ;save number of columns
	  mov	 nlines,dx	       ;save number of lines
	  mov	 ch,al		       ;physical screen width
	  mov	 cl,al	   	       ;logical screen width
	  call	 sc_init	       ;initialize screen
	  or	 ax,ax		       ;test if initialize was successful
	  je	 short inits1	       ;initialization failed
	  inc	 scinit 	       ;set flag
	  mov	 linecol,0	       ;set line and column to 0
	  ret			       ;return to caller
;
;  Here if screen buffer allocation error
;
inits1:   pop	 ax	 	       ;discard inits return
	  pop	 ax		       ;discard SGETx return
	  jmp	 exitx		       ;take exec error return
inits	  endp			       ;
	  page
;
;  TERMHOST - Terminate Host Function
;
;  This routine is called prior to exiting to DOS at the end of the
;  program, or when a SAVE file is generated, or when normal screen
;  input/output is performed. In each case the screen is blanked and
;  the cursor reset to the home position.
;
	  public  _termhost	       ;			      -- 2.03 --
_termhost  proc	 near		       ;
	  cmp	 scinit,0	       ;jump if screen not initialized
	  jz	 short termh1 	       ;
	  pushall		       ;save registers
;         call   sc_clr_scrn           ;clear screen
	  sub	 ax,ax		       ;normal cursor type
          mov    dx,1                  ;visible all the time
          call   sc_ctype              ;
          mov    dx,linecol            ;set position
          call   sc_con                ;turn cursor on
          mov    scinit,0              ;clear initialization flag
	  call	 sc_term	       ;terminate screen ops module
	  popall
termh1:   mov	 mscount,0	       ;make sure sound system re-initializes
	  ret			       ;return to caller
_termhost endp
	  page
;
;  NOTE - Play a single note
;
;	  (ax)			       frequency (0 = pause)
;	  (dx)			       duration (milliseconds)
;	  call	 note
;	  (ax,cx,dx)		       destroyed
;
note	  proc	 near		       ;
	  push	 di		       ;save regs
	  push	 si		       ;
;
;  See if need to calibrate counter
;
	  mov	 cx,mscount	       ;delay count
	  or	 cx,cx		       ;test for zero
	  jnz	 short note0	       ;jump if calibrated
	  call	 calib		       ;calibrate

note0:	  mov	 di,dx		       ;save duration in di
	  cmp	 ax,12h 	       ;treat frequency < 19 as pause
	  jb	 short note1	       ;
	  xchg	 si,ax		       ;copy frequency
	  mov	 al,10110110b	       ;set up timer counter 2
	  out	 43h,al 	       ;square wave, one byte binary input
;
;  Convert frequency to period and set timer
;
	  mov	 ax,34deh 	       ;set numerator for conversion
	  mov	 dx,0012h
	  div	 si		       ;divide by frequency
	  shl	 dx,1		       ;remainder * 2
	  cmp	 si,dx		       ;set CF if 2R > freq
	  adc	 ax,0		       ;round up
	  out	 42h,al 	       ;output lower byte of period
	  mov	 al,ah		       ;output upper byte of period
	  out	 42h,al 	       ;
;
;  Initiate generation of tone
;
	  in	 al,61h 	       ;get system port B
	  or	 al,3		       ;turn speaker and timer on
	  out	 61h,al
;
;  Loop to delay for required period
;
;   start 1 millisecond delay
;
note1:	  mov	 cx,mscount
	  mov	 ax,7654h
	  mul	 cx
	  div	 cx
	  mul	 cx
	  div	 cx
	  mul	 cx
	  div	 cx
	  jmp	 short note2
	  even
note2:	  dec	 cx
	  nop
	  nop
	  nop
	  jnz	 note2
	  dec	 di		       ;loop till delay complete
	  jnz	 note1
;
;  Done with tone generation
;
	  in	 al,61h 	       ;get system port B
	  and	 al,11111100b	       ;turn off timer and speaker
	  out	 61h,al 	       ;
	  pop	 si		       ;restore regs
	  pop	 di		       ;
	  ret			       ;return to caller
note	  endp			       ;
	  page

;   CALIB - calibrate counter for 1 millisecond delay loop
;
;   this is done by examining the BIOS-maintained timer that is
;   incremented 18.2 times per second by the hardware.
;
;   Input:  none
;   Output: 16-bit count stored in mscount.
;
;   To obtain a 1 ms delay, use the sequence:
;
;	mov	cx,mscount
;	mov	ax,7654h
;	mul	cx
;	div	cx
;	mul	cx
;	div	cx
;	mul	cx
;	div	cx
; loop:	dec	cx
;	nop
;	nop
;	nop
;	jnz	loop
;
calib	proc	near
	pushall
	xor	cx,cx
	mov	bx,timer
	push	ds
	mov	ax,mem64k
	mov	dx,-1
	mov	ds,ax
	mov	ax,[bx]			; load BIOS clock timer count
calib1:	cmp	ax,[bx]			; wait for it to change
	je	calib1
	mov	ax,[bx]
calibx:	inc	dx
	jmp	short calib2
	even
calib2:	inc	cx	    		; count
	jz	calibx
	cmp	ax,[bx]			; wait for another timer change
	je	calib2
	pop	ds
	xchg	ax,cx			; calibrate the count obtained
	mov	bx,1000/2
calib3:	or	dx,dx			; big result (fast machine?)
	je	calib4
	shr	dx,1			; scale:  DX:AX / 2, BX*2
	rcr	ax,1
	shl	bx,1
	jmp	calib3
calib4:	mul	bx
	mov	bx,54945/2		; 54.945 milliseconds per tick
	div	bx
	mov	mscount,ax		; count to use for 1 millisecond delay
	popall
	ret
calib	endp

;
;  PLAY - Play tune
;
;	  (es_si)		       string terminated by 0dh
;	  call	 play		       preserves all registers
;
play	  proc	 near		       ;
	  pushall		       ; save registers
;
;  Loop through characters in play string
;
play0:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;get next non-blank character
          cmp    al,' '                ;
	  je	 play0		       ;
	  cmp	 al,0dh 	       ;jump if more chars
	  jne	 short play0b	       ;
	  popall			       ; else restore registers
	  ret			       ; and return
play0b:	  call	 foldl		       ;fold lower case 	      -- 2.08 --
;
;  Deal with notes A-G
;
          cmp    al,'A'                ;Jump if not A-G
	  jb	 short play2	       ;
          cmp    al,'G'                ;
	  ja	 short play2	       ;
	  sub	 al,'A'		       ;else get note as zero origin letter
	  mov	 bl,al		       ;
	  xor    bh,bh
	  mov	 bl,notex[bx]	       ;convert to note number
;
;  In bx, we have a note number, adjust for sharps or flats
;
play0a:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;load next non-blank char
          cmp    al,' '                ;
	  je	 play0a 	       ;
	  inc	 bx		       ;bump index in case sharp
          cmp    al,'#'                ;jump if sharp
	  je	 short play1	       ;
          cmp    al,'+'                ;
	  je	 short play1	       ;
	  dec	 bx		       ;decrement index in case flat
	  dec	 bx		       ;
          cmp    al,'-'                ;jump if flat
	  je	 short play1	       ;
	  dec	 si		       ;else readjust string pointer
	  inc	 bx		       ;reset index for not sharp or flat
	  page
;
;  Here we have note index in bx, adjusted for sharps or flats
;
play1:	  shl	 bx,1
	  mov	 ax,freq[bx]	       ;get frequency from table
	  mov	 cl,6		       ;set octave shift
	  sub	 cl,octave	       ;
	  shr	 ax,cl		       ;adjust frequency for octave
	  jmp	 playn		       ;go play note
;
;  Deal with L (length of note)
;
play2:    cmp    al,'L'                ;jump if not length
	  jne	 short play3	       ;
	  mov	 dx,64		       ;max value is 64
	  call	 playi		       ;
	  or	 ax,ax		       ;convert 0 to 1
	  jnz	 short play2a	       ;
	  inc	 ax		       ;
play2a:	  mov	 nlength,al	       ;store new length
	  jmp	 play0		       ;loop
;
;  Deal with M (mode)
;
play3:    cmp    al,'M'                ;jump if not mode
	  jne	 short play6	       ;
play3a:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;load next non-blank char
          cmp    al,' '                ;
	  je	 play3a 	       ;
	  call	 foldl		       ;fold lower case 	      -- 2.08 --
          cmp    al,'L'                ;jump if ML
	  je	 short play5	       ;
          cmp    al,'N'                ;jump if MN
	  je	 short play5	       ;
          cmp    al,'S'                ;jump if MS
	  je	 short play5	       ;
;
;  Here if illegal mode
;
play4:	  popall		       ;restore registers
	  pop	 ax		       ;discard play return point
	  jmp	 exite		       ;error exit
;
;  Here if legal mode
;
play5:	  mov	 mode,al	       ;store new mode
	  jmp	 play0		       ;and loop
	  page
;
;  Deal with N (Note number)
;
play6:    cmp    al,'N'                ;jump if not note number
	  jne	 short play8	       ;
	  mov	 dx,84		       ;else set max note number is 84
	  call	 playi		       ;scan note number
	  or	 ax,ax		       ;jump if non-zero
	  jz	 playn		       ;else go play pause
;
;  Here with note number in ax
;
	  dec	 ax		       ;set zero origin
	  mov	 dl,12		       ;divide by 12
	  div	 dl		       ;octave to al, note to ah
	  mov	 cl,6		       ;get shift value in cl
	  sub	 cl,al		       ;
	  mov	 bl,ah		       ;note
	  xor	 bh,bh
	  shl	 bx,1
	  mov	 ax,freq+2[bx]         ;frequency for note
	  shr	 ax,cl		       ;do octave shift
	  jmp	 playn		       ;merge to play note
;
;  Deal with O (Octave set)
;
play8:    cmp    al,'O'                ;jump if not octave set
	  jne	 short play9	       ;
	  mov	 dx,6		       ;max octave is 6
	  call	 playi		       ;get octave
	  mov	 octave,al	       ;store new octave
	  jmp	 play0		       ;loop
;
;  Deal with P (pause)
;
play9:    cmp    al,'P'                ;jump if not P
	  jne	 short play10 	       ;
	  sub	 ax,ax	 	       ;set frequency = 0 (inaudible)
	  jmp	 short playn	       ;go play note
;
;  Deal with T (tempo)
;
play10:   cmp    al,'T'                ;jump if not T
	  jne	 short play12 	       ;
	  mov	 dx,255 	       ;max value is 255
	  call	 playi		       ;
	  cmp	 ax,32		       ;min value is 32
	  jae	 short play11 	       ;
	  popall 		       ;if error, restore registers
	  pop	 ax		       ;discard play return
	  jmp	 exite		       ;and exit
	  page
;
;  Here with OK tempo in AX
;
play11:   mov	 tempo,ax	       ;store tempo
	  jmp	 play0		       ;loop
;
;  Deal with < (decrease octave)
;
play12:   cmp    al,'<'                ;jump if not <
	  jne	 short play13 	       ;
	  mov	 al,octave	       ;get current octave
	  or	 al,al		       ;skip if already at 0
	  je	 short play12a	       ;
	  dec	 al		       ;else next lower octave
play12a:  mov	 octave,al	       ;store new octave
	  jmp	 play0		       ;loop
;
;  Deal with > (increase octave)
;
play13:   cmp    al,'>'                ;jump if not >
	  jne	 short play14 	       ;
	  mov	 al,octave	       ;get current octave
	  cmp	 al,6		       ;skip if already at 6
	  je	 short play13a	       ;
	  inc	 al		       ;else next higher octave
play13a:  mov	 octave,al	       ;store new octave
	  jmp	 play0		       ;loop
;
;  Here for illegal character in play string
;
play14:   popall		       ;restore registers
	  pop	 ax		       ;discard play return point
	  jmp	 exite		       ;signal error
;
;  Here to play a note, frequency is in AX
;
playn:	  xchg	 di,ax		       ;save frequency
playn0:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;get next non-blank character
          cmp    al,' '                ;
	  je	 playn0 	       ;
	  dec	 si		       ;and point back to it
          sub    al,'0'                ;test digit
	  xor	 ah,ah
	  cmp	 al,9		       ;
	  mov	 al,nlength	       ;jump if not wi default length
	  ja	 short playn1 	       ;
	  mov	 dx,64		       ;else set max time = 64
	  call	 playi		       ;read time value
	  or	 ax,ax	 	       ;reset 0 to 1
	  jnz	 short playn1 	       ;
	  inc	 ax		       ;
	  page
;
;  We have a length value in AX, frequency in DI
;
;  The length required in milliseconds is given by the formula:
;
;	      240000
;	 ----------------
;	 tempo	*  length
;
;  Use 240000*2, then round and divide by 2.
;
playn1:   xchg	 bx,ax		       ;save length value
	  mov	 ax,5300h	       ;dividend = 480000
	  mov	 dx,0007h
	  div	 tempo		       ;divide by tempo
	  sub	 dx,dx		       ;
	  div	 bx		       ;divide by length
	  inc	 ax		       ;round
	  shr	 ax,1		       ;divide by two
	  xchg	 dx,ax		       ;copy base value
;
;  Loop to process dots
;
playn2:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;get next non-blank character
          cmp    al,' '                ;
	  je	 playn2 	       ;
          cmp    al,'.'                ;test another dot
	  jne	 short playn3 	       ;jump if not
	  mov	 ax,dx
	  shr	 ax,1		       ;else halve remaining time
	  add	 dx,ax		       ;adjust by dot
	  jmp	 playn2 	       ;loop
;
;  Here with time in dx, adjusted for dots, freq in di, test mode
;
playn3:   dec	 si		       ;point back to non-dot terminator
	  mov	 al,mode	       ;test mode
	  mov	 cl,2		       ;jump with shift = 4 = 2**2 if S(taccato
          cmp    al,'S'                ;
	  je	 short playn4 	       ;
	  inc	 cl		       ;jump with shift = 8 = 2**3 if N(ormal
          cmp    al,'N'                ;
	  je	 short playn4 	       ;
;
;  Here for Legato
;
	  mov	 ax,di		       ;recall frequency
	  call	 note		       ;play note for entire period
	  jmp	 play0		       ;loop for next input
	  page
;
;  Here with time in dx and division shift factor in cl
;
playn4:   mov	 ax,dx		       ;copy length
	  shr	 ax,cl		       ;divide length using shift factor
	  sub	 dx,ax		       ;dx = time to play
	  xchg	 ax,di		       ;frequency to ax, save pause time
	  call	 note		       ;play note for required time
	  mov	 dx,di		       ;recall pause time
	  sub	 ax,ax		       ;play pause
	  call	 note		       ;
	  jmp	 play0		       ;loop for next input
;
;  Routine to scan integer [si], dx is max value permitted. On return, value
;  is in ax. If number is out of range, then control is passed to exite.
;
playi	  proc	 near
	  push	 di		       ;save registers
	  sub	 ax,ax		       ;clear msbs of new digits
	  sub	 di,di		       ;clear result
playi0:
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;get next non-blank character
          cmp    al,' '                ;
	  je	 playi0 	       ;
          sub    al,'0'                ;
	  cmp	 al,9		       ;
	  ja	 short playi2 	       ;error if no digits
;
;  Here with new digit in al
;
playi1:   shl	 di,1		       ;value * 2
	  add	 ax,di		       ;add 2 * value to new dig
	  shl	 di,1		       ;value * 4
	  shl	 di,1		       ;value * 8
	  add	 di,ax		       ;value * 10 + new digit
	  cmp	 ax,dx		       ;check too large
	  ja	 short playi2 	       ;jump if so
if LDATA
	  db	 26h		       ;ES override
endif
	  lodsb 		       ;load next char
          sub    al,'0'                ;loop if another digit
	  cmp	 al,9		       ;
	  jbe	 playi1 	       ;
;
;  Here at end of scanned integer
;
	  dec	 si		       ;back up pointer to terminator
	  xchg	 ax,di		       ;copy result to ax for exit
	  pop	 di		       ;restore registers
	  ret			       ;return to caller
	  page
;
;  Here if error in integer
;
playi2:   pop	 ax		       ;discard saved di
	  pop	 ax		       ;kill playi return point
	  popall		       ;discard play saved registers
	  pop	 ax		       ;kill play return point
	  jmp	 exite		       ;error exit
playi	  endp			       ;
play	  endp
;
;  Fold lower case to upper case.  Character in AL.
;
foldl	  proc	 near
	  cmp	 al,'a'
	  jb	 short foldl1
	  cmp	 al,'z'
	  ja	 short foldl1
	  sub	 al,'a'-'A'
foldl1:	  ret
foldl	  endp
;
;  End of host interface module
;
	Code_EndS@
	  end
