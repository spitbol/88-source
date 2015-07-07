	page	80,132
	title	KBOPS - Keyboard Routines
	name	KBOPS
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
;
;
;
;
;------------------------------------------------------------;
;							     ;
;	     D	E  W  A  R  S	 T  O  O  L  S		     ;
;							     ;
;	       TOOLS FOR USE WITH IBM PC/DOS		     ;
;							     ;
;			 K B O P S			     ;
;							     ;
;			 Written By			     ;
;							     ;
;		     Robert B. K. Dewar 		     ;
;							     ;
;		       Version 4.02			     ;
;							     ;
;	     Copyright (c) 1983, Realia Inc.		     ;
;							     ;
;------------------------------------------------------------;
;
;
;  REVISION HISTORY
;  ================
;
;  4.02  13 November 1990, 9:00 (MBE)
;	 Converted back to 8088 code.
;
;  4.01  26 November 1989, 08:00 (MBE)
;	 Converted to 80386 32-bit segments using Phar Lap DOS Extender.
;
;  3.01  05 August 1984, 09:45 (RBKD)
;	 Sound alarm by writing BEL in TTY mode
;
;  2.02  01 June 1983, 09:00 (RBKD)
;	 Remove routines for handling cursor type and display (moved to scops)
;
;  2.01  27 May 1983, 21:00 (RBKD)
;	 Upper case blank is now same as lower case blank
;
;  1.02  20 May 1983, 21:00 (RBKD)
;	 Add KB_CLEAR (clear keyboard buffer) routine
;	 Add KB_NLOFF (turn NumLock off) routine
;
;  1.01  14 April 1983, 07:00 (RBKD)
;	 Initial version
	 page
;
;  This module provides the following routines
;
	 public  kb_alarm    ;sound alarm
	 public  kb_clear    ;clear keyboard
	 public  kb_nloff    ;turn Num Lock off
	 public  kb_read     ;read key
         public  kb_state    ;get keyboard state
	 public  kb_test     ;test key available
;
;  These routine are assembled as NEAR procedures for use in a SMALL mode
;  program where the single public segment is called code and all segment
;  registers are set to point to this segment.
;
	Header@

	Data_Seg@
;
;  Data locations and definitions
;
romdata     equ   0040h      ;segment for ROM BIOS data
kb_flag     equ   0017h      ;offset to keyboard flag
caps_state  equ   40h	     ;flag for Caps Lock state on
num_state   equ   20h	     ;flag for Num Lock state on
;
;  Segment selectors hardwired by Phar Lap DOS Extender
;
mem1b	    equ   0034h	     ;first megabyte of conventional memory
scrnseg	    equ   001ch	     ;physical screen memory

;
;  Table used by kb_read to convert key scan code to ALT letter code
;
kreadat  db    132-'A'+'Q'   ;16 - ALT Q
         db    132-'A'+'W'   ;17 - ALT W
         db    132-'A'+'E'   ;18 - ALT E
         db    132-'A'+'R'   ;19 - ALT R
         db    132-'A'+'T'   ;20 - ALT T
         db    132-'A'+'Y'   ;21 - ALT Y
         db    132-'A'+'U'   ;22 - ALT U
         db    132-'A'+'I'   ;23 - ALT I
         db    132-'A'+'O'   ;24 - ALT O
         db    132-'A'+'P'   ;25 - ALT P
	 db    0	     ;26 - not used
	 db    0	     ;27 - not used
	 db    0	     ;28 - not used
	 db    0	     ;29 - not used
         db    132-'A'+'A'   ;30 - ALT A
         db    132-'A'+'S'   ;31 - ALT S
         db    132-'A'+'D'   ;32 - ALT D
         db    132-'A'+'F'   ;33 - ALT F
         db    132-'A'+'G'   ;34 - ALT G
         db    132-'A'+'H'   ;35 - ALT H
         db    132-'A'+'J'   ;36 - ALT J
         db    132-'A'+'K'   ;37 - ALT K
         db    132-'A'+'L'   ;38 - ALT L
	 db    0	     ;39 - not used
	 db    0	     ;40 - not used
	 db    0	     ;41 - not used
	 db    0	     ;42 - not used
	 db    0	     ;43 - not used
         db    132-'A'+'Z'   ;44 - ALT Z
         db    132-'A'+'X'   ;45 - ALT X
         db    132-'A'+'C'   ;46 - ALT C
         db    132-'A'+'V'   ;47 - ALT V
         db    132-'A'+'B'   ;48 - ALT B
         db    132-'A'+'N'   ;49 - ALT N
         db    132-'A'+'M'   ;50 - ALT M
	 page
;
;  Table used by kb_read for numeric pad keys, the first byte of each entry is
;  value to be used for the shifted (numeric pad) case, and the second byte
;  is the value to be used for the unshifted (base) case.
;
kreadnt  label word
	 db    166,193	     ;7 (Home)
	 db    167,194	     ;8 (cursor up)
	 db    168,195	     ;9 (PgUp)
	 db    170,196	     ;Minus
	 db    163,197	     ;4 (cursor left)
	 db    164,000	     ;5 (base case not used)
	 db    165,199	     ;6 (cursor right)
	 db    169,200	     ;Plus
	 db    160,201	     ;1 (End)
	 db    161,202	     ;2 (cursor down)
	 db    162,203	     ;3 (PgDn)
	 db    159,204	     ;0 (Ins)
	 db    158,205	     ;Period (Del)
	Data_EndS@

	 page
	Code_Seg@
;
;  KB_ALARM - Sound Audible Alarm
;
;	 call  kb_alarm      call to sound alarm
;
kb_alarm proc  near
	 push  ax	     ;save registers
	 push  bx
	 push  bp
	 mov   ax,0e07h      ;set function code, char = BEL
	 sub   bx,bx	     ;page zero
	 int   10h	     ;write BEL character
	 pop   bp
	 pop   bx
	 pop   ax
	 ret		     ;return to caller
kb_alarm endp
	 page
;
;  KB_CLEAR - Clear Keyboard Buffer
;
;	 call  kb_clear      call to clear keyboard buffer
;
kb_clear proc  near
	 push  ax	     ;save registers
;
;  Loop to test for and delete keys
;
clear1:  mov   ah,1	     ;test key available in buffer
	 int   16h
	 jz    short clear2  ;jump if none
	 mov   ah,0	     ;else read and discard code
	 int   16h
	 jmp   clear1	     ;and loop
;
;  Return with no key in buffer
;
clear2:  pop   ax	     ;restore registers
	 ret		     ;return to caller
kb_clear endp
	 page
;
;  KB_NLOFF - Turn Num Lock Mode Off
;
;	 call  kb_nloff      call to turn Num Lock mode off
;
kb_nloff proc  near
	 push  bx	       ;save registers
	 push  ds
	 mov   bx,mem1b	       ;map first meg of memory
	 mov   ds,bx
	 mov   bx,romdata*10h ;offset to rom bios data area
	 and   byte ptr [bx+kb_flag],0ffh-num_state ;turn Num Lock mode bit off
	 pop   ds	     ;restore registers
	 pop   bx
	 ret		     ;return to caller
kb_nloff endp
	 page
;
;  KB_READ - Read Next Key from Keyboard
;
;	 (dh,dl)	     line/char for cursor
;	 call  kb_read	     call to read key
;	 (al)		     key code
;	 (ah)		     destroyed
;        (dl)                ASCII code
;        (dh)                scan code
;        (bx,cx)             preserved
;
;  The key code is as follows:
;
;	 0	    Ctrl 2
;	 1-26	    Ctrl A-Z
;	 27-31	    Ctrl [ \ ] 6 -
;	 32-126     Standard ASCII codes (from main keyboard, not numeric pad)
;	 127	    Ctrl backspace
;	 128	    Shift backspace
;	 129	    Backspace
;	 130	    Tab
;	 131	    Back tab
;	 132-157    ALT A-Z
;	 158	    Numeric pad period
;	 159-168    Numeric pad 0-9
;	 169	    Numeric pad plus (shifted)
;	 170	    Numeric pad minus (shifted)
;	 171	    PrtSc (unshifted)
;	 172	    Enter (unshifted)
;	 173	    Shift enter
;	 174	    Ctrl enter
;	 175	    Esc (unshifted)
;	 176	    Shift Esc
;	 177	    Ctrl Esc
;	 179	    Ctrl space bar
;	 180	    ALT space bar	       206-215	  ALT F1-F10 (T2000!)
;	 181-190    F1-F10 (unshifted)	       216-225	  Ctrl F1-F10
;	 193	    Home		       226-235	  Shift F1-F10 (T2000!)
;	 194	    Cursor up		       236	  Ctrl PrtSc
;	 195	    PgUp		       237	  Ctrl cursor left
;	 196	    Numeric pad minus	       238	  Ctrl cursor right
;	 197	    Cursor left 	       239	  Ctrl End
;	 199	    Cursor right	       240	  Ctrl PgDn
;	 200	    Numeric pad plus	       241	  Ctrl Home
;	 201	    End 		       242-250	  ALT 1-9
;	 202	    Cursor down 	       251	  ALT 0
;	 203	    PgDn		       252	  ALT -
;	 204	    Ins 		       253	  ALT =
;	 205	    Del 		       254	  Ctrl PgUp
	 page
;
;  Entry point
;
kb_read  proc  near
	 push  bx	     ;save regs
	 push  cx
	 mov   ah,0	     ;read next ASCII key
	 int   16h
         mov   dx,ax         ;return scan code/ASCII code in dh/dl
;
;  Test cases of special function keys
;
	 cmp   al,20h	     ;treat space bar specially
	 je    short kread2
	 cmp   ah,14	     ;treat backspace specially
	 je    short kread3
	 cmp   ah,15	     ;treat tab specially
	 je    short kread4
	 cmp   ah,55	     ;treat print screen specially
	 je    short kread5
	 cmp   ah,28	     ;treat enter key specially
	 je    short kread6
	 cmp   ah,1	     ;treat ESC specially
	 je    short kread7
	 cmp   al,0	     ;jump if extended code
	 je    short kread9
	 cmp   ah,71	     ;treat numeric pad specially
	 jae   short kread8
	 jmp   kread13	     ;else ASCII code is in al, return
;
;  Space bar
;
kread2:  mov   bx,256*180+179 ;bh = code for ALT space, bl = code for Ctrl space
         mov   ah,' '         ;ah = code for shift space
	 jmp   short kread12  ;merge to select proper code based on shift keys
;
;  Backspace
;
kread3:  mov   bl,127	      ;bl = code for ctrl backspace
	 mov   ax,256*128+129 ;ah = code for shift backspace, al = code for bs
	 jmp   short kread12  ;merge to select proper code based on shift keys
	 page
;
;  Tab
;
kread4:  mov   ax,256*131+130 ;ah = code for shift tab, al = code for tab
         jmp   short kread12  ;merge to select proper code based on shift keys
	 page
;
;  PrtSc
;
kread5:  mov   bl,236	      ;bl = code for ctrl prtsc
	 mov   al,171	      ;al = code for unshifted prtsc
	 jmp   short kread12  ;merge to select proper code based on shift keys
;
;  Enter
;
kread6:  mov   bl,174	      ;bl = code for ctrl enter
	 mov   ax,256*173+172 ;ah = code for shift enter, al = code for enter
	 jmp   short kread12  ;merge to select proper code based on shift keys
;
;  Esc
;
kread7:  mov   bl,177	      ;bl = code for ctrl esc
	 mov   ax,256*176+175 ;ah = code for shift esc, al = code for esc
	 jmp   short kread12  ;merge to select proper code based on shift keys
;
;  Numeric pad
;
kread8:  mov   al,ah	     ;copy key #
	 sub   al,71	     ;get key as word index value in bx
	 xor   ah,ah
	 xchg  ax,bx
	 shl   bx,1
	 mov   bx,kreadnt[bx];get entry from numeric pad table
	 mov   ah,2	     ;test key statuses
	 int   16h
	 and   al,23h	     ;isolate numlock (20h) and left/right shift (03h)
	 mov   ah,al	     ;combine shift key indications
	 shr   al,1
	 or    al,ah	     ;10h is numlock, 01h is shift key pressed
	 and   al,11h	     ;test settings
	 jpo   short $+4     ;skip if numlock (shift or numlock but not both)
	 mov   bl,bh	     ;else adjust for both or neither set (numeric pad)
	 xchg  ax,bx	     ;result to al
         xor   dl,dl         ;smash ASCII code in dl
	 jmp   short kread13 ;return
	 page
;
;  Here for function key (al = 0), first deal with null
;
kread9:  mov   al,ah	     ;get pseudo-scan code in al and ax
	 xor   ah,ah
	 cmp   al,3	     ;null?
	 jne   short kread10 ;jump if not
	 sub   al,al	     ;else set zero as return value
	 jmp   short kread13 ;and return
;
;  Deal with ALT letters
;
kread10: cmp   al,50	     ;jump if not ALT letter
	 ja    short kread11
	 sub   al,16	     ;else get key as byte offset for index
	 mov   bl,al
	 xor   bh,bh
	 mov   al,kreadat[bx];load table entry
	 jmp   short kread13 ;and exit
;
;  For all other function keys, we just add a bias to convert the psuedo-scan
;  code to the code which is returned by the keyin procedure.
;
kread11: xchg  ax,cx         ;al,ah codes saved in cx
         mov   ah,2          ;get shift key status bits
	 int   16h
         xchg  ax,cx         ;shift key status to cl, key code to al
         and   cl,0Fh        ;test for shift, control, or alt key
         jnz   short kread14 ;skip F11,F12 test if any of the above
         cmp   al,87         ;test for F11 (only valid in unshifted case)
	 jb    short kread14
	 cmp   al,88	     ;or F12
	 ja    short kread14
	 add   al,69-87	     ;map unshifted F11/F12 to just above F10
kread14: add   al,181-59     ;adjust code for keyin return
         jmp   short kread13 ;return to caller
;
;  Come here to deal with shift states for special keys
;
;    (BH)   Code to use if ALT key is pressed
;    (BL)   Code to use if ALT key off, CTRL key pressed
;    (AH)   Code to use if ALT, CTRL keys off, SHIFT key pressed
;    (AL)   Code to use if ALT, CTRL, SHIFT keys off
;
kread12: xchg  ax,cx         ;al,ah codes saved in cx
	 mov   ah,2	     ;get shift key status bits
	 int   16h
	 test  al,03h	     ;set proper code if shift key pressed
	 jz    short $+4
	 mov   cl,ch
	 test  al,04h	     ;set proper code if CTRL key pressed
	 jz    short $+4
	 mov   cl,bl
	 test  al,08h	     ;set proper code if ALT key pressed
	 jz    short $+4
	 mov   cl,bh
	 mov   al,cl
         cmp   al,' '
         je    short kread13
         xor   dl,dl         ;smash ASCII code
;
;  Common return point
;
kread13: xor   ah,ah	     ;return clean ax
	 pop   cx	     ;restore registers
	 pop   bx
	 ret		     ;return to caller
;
kb_read  endp		     ;end of keyin procedure
	 page
;
;  KB_STATE - Get current keyboard state
;
;  This subroutine returns the keyboard state associated with the last read.
;
;        call  kb_state      Call to get the last keyboard state
;        (ax)                Key state bits
;                               shift           01h
;                               control         04h
;                               alt             08h
;                               scroll lock     10h
;                               num lock        20h
;                               caps lock       40h
;                               insert toggled  80h
;
kb_state proc  near
         mov   ah,2          ;test key statuses
	 int   16h
         xor   ah,ah
         ret
kb_state endp
         page
;
;  KB_TEST - Test Keyboard Data Available
;
;  This subroutine tests to see whether keyboard data is available, but does
;  not actually read any keyboard data (to read the data, use KB_READ).
;
;	 call  kb_test	     Call to test for data abailable
;	 (zf)		     Set if no data (Z), clear if data (NZ)
;
kb_test  proc  near
	 push  ax	     ;save registers
	 mov   ah,1	     ;any key available?
	 int   16h	     ;(sets ZF as required by call)
	 pop   ax	     ;restore registers
	 ret		     ;return to caller
kb_test  endp
;
	Code_EndS@
	 end
