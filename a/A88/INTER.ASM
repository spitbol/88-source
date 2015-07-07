	title	INTER
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


	name	INTER
globals	equ	1			;ASM globals defined here
	include systype.ah
	include	rules.asi
	include	osint.inc

;
;       File: INTER.ASM           Version: 1.32
;	---------------------------------------
;
;	This file contains the assembly language routines that interface
;	the Macro SPITBOL compiler written in 8088 assembly language to its
;	operating system interface functions written in C.
;
;	Contents:
;
;	o Overview
;	o Global variables accessed by OSINT functions
;	o Interface routines between compiler and OSINT functions
;	o C callable function startup
;	o C callable function get_fp
;	o C callable function restart
;	o C callable function makeexec
;	o Routines for Minimal opcodes CHK and CVD
;	o Math functions for integer multiply, divide, and remainder
;	o Math functions for real operation
;
;-----------
;
;	Overview
;
;	The Macro SPITBOL compiler relies on a set of operating system
;	interface functions to provide all interaction with the host
;	operating system.  These functions are referred to as OSINT
;	functions.  A typical call to one of these OSINT functions takes
;	the following form in the 80386 version of the compiler:
;
;		...code to put arguments in registers...
;		call	SYSXX		; call osint function
;	   	dw	EXIT_1		; address of exit point 1
;		dw	EXIT_2		; address of exit point 2
;		...	...		; ...
;		dw	EXIT_n		; address of exit point n
;		...instruction following call...
;
;	The OSINT function 'SYSXX' can then return in one of n+1 ways:
;	to one of the n exit points or to the instruction following the
;	last exit.  This is not really very complicated - the call places
;	the return address on the stack, so all the interface function has
;	to do is add the appropriate offset to the return address and then
;	pick up the exit address and jump to it OR do a normal return via
;	an ret instruction.
;
;	Unfortunately, a C function cannot handle this scheme.  So, an
;	intermediary set of routines have been established to allow the
;	interfacing of C functions.  The mechanism is as follows:
;
;	(1) The compiler calls OSINT functions as described above.
;
;	(2) A set of assembly language interface routines is established,
;	    one per OSINT function, named accordingly.  Each interface
;	    routine ...
;
;	    (a) saves all compiler registers in global variables
;	        accessible by C functions
;	    (b) calls the OSINT function written in C
;	    (c) restores all compiler registers from the global variables
;	    (d) inspects the OSINT function's return value to determine
;	        which of the n+1 returns should be taken and does so
;
;	(3) A set of C language OSINT functions is established, one per
;	    OSINT function, named differently than the interface routines.
;	    Each OSINT function can access compiler registers via global
;	    variables.  NO arguments are passed via the call.
;
;	    When an OSINT function returns, it must return a value indicating
;	    which of the n+1 exits should be taken.  These return values are
;	    defined in header file 'inter.h'.
;
;	Note:  in the actual implementation below, the saving and restoring
;	of registers is actually done in one common routine accessed by all
;	interface routines.
;
;	Other notes:
;
;	Some C ompilers transform "internal" global names to
;	"external" global names by adding a leading underscore at the front
;	of the internal name.  Thus, the function name 'osopen' becomes
;	'_osopen'.  However, not all C compilers follow this convention.
;
;	Acknowledgement:
;
;	This interfacing scheme is based on an idea put forth by Andy Koenig.
;
;	V1.0	10/21/86 Robert E. Goldberg, DISC.  VAX version
;	V1.01	10/23/86 Mark B. Emmer, Catspaw.  AT&T 7300 version
;	V1.02	01/07/87   "  "    "      "   Generic 68000 version
;	V1.03	01/15/87 Revised to match new VAXINTER, V1.03.  MBE
;	V1.04	01/23/87 Adjust pointers within stack during restart.
;			 Add function makeexec to write a.out file when
;			 requested with -w command option.  MBE
;	V1.05	02/04/87 Adjustments to overcome shortcomings in Apollo
;			 assembler.  MBE.
;	V1.06	02/14/87 Add  reg_xx usages.  Put Apollo restart code
;			 in.  MBE
;	V1.07	02/21/87 Corrections for Apollo.  MBE
;	V1.08	05/18/87 Corrections for Apollo.  MBE
;	V1.10	06/03/87 Changed compiler to use A6 as pointer to constant
;			 and working storage, rather than A5.  A5 must be
;			 free in compiler for jump table addressing on
;			 Apple Macintosh versions.
;	V1.11	10/11/87 Added conditional 68020 opcodes for multiply,
;			 divide and remainder.
;			 Added conditional for UNISYS.
;			 Corrected bug in makeexec added when moved DB
;			 from A5 to A6.
;	V1.12	11/16/87 Versions for Amiga and WICAT
;	V1.13	01/15/88 Version for HP.  Initialize 68881 if present
;	V1.14	02/27/88 Version for Definicon.
;	V1.15	03/03/88 Added SYSGC call.
;	V1.16	05/09/88 Split off 80386 version
;	V1.17   09/12/89 Add support for keyboard polling and keyboard editing.
;			 Makeexec accepts file name scblk.
;	V1.18	10/27/89 Remove id2, add dosvers()
;	V1.19	12/16/89 Add pushregs, popregs
;	V1.20	07/21/90 Modify pushregs/popregs to save/restore BP and
;			 load it with reg_cp, so that any MINIMAL routine
;			 call between adjusts CP properly.
;			 Change ccaller to clear BP, so CP (which points
;			 within a SPITBOL block) isn't placed on stack when
;			 C routine pushes BP.  This is not a legimate
;			 collectable value, and will crash if a garbage
;			 collect occurs (as within callef). 2.43 I/O.
;	V1.21	09/09/90 Modify pushregs to return zero in SI (XL), so
;			 that a safe, collectable value is there for any
;			 subsequent call to a memory allocation routine.
;	V1.22	10/31/90 Remove SYSGC call.
;	V1.23	11/08/90 Conversion to 8088 version.
;	V1.24	11/17/90 Modify push/pop regs to put reg_xl on stack,
;			 other regs into a temp area.
;	V1.25	12/04/90 Mark SYSEX and SYSLD for 3 exits each.
;	V1.26	01/21/91 Update version number to 2.46.
;	V1.27	02/16/91 Add offset word to inpbuf, ttybuf.  Add SYSBS.
;	V1.28	03/13/92 Fix pushregs and popregs to not save and
;			 restore REG_CP.  It is necessary for GBCOL
;			 to be able to modify REG_CP and have its
;			 change stick.  We also go to a dual stack
;			 approach like the Macintosh, with the compiler's
;			 stack and the OSINT stack keep seperate.  This
;			 is necessary because if an OSINT return calls into
;			 Minimal and triggers a garbage collect, anything on
;			 the stack from OSINT could be fatal to GBCOL.
;	V1.29	09/09/92 Borland C 8087 emulation library requires that
;			 the stack segment be the same as the startup
;			 stack segment when executed emulated floating
;			 point opcodes.  Instead of doing LDR, ADR, etc.
;			 in-line, we have to invoke calls to LDR_, ADR_,
;			 etc., and have them switch to the OSINT stack
;			 for the duration of the floating point operation.
;	V1.30	10/11/95 Redid overflow detection logic to check for infinity
;			 in RA, relying on masked exceptions to produce infinity
;			 from basic math ops.
;       V1.31   10/08/96 Increased number of exits for SYSFC from 1 to 2.
;       V1.32   03/31/97 Added sav_compsp for use by push/popregs.  Call to
;                        SYSLD would save esp in compsp (at ccaller), then
;                        loadef calls ALOST which may call GBCOL, which calls
;                        SYSGC and clobbers compsp with this nested cccaller call.
;                        pushregs saves compsp in sav_compsp, and popregs
;                        restores it.
;

;
;-----------
;
;	Global Variables and routines
;
	Header@

MINCODE	segment	word public 'CODE'
	ext	callmin,far
MINCODE	ends

	Code_Seg@
	ext	f_atn,far
	ext	f_cos,far
	ext	f_etx,far
	ext	f_lnf,far
	ext	f_sin,far
	ext	f_tan,far
if ENGINE eq 0
	ext	swcoup,near
endif
ifndef __WINDOWS__
dgroup_ds  dw	DGROUP
endif
osisp	dw	?		; 1.28 OSInt's stack pointer
osiss	dw	?
compsp	dw	0		; 1.28 compiler's stack pointer
compss	dw	0
sav_compsp dw   0               ; save compsp here
        Code_EndS@

	MinData_Seg@
if USE_MALLOC
	dw	2 dup(?)		; For malloc header words
					; Not needed for Windows, but leave in for now
endif
	pubdef	minbdata,label,word	; FIRST USEABLE WORD IN MINIMAL DATA SEGMENT
	extrn	REG_CP:word
if IA32
ten	dd	10		; constant 10
else
ten	dw	10		; constant 10
endif
cw	dw	?		; floating point control word
cwtemp	dw	?		;  and backing temp
if IA32
	public	REG_IA
REG_IA	dd	?		; Low-order end used by Minimal code
else
REG_IA	dw	?		; temp for ITR_
endif
REG_RA	dq	?		; temp for float routines
ifdef __WINDOWS__
dgroup_ds  dw	DGROUP
endif
	MinData_EndS@

	Data_Seg@
	ext	stacksiz,word
	ext	lmodstk,word
	dPtrExt@ outptr,__C__
if direct eq 0
	ext	valtab,word
endif



include	extrn88.inc


; Words saved during exit(-3)
;
	align	msize
	pubdef	reg_block,label,word
	pubdef	reg_wa,dw,?	; Register WA (CX)
	pubdef	reg_wb,dw,?	; Register WB (BX)
if IA32
	pubdef	reg_ia,label,dword
	dw	?		; low order half of reg_ia
else
	pubdef	reg_ia,label,word
endif
	pubdef	reg_wc,dw,?	; Register WC & IA (DX)
	pubdef	reg_xr,dw,?	; Register XR (DI)
	pubdef	reg_xl,dw,?	; Register XL (SI)
	pubdef	reg_cp,dw,?	; Register CP
	pubdef	reg_ra,dq,0	; Register RA

; These locations save information needed for restart after EXIT()
; They are only set by SYSXI and SYSBX calls, since these are the only
; places where load modules may be created.
;
	align	msize
	pubdef	reg_pc,dw,?	; Return PC from ccaller (offset)
	dw	?		; Return PC from ccaller (segment)
reg_pp	dw	?		; Number of bytes of PPMs
	pubdef	reg_xs,dw,?	; Minimal stack pointer
reg_bp	dw	?		; Hold BP register
;
r_size	equ	$-reg_block
	pubdef	reg_size,dw,r_size
;
; end of words saved during exit(-3)
;

	pubdef	inf,dd,0
	dd	7ff00000h	; double precision infinity


sav_block db	r_size dup (?)	; Save minimal registers during push/pop reg

	pubdef	mindata_ds,dw,MINDATA

;	Variables used to assist in control-C processing.
;
	pubdef	brkpnd,db,0	; incremented when control-C hit
brkste	db	0	 	; old BREAK state
i23_adr	dd	0		; int 23 address

;
;	For Borland initialization, here's the max number of files allowed:
;
	pubdef	_nfile,dw,255	; allow user to open more than 20 files

;	FAR vectors to trig functions in C code
;
	def	j_atn,dd,f_atn
	def	j_cos,dd,f_cos
	def	j_etx,dd,f_etx
	def	j_lnf,dd,f_lnf
	def	j_sin,dd,f_sin
	def	j_tan,dd,f_tan

	Data_EndS@

	MinData_Seg@
	pubdef	_ID1,dw,0
        dw      13
        db      "(ver 1.30.20)"
        db      0
;
	pubdef	_ID2BLK,dw,0
	dw	ID2BLK_LENGTH
	db	ID2BLK_LENGTH dup (?)

	pubdef	_TICBLK,dw,0
	dw	0

	pubdef	_TSCBLK,dw,TSCBLK_LENGTH
	dw	0
	db	TSCBLK_LENGTH dup (?)

;
;	Standard input buffer block.
;
	pubdef	_INPBUF,dw,0
	dw	0		; block length
	dw	IRECSIZ		; buffer size
	dw	0		; remaining chars to read
	dw	0		; offset to next character to read
	dd	0		; file position of buffer
	dd	0		; physical position in file
	db	IRECSIZ dup (?)	; buffer
;
	pubdef	_TTYBUF,dw,0
	dw	0		; block length
	dw	260		; buffer size  (260 OK in MS-DOS with cinread())
	dw	0		; remaining chars to read
	dw	0		; offset to next char to read
	dd	0		; file position of buffer
	dd	0		; physical position in file
	db	260 dup (?)	; buffer

	MinData_EndS@

	Code_Seg@
;
;-----------
;
;	Save and restore MINIMAL interface registers.
;	Used by any routine that needs to call back into the MINIMAL
;	code in such a way that the MINIMAL code might trigger another
;	SYSxx call before returning.
;
;	Note 1:  pushregs returns a collectable value in XL, safe
;	for subsequent call to memory allocation routine.
;
;	Note 2:  these are not recursive routines.  Only reg_xl is
;	saved on the stack, where it is accessible to the garbage
;	collector.  Other registers are just moved to a temp area.
;
;	Note 3:  popregs does not restore REG_CP, because it may have
;	been modified by the Minimal routine called between pushregs
;	and popregs as a result of a garbage collection.  Calling of
;	another SYSxx routine in between is not a problem, because
;	CP will have been preserved by Minimal.
;
;	Note 4:  if there isn't a compiler stack yet, we don't bother
;	saving XL.  This only happens in call of nextef from sysxi when
;	reloading a save file.
;
;
	assume	DS:DGROUP
pushregs proc	near
	publab	pushregs
	push	es
	push	cx
	push	si
	push	di
	mov	reg_bp,bp			;1.20 save BP
	lea	si,reg_block
	lea	di,sav_block
	mov	cx,r_size/msize
	cld
	push	ds
	pop	es
   rep	movsw					;save in temp area
   	mov	es,compss
;	mov	di,compss			;compiler's stack
;	or	di,di				;1.39 is there one?
;	jne	short push0			; 1.39 jump if there is one
;	int	3
;push0:	mov	es,di
	mov	di,compsp
	sub	di,msize			;push onto compiler's stack
        mov     compsp,di                       ;smashed if call OSINT again (SYSGC)
        mov     sav_compsp,di                   ;used by popregs
	mov	si,reg_xl			;collectable XL
	mov	es:[di],si
push1:	pop	di
	pop	si
	pop	cx
	pop	es
	retc	0
pushregs endp

popregs	proc	near
	publab	popregs
	push	es
	push	cx
	push	si				;save registers
	push	di
	mov	ax,reg_cp
	cld
	lea	si,sav_block			;unload saved registers
	lea	di,reg_block
	mov	cx,r_size/msize
	push	ds
	pop	es
   rep	movsw					;restore from temp area
	mov	reg_cp,ax
	mov	es,compss
;	mov	di,compss			;compiler's stack
;	or	di,di				;1.39 is there one?
;	jne	short pop0			;1.39 jump if none yet
;	int	3
;pop0:	mov	es,di
        mov     di,sav_compsp                   ;saved compiler's stack
	mov	si,es:[di]			;retrieve collectable XL
	mov	reg_xl,si			;update XL
	add	di,msize
	mov	compsp,di			;update compiler's sp

pop1:	pop	di
	pop	si
	pop	cx
	pop	es
	mov	bp,reg_bp			;1.20 load BP
	retc	0
popregs	endp

;
;-----------
;
;	Interface routines
;
;	Each interface routine takes the following form:
;
;		SYSXX	call	ccaller		; call common interface
;			dw	zysxx		; address of C OSINT function
;			db	n		; offset to instruction after
;						;   last procedure exit
;
;	In an effort to achieve portability of C OSINT functions, we
;	do not take take advantage of any "internal" to "external"
;	transformation of names by C compilers.  So, a C OSINT function
;	representing sysxx is named _zysxx.  This renaming should satisfy
;	all C compilers.
;
;	IMPORTANT  ONE interface routine, SYSFC, is passed arguments on
;	the stack.  These items are removed from the stack before calling
;	ccaller, as they are not needed by this implementation.
;
;
;-----------
;
;	CCALLER is called by the OS interface routines to call the
;	real C OS interface function.
;
;	General calling sequence is
;
;		call	ccaller
;		dw	address_of_C_function
;		db	2*number_of_exit_points
;
;	Control IS NEVER returned to a interface routine.  Instead, control
;	is returned to the compiler (THE caller of the interface routine).
;
;	The C function that is called MUST ALWAYS return an integer
;	indicating the procedure exit to take or that a normal return
;	is to be performed.
;
;		C function      Interpretation
;		return value
;		------------	-------------------------------------------
;		     <0		Do normal return to instruction past
;				last procedure exit (distance passed
;				in by dummy routine and saved on stack)
;		      0		Take procedure exit 1
;		      4		Take procedure exit 2
;		      8		Take procedure exit 3
;		     ...	...
;
ccaller	proc	near

;	(1) Save registers in global variables
;
	assume	DS:MINDATA
	mov	bp,REG_CP		; fetch CP from Minimal space
if IA32
	push	word ptr REG_IA		; fetch IA(low) from Minimal space
endif
	mov	ds,dgroup_ds		; switch to C data space
	assume	DS:DGROUP
	mov	reg_xs,ax		; SP for a few SYSxx calls
	mov	ax,ds
	mov	es,ax
	mov	reg_cp,bp		; save CP in C data space
if IA32
	pop	word ptr reg_ia		; save IA(low) in C data space
endif

	mov	reg_wa,cx		; save registers
	mov	reg_wb,bx
	mov	reg_wc,dx		; also reg_ia (msh if IA32)
	mov	reg_xr,di
	mov	reg_xl,si

;	(2) Get pointer to arg list
;
	pop	si			; point to arg list
;
;
;	(3) Fetch address of C function, fetch offset to 1st instruction
;	    past last procedure exit, and call C function.
;
 	lods	word ptr ccaller	; point to C function entry point

	xor	bh,bh
	mov	bl,cs:[si]		; save normal exit adjustment
	mov	reg_pp,bx		; in memory
	pop	reg_pc			; save return PC past "CALL SYSXX"
	pop	reg_pc+2

;
;	(3a) Save compiler stack and switch to OSINT stack
;
	mov	compsp,sp
;	mov	compss,ss
	loadstk	osiss,osisp		; 1.28 switch to OSINT stack
;
;	(3b) Make call to OSINT
;
	even
	nop				; call must produce odd return address
	call	ax			; call C interface function
;
;	(4) Restore registers after C function returns.
;

cc1:	mov	osisp,sp		; 1.28 save OSINT stack pointer
	mov	osiss,ss
	loadstk	compss,compsp		; 1.28 switch to compiler stack
	cld				; restore registers & direction flag
	mov	cx,reg_wa		; restore registers
	mov	bx,reg_wb
	mov	dx,reg_wc		; also reg_ia (msh if IA32)
	mov	di,reg_xr
	mov	si,reg_xl
;
;	(5) Based on returned value from C function (in AX) either do a normal
;	    return or take a procedure exit.
;
	or	ax,ax			; test if normal return ...
	jns	short erexit		; j. if >= 0 (take numbered exit)
	mov	ax,reg_pp
	add	ax,reg_pc		; point to instruction following exits
	push	reg_pc+2		; push return segment
	push 	ax			; push return offset to bypass PPM exits

cc2:	mov	bp,reg_cp		; fetch C version of reg_CP
if IA32
	push	word ptr reg_ia		; fetch C version of reg_ia(low)
endif
	mov	ds,_mindata_ds		; load Minimal data space
	assume	DS:MINDATA
	mov	REG_CP,bp		; copy reg_cp to Minimal space
if IA32
	pop	word ptr REG_IA		; copy reg_ia(low) to Minimal space
endif
	mov	bp,ds
	mov	es,bp
	db	0CBh			; far return
;					; else (take procedure exit n)
	assume	DS:DGROUP
erexit:	shr	ax,1			; divide by 2
	add	ax,reg_pc		;   get to address of exit offset
	mov	bp,ax
	mov	es,reg_pc+2		; V37 code segment
	push	es
	push	es:[bp]			; push address of error routine
	xor	ax,ax			; in case branch to error cascade
	jmp	cc2			; copy reg_cp and return

ccaller	endp
;
;---------------
;
;	Individual OSINT routine entry points
;
	assume	DS:MINDATA
	public	SYSAX
	ext	zysax,near
SYSAX	label   far
	call	ccaller
	dw	offset _TEXT:zysax
	db	0
;
	public	SYSBS
	ext	zysbs,near
SYSBS	label	far
	call	ccaller
	dw	offset _TEXT:zysbs
	db	3*2
;
	public	SYSBX
	ext	zysbx,near
SYSBX	label	far
	mov	ax,sp
	inc	ax			;don't record segment
	inc	ax
	call	ccaller
	dw	offset _TEXT:zysbx
	db	0
;
	public	SYSCR
	ext	zyscr,near
SYSCR	label	far
	call	ccaller
	dw	offset _TEXT:zyscr
	db	0
;
	public	SYSDC
	ext	zysdc,near
SYSDC	label	far
	call	ccaller
	dw	offset _TEXT:zysdc
	db	0
;
	public	SYSDM
	ext	zysdm,near
SYSDM	label	far
	call	ccaller
	dw	offset _TEXT:zysdm
	db	0
;
	public	SYSDT
	ext	zysdt,near
SYSDT	label	far
	call	ccaller
	dw	offset _TEXT:zysdt
	db	0
;
	public	SYSEA
	ext	zysea,near
SYSEA	label	far
	call	ccaller
	dw	offset _TEXT:zysea
	db	1*2
;
	public	SYSEF
	ext	zysef,near
SYSEF	label	far
	call	ccaller
	dw	offset _TEXT:zysef
	db	3*2
;
	public	SYSEJ
	ext	zysej,near
SYSEJ	label	far
	call	ccaller
	dw	offset _TEXT:zysej
	db	0
;
	public	SYSEM
	ext	zysem,near
SYSEM	label	far
	call	ccaller
	dw	offset _TEXT:zysem
	db	0
;
	public	SYSEN
	ext	zysen,near
SYSEN	label	far
	call	ccaller
	dw	offset _TEXT:zysen
	db	3*2
;
	public	SYSEP
	ext	zysep,near
SYSEP	label	far
	call	ccaller
	dw	offset _TEXT:zysep
	db	0
;
	public	SYSEX
	ext	zysex,near
SYSEX	label	far
	mov	ax,sp
	inc	ax			;don't record segment
	inc	ax
	call	ccaller
	dw	offset _TEXT:zysex
	db	3*2
;
	public	SYSFC
	ext	zysfc,near
SYSFC	label	far
	pop	ax			; save return offset
	pop	bp			; save return segment
sysfc1:	add	sp,msize		; <<<<remove stacked SCBLKs>>>>
	dec	dx
	jne	sysfc1
	push	bp
	push	ax
	call	ccaller
	dw	offset _TEXT:zysfc
        db      2*2
;
	public	SYSGC
	ext	zysgc,near
SYSGC	label	far
	call	ccaller
	dw	offset _TEXT:zysgc
	db	0
;
	public	SYSHS
	ext	zyshs,near
SYSHS	label	far
	mov	ax,sp
	inc	ax			;don't record segment
	inc	ax
	call	ccaller
	dw	offset _TEXT:zyshs
	db	8*2
;
	public	SYSID
	ext	zysid,near
SYSID	label	far
	call	ccaller
	dw	offset _TEXT:zysid
	db	0
;
	public	SYSIF
	ext	zysif,near
SYSIF	label	far
	call	ccaller
	dw	offset _TEXT:zysif
	db	1*2
;
	public	SYSIL
	ext	zysil,near
SYSIL	label	far
	call	ccaller
	dw	offset _TEXT:zysil
	db	0
;
	public	SYSIN
	ext	zysin,near
SYSIN	label	far
	call	ccaller
	dw	offset _TEXT:zysin
	db	3*2
;
	public	SYSIO
	ext	zysio,near
SYSIO	label	far
	call	ccaller
	dw	offset _TEXT:zysio
	db	2*2
;
	public	SYSLD
	ext	zysld,near
SYSLD	label	far
	call	ccaller
	dw	offset _TEXT:zysld
	db	3*2
;
	public	SYSMM
	ext	zysmm,near
SYSMM	label	far
	call	ccaller
	dw	offset _TEXT:zysmm
	db	0
;
	public	SYSMX
	ext	zysmx,near
SYSMX	label	far
	call	ccaller
	dw	offset _TEXT:zysmx
	db	0
;
	public	SYSOU
	ext	zysou,near
SYSOU	label	far
	call	ccaller
	dw	offset _TEXT:zysou
	db	2*2
;
	public	SYSPI
	ext	zyspi,near
SYSPI	label	far
	call	ccaller
	dw	offset _TEXT:zyspi
	db	1*2
;
	public	SYSPL
	ext	zyspl,near
SYSPL	label	far
	call	ccaller
	dw	offset _TEXT:zyspl
	db	3*2
;
	public	SYSPP
	ext	zyspp,near
SYSPP	label	far
	call	ccaller
	dw	offset _TEXT:zyspp
	db	0
;
	public	SYSPR
	ext	zyspr,near
SYSPR	label	far
	call	ccaller
	dw	offset _TEXT:zyspr
	db	1*2
;
	public	SYSRD
	ext	zysrd,near
SYSRD	label	far
	call	ccaller
	dw	offset _TEXT:zysrd
	db	1*2
;
	public	SYSRI
	ext	zysri,near
SYSRI	label	far
	call	ccaller
	dw	offset _TEXT:zysri
	db	1*2
;
	public	SYSRW
	ext	zysrw,near
SYSRW	label	far
	call	ccaller
	dw	offset _TEXT:zysrw
	db	3*2
;
	public	SYSST
	ext	zysst,near
SYSST	label	far
	call	ccaller
	dw	offset _TEXT:zysst
	db	5*2
;
	public	SYSTM
	ext	zystm,near
SYSTM	label	far
	call	ccaller
systm_p	dw	offset _TEXT:zystm
	db	0
;
	public	SYSTT
	ext	zystt,near
SYSTT	label	far
	call	ccaller
	dw	offset _TEXT:zystt
	db	0
;
	public	SYSUL
	ext	zysul,near
SYSUL	label	far
	call	ccaller
	dw	offset _TEXT:zysul
	db	0
;
	public	SYSXI
	ext	zysxi,near
SYSXI	label	far
	mov	ax,sp
	inc	ax			;don't record segment
	inc	ax
	call	ccaller
sysxi_p	dw	offset _TEXT:zysxi
	db	2*2

;
;-----------
;
;	startup( char *dummy, char *stackbase) - startup compiler
;
;	An OSINT C function calls startup to transfer control
;	to the compiler.
;
;	(XR) = basemem
;	(XL) = topmem - sizeof(WORD)
;
	assume	DS:DGROUP

startup proc	near
	publab	startup

	pop	ax			; discard return
if LPROG
	pop	ax
endif
	pop	ax			; discard dummy
if LDATA
	pop	ax
endif
	pop	ax			; get lowest legal stack value
if LDATA
	pop	cx
endif
	mov	osisp,sp		; 1.28 save OSINT stack pointer
	mov	osiss,ss

	cld
	mov	bx,ax
	add	bx,_stacksiz		; top of stack
	mov	compsp,bx		; 1.28 save as compiler's stack pointer
	mov	cx,_mindata_ds
	mov	compss,cx
	SET_WA	bx			; 1.28 startup stack pointer
	add	ax,200			; 100 words smaller for LOWSPMIN
	SETMIN	LOWSPMIN,ax

	MINIMAL	START			; load registers and start compiler

startup endp


locate_stack	proc	near
	publab	locate_stack

	push	bp
	mov	bp,sp
	mov	ax,[bp+4]		; load compiler stack offset
	mov	compsp,ax
	mov	ax,[bp+6]		; load compiler stack segment
	mov	compss,ax
	pop	bp
	retc	4

locate_stack	endp



;
;-----------
;
;	mimimal -- call MINIMAL function from C
;
;	Usage:  extern void minimal(WORD callno)
;
;	where:
;	  callno is an ordinal defined in osint.h, osint.inc, and calltab.
;
;	Minimal registers WA, WB, WC, XR, and XL are loaded and
;	saved from/to the register block.
;
;	Note that we pass along a code on the top byte of the stack.  This
;	code is used to adjust the return segment on the stack to
;	guarantee that it is odd parity during the time the Minimal
;	code is running.  This value is XORed into the low-order
;	byte of the return segment on the stack prior to returning.
;
;	Note that before restart is called, we do not yet have compiler
;	stack to switch to.  In that case, just make the call on the
;	the OSINT stack.
;

minimal	proc	near
	publab	minimal

	push	bp
	mov	bp,sp
	push	ax			; save all registers for C
	push	cx
	push	dx
	push	bx
	push	si
	push	di
	push	es

	fld	reg_ra			; using osint's stack segment

	mov	cx,cs			; prepare to test parity of our CS
	mov	bx,cx
	and	cx,1			; isolate parity bit
	xor	cl,1			; cl = 0 if CS odd, cl = 1 if CS even
	or	bl,cl			; force CS to be odd

	mov	bp,[bp+4]		; argument call number (do before stack switch)

	mov	osisp,sp		; 1.28 save OSINT stack pointer
	mov	osiss,ss

	call	compstk			; 1.28 switch to compiler stack

	push	bx			; build far return
	lea	bx,minret
	push	bx			; offset
	push	cx			; push odd/even flag (0/1)

	cld
	mov	cx,reg_wa		; restore registers
	mov	bx,reg_wb
	mov	dx,reg_wc		; also reg_ia (lsh if IA32)
	mov	di,reg_xr
	mov	si,reg_xl
	mov	ax,reg_cp		; copy C version of CP
if IA32
	push	word ptr reg_ia		; copy C version of IA
endif
	mov	ds,_mindata_ds
	assume	DS:MINDATA
	mov	REG_CP,ax		; to Minimal version
if IA32
	pop	word ptr REG_IA		;  "   "        "
endif
	mov	ax,ds
	mov	es,ax

	jmp	callmin			; call into Minimal

	even				; force odd-parity return
	nop
minret:
	mov	ax,REG_CP		; fetch CP from Minimal space
	mov	ds,dgroup_ds		; switch to C data space
	assume	DS:DGROUP
	mov	reg_cp,ax		; save CP in C data space

	call	osistk			; switch to back osint stack

	mov	reg_wa,cx		; save registers
	mov	reg_wb,bx
	mov	reg_wc,dx
	mov	reg_xr,di
	mov	reg_xl,si
	fstp	reg_ra			; using osint's stack segment

	pop	es
	pop	di
	pop	si
	pop	bx
	pop	dx
	pop	cx
	pop	ax
	pop	bp
	retc	2

minimal	endp


if direct eq 0
;
;-----------
;
;	minoff -- obtain address of MINIMAL variable or routine.
;
;	Usage:  extern WORD *minoff(WORD valno)
;
;	where:
;	  valno is an ordinal defined in osint.h, osint.inc and valtab.
;
;	addresses of variables are in the V37 data segment.
;	addresses of routines are offsets within the V37 code segment.

minoff	proc	near
	publab	minoff

	push	bp
	mov	bp,sp
	mov	bp,[bp+4]		; get ordinal
	shl	bp,1
	mov	ax,ds:valtab[bp]	; get address of Minimal value
if LDATA
	mov	dx,_mindata_ds		; in case need to access value
	mov	es,dx
endif
	pop	bp
	retc	2

minoff	endp
endif



;
;-----------
;
;	osistk	- switch to osint stack
;
;	Bashes AX.
;
osistk	proc	near
;	mov	ax,sp			; used for debugging only
;	cmp	ax,810h			; should be >= 810h
;	jae	osis0
;	int	3
;osis0:
	pop	ax
;	cmp	compss,0		; 1.28 is there a compiler stack
;	jne	short osis0		; 1.28 jump if none yet
;	int	3
osis0:	mov	compsp,sp
;	mov	compss,ss		; redundant (doesn't change)
	loadstk	osiss,osisp		; 1.28 switch to OSINT stack
osis1:	jmp	ax
osistk	endp

;
;-----------
;
;	compstk	- switch to compiler stack.  Use after call to osistk.
;
;	Bashes AX.
;
compstk	proc	near
;	mov	ax,sp			; used for debugging only
;	cmp	ax,810h			; should be <= 810h
;	jbe	comps0
;	int	3
;comps0:
	pop	ax
	mov	osisp,sp		; 1.28 save OSINT stack pointer
;	mov	osiss,ss		; redundant (doesn't change)
;	cmp	compss,0		; 1.28 is there a compiler stack
;	jne	short comps0		; 1.28 jump if none yet
;	int	3
comps0:	loadstk	compss,compsp		; 1.28 switch to compiler stack
comps1:	push	ds
	pop	es			; Must have ES=DS (FPU emulator may bash)
	jmp	ax
compstk	endp



;
;-----------
;
;	dosvers	- get DOS version number
;
dosvers	proc	near
	publab	dosvers

	push	bx
	push	cx
	push	dx
	mov	ax,3000h		; Get version number
	int	21h
	pop	dx
	pop	cx
	pop	bx
	retc	0

dosvers	endp


;
;-----------
;
;	get_fp	- get C caller's FP (frame pointer)
;
;	get_fp() returns the frame pointer for the C function that called
;	this function.  HOWEVER, THIS FUNCTION IS ONLY CALLED BY _zysXI.
;
;	C function _zysxi calls this function to determine the lowest USEFUL
;	word on the stack, so that only the useful part of the stack will be
;	saved in the load module.
;
;	The flow goes like this:
;
;	(1) User's spitbol program calls EXIT function
;
;	(2) spitbol compiler calls interface routine sysxi to handle exit
;
;	(3) Interface routine sysxi passes control to ccaller which then
;	    calls C function _zysxi
;
;	(4) C function _zysxi will write a load module, but needs to save
;	    a copy of the current stack in the load module.  The base of
;	    the part of the stack to be saved begins with the frame of our
;	    caller, so that the load module can execute a return to ccaller.
;
;	    This will allow the load module to pretend to be returning from
;	    C function _zysxi.  So, C function _zysxi calls this function,
;	    get_fp, to determine the BASE OF THE USEFUL PART OF THE STACK.
;
;	    We cheat just a little bit here.  C function _zysxi can (and does)
;	    have local variables, but we won't save them in the load module.
;	    Only the portion of the frame established by the 80386 call
;	    instruction, from BP up, is saved.  These local variables
;	    aren't needed, because the load module will not be going back
;	    to C function _zysxi.  Instead when function restart returns, it
;	    will act as if C function _zysxi is returning.
;
;	(5) After writing the load module, C function _zysxi calls C function
;	    _zysej to terminate spitbol's execution.
;
;	(6) When the resulting load module is executed, C function main
;	    calls function restart.  Function restart restores the stack
;	    and then does a return.  This return will act as if it is
;	    C function _zysxi doing the return and the user's program will
;	    continue execution following its call to EXIT.
;
;	On entry to _get_fp, the stack looks like
;
;		/      ...	/
;	(high)	|             	|
;		|---------------|
;	ZYSXI	|    old PC	|  --> return point in CCALLER
;	  +	|---------------|  USEFUL part of stack
;	frame	|    old BP	|  <<<<-- BP of get_fp's caller
;		|---------------|     -
;		|     ZYSXI's	|     -
;		/     locals	/     - NON-USEFUL part of stack
;		|		|     ------
;	=======	|---------------|
;	SP-->	|    old PC	|  --> return PC in C function ZYSXI
;	(low)	+---------------+
;
;	On exit, return BP in AX. This is the lower limit on the
;	size of the stack.

get_fp	proc	near
	publab	get_fp

	mov	ax,reg_xs	; Minimal's XS
	inc	ax		; pop return from call to SYSBX or SYSXI
	inc	ax
	mov	dx,compss
	retc	0		; done

get_fp	endp
;
;-----------
;
;       get_ra - get real value from RA
;
;       double get_ra(void);
;
;       Because RA is kept on the floating point stack, this
;       routine just needs to duplicate the value on the top
;	of the stack.
;
get_ra	proc	near
	publab	get_ra
	fld	st(0)		; using osint's stack
	retc	0		; done
get_ra	endp


if SAVEFILE
;
;-----------
;
;	restart - restart for load module
;
;	restart( char *dummy, char *stackbase ) - startup compiler
;
;	The OSINT main function calls restart when resuming execution
;	of a program from a load module.  The OSINT main function has
;	reset global variables except for the stack and any associated
;	variables.
;
;	Before restoring stack, set up values for proper checking of
;	stack overflow. (initial sp here will most likely differ
;	from initial sp when compile was done.)
;
;	It is also necessary to relocate any addresses in the the stack
;	that point within the stack itself.  An adjustment factor is
;	calculated as the difference between the stbas at exit() time,
;	and stbas at restart() time.  As the stack is transferred from
;	tscblk to the active stack, each word is inspected to see if it
;	points within the old stack boundaries.  If so, the adjustment
;	factor is subtracted from it.

	ext	rereloc,near

restart proc	near
	publab	restart

	pop	ax			; discard return
if LPROG
	pop	ax
endif
	pop	ax			; discard dummy
if LDATA
	pop	ax			;   "       "
endif
	pop	ax			; get lowest legal stack value (sp)
if LDATA
	pop	bx			; get compiler ss
endif
	mov	osisp,sp		; 1.28 save OSINT stack pointer
	mov	osiss,ss

	add	ax,stacksiz		; top of stack
	loadstk	bx,ax			; switch to compiler stack

					; set up for stack relocation
	mov	bx,lmodstk		; bottom of saved stack
	GETMIN	cx,STBAS		; cx = stbas from exit() time
	lea	ax,_TSCBLK.scstr	; top of saved stack
	sub	bx,ax			; bx = size of saved stack
	mov	dx,cx
	sub	dx,bx			; dx = stack bottom from exit() time
	mov	bx,cx
	sub	bx,sp			; bx = old stbas - new stbas

	mov	di,sp
	push	dx
	SETMIN	STBAS,di		; save initial sp
	sub	di,stacksiz
	add	di,200			; 100 words smaller for LOWSPMIN
	SETMIN	LOWSPMIN,di
	pop	dx
;
;	restore stack from tscblk.
;
	mov	si,lmodstk		; -> bottom word of stack in tscblk
	lea	di,_TSCBLK.scstr		; -> top word of stack
	cmp	si,di			; Any stack to transfer?
	je	short re3			;  skip if not
	dec	si
	dec	si
	std
	mov	ds,_mindata_ds		; load Minimal data space
	assume	DS:MINDATA
re1:	lodsw				; get old stack word to ax
	test	al,1			; odd parity words are NEVER
	jnz	short re2		;  relocated (they are call returns)
	cmp	ax,dx			; below old stack bottom?
	jb	short re2		;   j. if ax < dx
	cmp	ax,cx			; above old stack top?
	ja	short re2		;   j. if ax > cx
	sub	ax,bx			; within old stack, perform relocation
re2:	push	ax			; transfer word of stack
	cmp	si,di			; if not at end of relocation then
	jae	re1			;    loop back
	mov	ds,dgroup_ds		; switch to C data space
	assume	DS:DGROUP

re3:	cld
	mov	compsp,sp		; 1.28 initialize compiler's stack pointer
	mov	compss,ss
	call	osistk			; back to osint's stack
	callc	rereloc,0		; V1.08 relocate compiler pointers into stack
	GETMIN	ax,STATB		; V1.34 start of static region to XR
	SET_XR  ax
	MINIMAL	INSTA			; V1.34 initialize static region


;
;	now pretend that we're executing the following C statement from
;	function zysxi:
;
;		return  NORMAL_RETURN;
;
;	If the load module was invoked by EXIT(), the return path is
;	as follows:  back to ccaller, back to S$EXT following SYSXI call,
;	back to user program following EXIT() call.
;
;	Alternately, the user specified -w as a command line option, and
;	SYSBX called MAKEEXEC, which in turn called SYSXI.  The return path
;	should be:  back to ccaller, back to MAKEEXEC following SYSXI call,
;	back to SYSBX, back to MINIMAL code.  If we allowed this to happen,
;	then it would require that stacked return address to SYSBX still be
;	valid, which may not be true if some of the C programs have changed
;	size.  Instead, we clear the stack and execute the restart code that
;	simulates resumption just past the SYSBX call in the MINIMAL code.
;	We distinguish this case by noting the variable STAGE is 4.
;
	call	startbrk		; start control-C logic

	GETMIN	ax,STAGE		; is this a -w call?
	cmp	ax,4
	je	re4			; yes, do a complete fudge
;
;	Adjust return segment
;
	mov	ax,SEG callmin
	mov	word ptr reg_pc+2,ax

;
;	Jump back to cc1 with return value = NORMAL_RETURN
;	Put an initial value on the floating point stack.
;
	fldz				; using osint's stack
	mov	ax,-1
	jmp	cc1			; jump back

;	Here if -w produced load module.  simulate all the code that
;	would occur if we naively returned to sysbx.  Clear the stack and
;	go for it.
;
re4:	GETMIN	ax,STBAS
	mov	compsp,ax		; 1.28 empty the stack

;	Code that would be executed if we had returned to makeexec:
;
	xor	ax,ax
	SETMIN	GBCNT,ax		; reset garbage collect count
	callc	zystm,0			; Fetch execution time to reg_ia
if IA32
	mov	ax,word ptr reg_ia	; Set time into compiler
	mov	dx,word ptr reg_ia+2
	SETMIN	TIMSX,ax,dx
else
	mov	ax,reg_ia		; Set time into compiler
	SETMIN	TIMSX,ax
endif
;	Code that would be executed if we returned to sysbx:
;
	PushPtr	<word ptr outptr@>,<word ptr outptr@ + 2>
	callc	swcoup,dPtrSize		; swcoup(outptr)

;	Jump to minimal code following call to sysbx.
	MINIMAL	RSTRT			; no return

restart endp

if 0
; 4/28/95 - recoded in C and moved to sysbcc16.c
;
;-----------
;
;	makeexec( scptr, type ) - C callable make executable file function
;
;	Allows C function _zysbx() to create executable files
;	(a.out files) in response to user's -w command option.
;
;	SPITBOL performed a garbage collection prior to calling
;	SYSBX, so there is no need to duplicate it here.
;
;	Then _zysxi() is invoked directly to write the module.
;
;	Input:	bp+4 - Pointer to SCBLK for load module name.
;		bp+6 - Type (-3 or +3)
;	Output:	Return with ax <> 0 if error writing a.out
;		eax = 0 if type was |4| and no error.
;		No return if type was |3| and file written successfully.
;		When a.out is eventually loaded and executed, makeexec()
;		is resumed and returns with ax = 0.
;		Upon resumption, the execution start time and garbage collect
;		count are reset appropriately.
;
;

makearg	struc
mak_bp	dw	?
	cPtr@	mak_ip,?,__C__
	dPtr@	mak_scp,?,__C__
mak_typ	dw	?
makearg	ends

makeexec proc	near			; this routine preserves bx, si, di
	publab	makeexec

	push	bp
	mov	bp,sp
	mov	dx,word ptr mak_scp@[bp]; Get offset to load module name
	mov	ax,mak_typ[bp]		; Get type
	push	reg_xr			; Save _zysxi()'s argument registers
if IA32
	push	word ptr reg_ia+2
endif
	push	word ptr reg_ia		; (but not XL)
	push	reg_wb
	push	reg_wa

	mov	reg_wa,dx		; WA = file name SCBLK (in mindata_ds)
	mov	word ptr reg_ia,ax	; IA = Type
if IA32
	cwd
	mov	word ptr reg_ia+2,dx
endif
	xor	ax,ax
	mov	reg_xl,ax		; XL = 0
	mov	reg_wb,ax		; WB = 0
	GETOFF	ax,HEADV
	mov	reg_xr,ax		; XR = =HEADV

	callc	zysxi,0			; Write save file
	inc	ax			; Test for normal return (-1)
	jnz	short make1
	inc	ax			; Return 0
make1:	dec	ax

	pop	reg_wa
	pop	reg_wb			; Restore arg regs
	pop	word ptr reg_ia
if IA32
	pop	word ptr reg_ia+2
endif
	pop	reg_xr
	mov	reg_xl,0

	pop	bp
	retc	((size makearg) - 2 - cPtrSize)	; Return result in ax

makeexec endp
endif
endif

if IA32
;
;-----------
;
;	CVD_ - convert by division
;
;	Input	IA (DX:REG_IA) = number <=0 to convert
;	Output	IA / 10
;		WA (CX) = remainder + '0'
;
	assume	DS:MINDATA
	public	CVD_
CVD_	proc	far

	mov	ax,word ptr REG_IA
	not	dx		; make IA positive
	neg	ax
	sbb	dx,-1
	or	dx,dx		; IA(msh) == 0?
	je	short cvd10
	push	bx
	mov	bx,word ptr ten
	mov	cx,32
	xor	bp,bp		; remainder developed here
cvd1:	shl	ax,1
	rcl	dx,1
	rcl	bp,1
	cmp	bp,bx
	jb	short cvd3
	sub	bp,bx
	inc	ax
cvd3:	loop	cvd1
	not	dx		; make IA negative again
	neg	ax
	sbb	dx,-1
	mov	word ptr REG_IA,ax
	mov	cx,bp		; get remainder
	add	cl,'0'		; return remainder + '0' in WA
	pop	bx
	ret

cvd10:	div	word ptr ten	; divide by 10. dx = remainder (positive)
	add	dl,'0'		; convert remainder to ascii ('0')
	mov	cx,dx		; return remainder + '0' in WA
	neg	ax		; make IA negative again
	cwd
	mov	word ptr REG_IA,ax
	ret

CVD_	endp
else
;
;-----------
;
;	CVD_ - convert by division
;
;	Input	IA (DX) = number <=0 to convert
;	Output	IA / 10
;		WA (CX) = remainder + '0'
;
	assume	DS:MINDATA
	public	CVD_
CVD_	proc	far

	xchg	ax,dx		; IA to AX
	cwd			; sign extend
	idiv	ten		; divide by 10. dx = remainder (negative)
	neg	dx		; make remainder positive
	add	dl,30h		; convert remainder to ascii ('0')
	mov	cx,dx		; return remainder in WA
	xchg	dx,ax		; return quotient in IA
	ret

CVD_	endp
endif

if IA32
;
;-----------
;
;	CVM_ - convert by multiplication
;
;	Input	IA (DX:REG_IA) = number <=0 to convert
;	Output	IA * 10 + (WB - '0')
;		overflow flag set appropriately
;
;	Unsigned multiplication algorithm:
;
;		   P|Q = IA
;		 x   S = Argument = 10
;	       -------
;	       	| |Q*S
;	     +  |P*S|
;	    ----------
;              A|B|C|D
;
	public	CVM_
CVM_	proc	far

	push	si		; save SI
	mov	si,word ptr REG_IA ; SI = Q
	not	dx		;   make IA positive
	neg	si
	sbb	dx,-1
	xchg	dx,ax		; AX = P
	test	ax,ax		; P == 0?
	je	short cvm3	;  yes, no need to multiply it
	imul	word ptr ten	; DX:AX = P*10
	jo	short cvm6	; if more than 15 bit result
cvm3:	xchg	si,ax		; AX = Q, SI = (P*10)(low)
	mul	word ptr ten	; DX:AX = Q*10
	add	dx,si
	jo	short cvm6	; overflow if result too large
	not	dx		; make result negative
	neg	ax
	sbb	dx,-1
	sub	bl,'0'		; subtract (WB-'0')
	sub	ax,bx
	sbb	dx,0
	jo	short cvm6
	mov	word ptr REG_IA,ax
	pop	si		; restore SI
	ret

cvm6:	pop	si
	mov	al,80h		; set overflow indicator
	dec	al
	ret

CVM_	endp
endif

;----------
;
;    Calls to C
;
;	The calling convention of the Turbo C compiler is thus:
;
;	int or long results returned in AX or DX:AX
;	doubles are pushed on the CPU stack (msh first), returned
;	on the emulator (or 8087) stack.
;	C function preserves BP, SI, DI
;
;

if IA32
;
;-----------
;
;	MLI_ - signed 32-bit multiply DX:AX by long at DS:[BP].
;
;	Algorithm:
;
;		IA = P|Q
;		Argument = R|S	where P,Q,R,S are each 16-bit values.
;
;	1. Record sign of result (P xor R).
;	2. Make IA and argument positive.
;	3. Perform unsigned multiply.
;	4. Correct sign of result.
;
;	Unsigned multiplication algorithm:
;
;		   P|Q = IA
;		 x R|S = Argument
;	       -------
;	       	| |Q*S
;	     +  |P*S|
;	     +  |R*Q|
;            + P*R| |
;	    ----------
;              A|B|C|D
;
;	Overflow if A or B is non-zero, or if sign of C|D is negative.
;
;	Input:
;		DX:REG_IA(lsh) = Multiplicand
;		DS:[BP] = pointer to multiplier
;
;	Output:
;		DX:REG_IA(lsh) = Result
;		Overflow flag set if result too large, else clear.
;
;	Destroys:
;		BP
;
	public	MLI_
MLI_	proc	far

	push	bx		; save BX
	push	cx		; save CX
	push	si		; save SI
	mov	si,word ptr REG_IA ; SI = Q
	mov	cx,ds:[bp]+2	; get argument to CX:BP = R|S
	mov	bp,ds:[bp]                     
	mov	bl,dh		; set up negative indicator
	xor	bl,ch		; in sign bit of BL.
	or	dh,dh		; IA negative?
	jns	short mli1	;   no. go check argument
	not	dx		;   yes. 2's comp. argument
	neg	si
	sbb	dx,-1
mli1:	or	ch,ch		; argument negative?
	jns	short mli2	;   no. go multiply
	not	cx		;   yes. 2's comp. argument
	neg	bp
	sbb	cx,-1
mli2:	xchg	dx,ax		; AX = P
	test	ax,ax		; P == 0?
	je	short mli3	;  yes, no need to multiply it
	or	cx,cx		; are both P and R non-zero?
	jnz	short mli6	;   yes. will result in overflow
	imul	bp		; DX:AX = P*S
	jo	short mli6	; if P*S(high) is more than 15 bits
mli3:	jcxz	mli4		; R == 0?  Skip multiply if so.
	xchg	cx,ax		; AX = R, CX = P*S(low)
	imul	si		; DX:AX = R*Q
	jo	short mli6	; if R*Q(high) is more than 15 bits
	add	ax,cx		; AX = P*S(low) + R*Q(low)
	jo	short mli6	; overflow if intermediate result too large
mli4:	xchg	si,ax		; AX = Q, SI = P*S(low) + R*Q(low)
	mul	bp		; DX:AX = Q*S
	add	dx,si
	jo	short mli6	; overflow if result too large
	or	bl,bl		; is result sign correct?
	jns	short mli5	;   yes, sign ok
	not	dx		;   no, correct it
	neg	ax
	sbb	dx,-1
mli5:	mov	word ptr REG_IA,ax
	pop	si		; restore SI
	pop	cx		; restore CX
	pop	bx		; restore BX
	or	al,al		; clear overflow indicator
	ret

mli6:	pop	si
setovr:	pop	cx		; enter here from DVI_ and RMI_
	pop	bx
	mov	al,80h		; set overflow indicator
	dec	al
	ret

MLI_	endp
endif


if IA32
;
;-----------
;
;	DVI_ - divide DX:REG_IA(lsh) by long at DS:[BP]
;
;	Input:
;		DX:REG_IA(lsh) = Dividend
;		DS:[BP] = pointer to divisor
;
;	Output:
;		DS:REG_IA(lsh) = Result
;		Overflow flag set if divisor is 0, else clear.
;
;	Destroys:
;		BP
;
	public	DVI_
	extrn	F_LDIV@:far
DVI_	proc	far

	push	bx
	push	cx
	mov	bx,ds:[bp]
	mov	cx,ds:[bp]+2	; load divisor
	or	bx,bx		; check for zero divisor
	jne	short dvi1
	jcxz	setovr		; jump if both halves are zero
dvi1:	push	cx		; push divisor
	push	bx
	push	dx		; push numerator
	push	word ptr REG_IA
	call	F_LDIV@
	mov	word ptr REG_IA,ax
	pop	cx
	pop	bx
	or	al,al		; clear ovrflow indicator
	ret	

DVI_	endp
else
;
;-----------
;
;	DVI_ - divide IA (DX) by AX
;
	public	DVI_
DVI_	proc	far

	or	ax,ax		; test for 0
	jz	short setovr	; jump if 0 divisor
	xchg	bp,ax		; divisor to bp
	xchg	ax,dx		; dividend in ax
	cwd			; extend dividend
	idiv	bp		; perform division. ax=quotient, dx=remainder
	xchg	dx,ax		; place quotient in dx (IA)
	xor	ax,ax		; clear overflow indicator
	ret

DVI_	endp
endif


if IA32
;
;-----------
;
;	RMI_ - remainder of DX:AX divided by long at DS:[BP]
;
;	Input:
;		DX:REG_IA(lsh) = Dividend
;		DS:[BP] = pointer to divisor
;
;	Output:
;		DX:REG_IA(lsh) = Result
;		Overflow flag set if divisor is 0, else clear.
;
;	Destroys:
;		BP
;
	public	RMI_
	extrn	LMOD@:far
RMI_	proc	far

	push	bx
	push	cx
	mov	bx,ds:[bp]
	mov	cx,ds:[bp]+2	; load divisor
	or	bx,bx		; check for zero divisor
	jne	short rmi1
	jcxz	setovr		; jump if both halves are zero
rmi1:	push	cx		; push divisor
	push	bx
	push	dx		; push numerator
	push	word ptr REG_IA
	call	LMOD@
	mov	word ptr REG_IA,ax
	pop	cx
	pop	bx
	or	al,al		; clear ovrflow indicator
	ret	

RMI_	endp
else
;
;-----------
;
;	RMI_ - remainder of IA (DX) divided by AX
;
	public	RMI_
RMI_	proc	far

	or	ax,ax		; test for 0
	jz	short setovr	; jump if 0 divisor
	xchg	bp,ax		; divisor to bp
	xchg	ax,dx		; dividend in ax
	cwd			; extend dividend
	idiv	bp		; perform division. ax=quotient, dx=remainder
	xor	ax,ax		; clear overflow indicator
	ret			; return remainder in dx (IA)
setovr:	mov	al,80h		; set overflow indicator
	dec	al
	ret

RMI_	endp
endif


;
;----------
;
;	RTI_ - convert real in RA to integer in IA
;               returns C=0 if fit OK, C=1 if too large to convert
;
	public	RTI_
RTI_	proc	far
        call    osistk                  ; switch to osint's stack
        fst     REG_RA                  ; get RA to memory
        fwait
if IA32
; 41E00000 00000000 = 2147483648.0
; 41E00000 00200000 = 2147483649.0
        mov     ax, word ptr REG_RA+6   ; RA msh
        or      ah,ah
        js      RTI_2                   ; jump if negative real
        cmp     ax,41E0H                ; test against 2147483648
        jae     RTI_1                   ; jump if >= +2147483648
else
; 40E00000 00000000 = 32768.0
; 40E00020 00000000 = 32769.0
        mov     ax, word ptr REG_RA+6   ; RA msh
        or      ah,ah
        js      RTI_2                   ; jump if negative real
        cmp     ax,40E0H                ; test against 32768
        jae     RTI_1                   ; jump if >= +32768
endif
RTI_0:  fstcw   cw                      ; store control word
        fstcw   cwtemp                  ; save control word copy
	or	byte ptr cw+1, 0Ch	; change RC to Chop
	fldcw	cw			; load new control
        fist    REG_IA                  ; Store as integer
	fstsw	cw			; store status
	fldcw	cwtemp			; reload control word
	call	compstk			; back to compiler's stack
if IA32
	mov	dx,word ptr REG_IA+2	; return msh in DX
else
	mov	dx,REG_IA		; return in IA
endif
        clc                             ; return C=0 for fit OK
        ret

; here to test negative number
RTI_2:  and     ah,7FH                  ; make positive
if IA32
        cmp     ax,41E0H                ; test against 2147483649
        jb      RTI_0                   ; definately smaller
        ja      RTI_1                   ; definately larger
        cmp     word ptr REG_RA+4, 0
        jne     RTI_1                   ; definately larger
        cmp     word ptr REG_RA+2, 0020H
        jb      RTI_0
else
        cmp     ax,40E0H                ; test against 32769
        jb      RTI_0                   ; definately smaller
        ja      RTI_1                   ; definately larger
        cmp     word ptr REG_RA+4, 0020H
        jb      RTI_0
endif
RTI_1:  call    compstk
        stc                             ; return C=1 for too large to convert
        ret

RTI_	endp
;
;----------
;
;	ITR_ - convert integer in IA to real in RA
;
	public	ITR_
ITR_	proc	far

if IA32
	mov	word ptr REG_IA+2,dx	; store msh
else
	mov	REG_IA,dx		; make it accessible in memory
endif
	call	osistk			; switch to osint's stack
	fstp	st(0)
	fild	REG_IA
	call	compstk			; back to compiler's stack
	ret

ITR_	endp

;
;----------
;
;	LDR_ load real at DS:[BP]
;
	public	LDR_

LDR_	proc	far

   	call	osistk				; switch to osint's stack
	fstp	st(0)				; dump current st(0) contents
	fld	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

LDR_	endp

;
;----------
;
;	STR_ store real at DS:[BP]
;
	public	STR_

STR_	proc	far

   	call	osistk				; switch to osint's stack
	fst	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

STR_	endp

;
;----------
;
;	ADR_ add real at DS:[BP]
;
	public	ADR_

ADR_	proc	far

   	call	osistk				; switch to osint's stack
	fadd	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

ADR_	endp

;
;----------
;
;	SBR_ subtract real at DS:[BP]
;
	public	SBR_

SBR_	proc	far

   	call	osistk				; switch to osint's stack
	fsub	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

SBR_	endp

;
;----------
;
;	MLR_ multiply real at DS:[BP]
;
	public	MLR_

MLR_	proc	far

   	call	osistk				; switch to osint's stack
	fmul	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

MLR_	endp

;
;----------
;
;	DVR_ divide real at DS:[BP]
;
	public	DVR_

DVR_	proc	far

   	call	osistk				; switch to osint's stack
	fdiv	qword ptr ds:[bp]
	call	compstk				; back to compiler's stack
	ret

DVR_	endp

;
;----------
;
;	NGR_ negate real
;
	public	NGR_

NGR_	proc	far

   	call	osistk				; switch to osint's stack
	fchs
	call	compstk				; back to compiler's stack
	ret

NGR_	endp

;
;----------
;
;	SQR_ negate real
;
	public	SQR_

SQR_	proc	far

   	call	osistk				; switch to osint's stack
	fsqrt
	call	compstk				; back to compiler's stack
	ret

SQR_	endp

;
;----------
;
;	ATN_ arctangent of real in RA
;
	public	ATN_

ATN_	proc	far

	mov	bp, offset DGROUP: j_atn	; point to address of routine
mathj:	call	osistk				; switch to compiler's stack
	fstp	REG_RA				; get RA to memory
	push	bx				; preserve regs for C
	push	cx
	push	dx
	fwait
	push	word ptr REG_RA+6		; RA msh
	push	word ptr REG_RA+4
	push	word ptr REG_RA+2
	push	word ptr REG_RA			; RA lsh
	mov	ds,dgroup_ds			; switch to C data space
	assume	DS:DGROUP
	call	dword ptr ds:[bp]		; perform op, result in RA
	mov	ds,_mindata_ds			; switch to Minimal data space
	assume	DS:MINDATA
	add	sp,8				; pop arg
	pop	dx				; restore regs
	pop	cx
	pop	bx
	call	compstk				; back to compiler's stack
	ret

ATN_	endp
;
;----------
;
;	CHP_ chop fractional part of real in RA
;
	public	CHP_

CHP_	proc	far

	call	osistk			; switch to osint stack
	fstcw	cw			; store control word
	fstcw	cwtemp			; and copy
	or	byte ptr cw+1, 0Ch	; change RC to chop
	fldcw	cw
	frndint				; chop
	fldcw	cwtemp			; restore control word
	call	compstk			; back to compiler's stack
	ret

CHP_	endp
;
;----------
;
;	COS_ cosine of real in RA
;
	public	COS_

COS_	proc	far

      	mov	bp,offset DGROUP: j_cos		; point to address of routine
	jmp	mathj				; do it

COS_	endp
;
;----------
;
;	ETX_ exponential of real in RA
;
	public	ETX_

ETX_	proc	far

	mov	bp,offset DGROUP: j_etx		; point to address of routine
	jmp	mathj				; do it

ETX_	endp
;
;----------
;
;	LNF_ natural logarithm of real in RA
;
	public	LNF_

LNF_	proc	far

	mov	bp,offset DGROUP: j_lnf		; point to address of routine
	jmp	mathj				; do it

LNF_	endp
;
;----------
;
;	SIN_ arctangent of real in RA
;
	public	SIN_

SIN_	proc	far

	mov	bp,offset DGROUP: j_sin		; point to address of routine
	jmp	mathj				; do it

SIN_	endp
;
;----------
;
;	TAN_ arctangent of real in RA
;
	public	TAN_

TAN_	proc	far

	mov	bp,offset DGROUP: j_tan		; point to address of routine
	jmp	mathj				; do it

TAN_	endp
;
;----------
;
;	CPR_ compare real in RA to 0
;
	public	CPR_

CPR_	proc	far

	call	osistk			; switch to osint's stack
	ftst				; compare against zero
	fstsw	cw			; get results
        call    compstk                 ; back to compiler's stack
        fwait
        mov     ah,byte ptr cw+1        ; load floating condition codes
	sahf				; jam into CPU condition codes
	ret

CPR_	endp
;
;----------
;
;	OVR_ test for overflow value in RA
;		returns Z reset if infinity or NAN or too large for 64-bit real
;
	public	OVR_

OVR_	proc	far

	call	osistk			; switch to osint
	fst	REG_RA			; try to fit in 64 bits
	fwait
	mov	ax, word ptr REG_RA+6	; get top 2 bytes
	and	ax, 7ff0h		; check for infinity or nan
	add	ax, 10h			; ==> 8000 if infinity
	js	short ovr1		; if matherr or exception
	call	compstk			; back to compiler stack
	xor	al,al
	ret               		; return Z set
ovr1:   call	compstk			; back to compiler stack
	or	al,1			; return Z clear
	ret


OVR_	endp

ifndef __WINDOWS__
;
;----------
;
;	startbrk			Start system BREAK logic

	assume	DS:DGROUP
brki	equ	23h			; break interrupt number

startbrk proc	near
	publab	startbrk

	push	bx
	mov	brkpnd,0		; no breaks pending

	push	es
	mov	ax,3500h+brki		; save original vector
	int	21h			; vector
	mov	word ptr i23_adr,bx
	mov	word ptr i23_adr+2,es
	pop	es

	mov	ax,2500h+brki		; set vector
	push	ds			; save ds
	push	cs
	pop	ds
	lea	dx,int23		; Use ds:ds for vector
	int	21h
	pop	ds			; restore ds

	mov	ax,3300h		; Get current state of break checking
	int	21h
	mov	brkste,dl		; Save state for exit
	mov	ax,3301h		; Set to check on all DOS calls
	mov	dl,al
	int	21h

	pop	bx
	retc	0

startbrk endp

;
;----------
;
;	endbrk				End system BREAK logic

endbrk	proc	near
	publab	endbrk

	mov	dx,word ptr i23_adr	; restore original
	or	dx,dx			; did we initialize?
	je	endb1			; if not

	mov	ax,2500h+brki		; set vector
	push	ds
	mov	ds,word ptr i23_adr+2
	int	21h
	pop	ds

	mov	ax,3301h
	mov	dl,brkste		; restore old BREAK state
	int	21h

endb1:	ret

endbrk	endp



;
;----------
;
;	int23				Our interrupt routine for BREAK interrupt.
;					It was during a DOS function call.

int23	proc	near
	publab	int23

	push	ds			; Save DOS DS, ES, DX, AX
	push	es
	push	ax
        push    cx
        push    dx
	mov	ds,dgroup_ds
	inc	brkpnd			; Set Break Pending non-zero
        GETMIN  bx,STMCT                ; bx = current statement downcounter
	mov	ax,1
	SETMIN	POLCT,ax		; Force minimal to call SYSPL soon
        mov     ax,1
        SETMIN  STMCT,ax                ; Force STMCT to check counters
        GETMIN  cx,STMCS
        dec     bx
        sub     cx,bx                   ; STMCS - (STMCT - 1)
        SETMIN  STMCS,cx                ; Update STMCS to statement count correct
	pop	dx
        pop     cx
        pop     ax
	pop	es
	pop	ds
	cmp	ah,0ah			; Was DOS character I/O interrupted?
	ja	int23b			; J. if no
	add	sp,6			; Yes, discard return to DOS
	mov	al,0dh			; Return CR to input functions
	cmp	ah,08h
	je	int23d
	cmp	ah,01h
	jne	int23c
int23d:	mov	cx,1			; Reduce to one char for reader
	jmp	short int23b
int23c:	cmp	ah,0ah
	jne	int23b			; If not buffered Input
	mov	word ptr es:[di+1],0d00h ; Set count to zero, CR in
					;  first char position
int23b:	iret				; Return to DOS or cinread call.

int23	endp

;
;----------
;
;	pollevent allow control-c to enter system by polling keyboard
;
pollevent	proc	near
	publab	pollevent

	mov	ah,0bh			; Innocuous check of keyboard status
	int	21h			; will allow Control-C to enter
	retc    0

pollevent	endp
endif

;
;----------
;
;	cinread(fdn, buffer, size)	Do buffered console input
;
;	Input:	fdn	= file descriptor in case can't use DOS function 0A
;		buffer	= buffer
;		size	= buffer size
;	Output:	ax    = number of bytes transferred if no error
;		= -1 if error
;
;	Preserves ds, es, si, di, bx
;
;	Uses DOS function 0A because that is the function intercepted by
;	various keyboard editing programs, such as DOS Edit.
;
;	If a program has redirected standard output, function 0A's echos
;	will go to the redirected file, instead of the screen.  To overcome
;	this, we save standard out's handle, and force it to be the console
;	(stderr).  Similarly, function 0Ah reads from handle 0, and we
;	force it to the console to preclude reading from a file.
;
;	If insufficient handles remain in the system to do this little
;	shuffle, we simple fall back to the normal DOS read routine.
;
;	Note that in some memory models, DS != SS.

cinarg	struc
cin_bp	dw	?
	cPtr@	cin_ip,?,__C__
cin_fdn	dw	?
	dPtr@	cin_buf,?,__C__
cin_siz	dw	?
cinarg	ends

ct	struc				;cinread temps
crbuf	db	260 dup (?)		;keyboard buffer
ct	ends
zct	equ	(((size ct)+1)/2)*2	;word aligned temp size
ctemp	equ	[bp-zct]		;temp on stack

	ext	read,near

cinread	proc	near
	publab	cinread

	push	bp
	mov	bp,sp
	sub	sp,zct
	push	bx
	push	si
	push	di

	xor	bx,bx			; Save STDIN by duplicating to
	mov	cx,bx			;  get another handle
	xor	ax,ax
	mov	ah,45h			; CX = STDIN
	int	21h
	jnc	short cinr0
	jmp	cinr5			; Out of handles
cinr0:	push	ax			; Save handle to old STDIN
	mov	bx,2			; Make STDIN refer to STDERR
	mov	ah,46h			; so DOS's input comes from keyboard
	int	21h

	mov	bx,1			; Save STDOUT by duplicating to
	mov	cx,bx			;  get another handle
	mov	ah,45h			; CX = STDOUT
	int	21h
	jc	cinr4			; Out of handles
	push	cx
	push	ax			; Save handle to old STDOUT
	mov	bx,2			; Make STDOUT refer to STDERR
	mov	ah,46h			; so DOS' echo goes to screen
	int	21h

	mov	cx,cin_siz[bp]
	inc	cx			; Allow for CR
	cmp	cx,255
	jle	short cinr1
	mov	cl,255			; 255 is the max size for function 0Ah
cinr1:	lea	bx,ctemp.crbuf		; Buffer
	mov	dx,bx
	mov	SS_[bx],cl		; Set up count
	mov	ah,0ah

	pushDS_
	pushSS_
	popDS_
	int	21h			; Do buffered input into [dx+2]
	popDS_

	pop	bx
	pop	cx			; (CX = STDOUT)
	mov	ah,46h			; Restore STDOUT to original file
	int	21h
	mov	ah,3eh			; Discard dup'ed handle
	int	21h

	xor	cx,cx			; CX = STDIN
	pop	bx
	mov	ah,46h			; Restore STDIN to original file
	int	21h
	mov	ah,3eh			; Discard dup'ed handle
	int	21h

	mov	si,dx
	inc	si			; Point to number of bytes read
	mov	bl,byte ptr SS_[si]	; Char count
	xor	bh,bh
	inc	bx			; Include CR
	inc	si
	lea	di,[si+bx]		; Point past CR
	mov	SS_[di],byte ptr 10	; Append LF after CR
	mov	dx,di			; Save pointer to LF
	inc	bx			; Include LF
	cmp	bx,cin_siz[bp]		; Compare with caller's buffer size
	jle	short cinr3
	mov	bx,cin_siz[bp]		; Caller's buffer size limits us
cinr3:	mov	cx,bx
	LES_	di,cin_buf@[bp]
	pushDS_
	pushSS_
	popDS_
  rep	movsb
	push	bx
	mov	bx,2			; Force LF echo to screen
	mov	ah,40h
	mov	cx,1
	int	21h			; Register DX set above
	pop	ax
	popDS_

cinr2:	pop	di
	pop	si
	pop	bx
	mov	sp,bp
	pop	bp
	retc	((size cinarg) - 2 - cPtrSize)

; Here if insufficient handles to save standard out.
; Release standard in.
;
cinr4:	xor	cx,cx			; CX = STDIN
	pop	bx
	mov	ah,46h			; Restore STDIN to original file
	int	21h
	mov	ah,3eh			; Discard dup'ed handle
	int	21h

; Here if insufficient handles to save standard in.
; Just fall back to read routine.
;
cinr5:	push	cin_siz[bp]
	LES_	ax,cin_buf@[bp]
	pushES_
	push	ax
	push	cin_fdn[bp]
	callc	read,dPtrSize+4
	jmp	cinr2

cinread endp

;
;----------
;
;  ALLOCS - allocate new segment
;
;  short int allocs(nbytes)
;
;	Input:  nbytes		number of bytes required
;	Output: segment of allocated segment, or zero if none
;
allocs	proc	 near
	publab	allocs

	push	bp
	mov	bp,sp
	push	bx			;save register
	push	cx
	mov	bx,[bp+4]		;load argument
	add	bx,0fh			;round-up to pargraph size
	mov	cl,4
	shr	bx,cl			;number of paragraphs
	mov	ah,48h			;allocate segment
	int	21h
	jnc	alcs1			;test result
	xor	ax,ax			;failure
alcs1:	pop	cx
	pop	bx			;restore register
	pop	bp
	ret

allocs	endp

;
;----------
;
;  FREES - free a segment
;
;  void frees(short int segment)
;
;	Input:  segment		a segment, or zero
;	Output: none
;
frees	proc	near
	publab	frees

	push	bp
	push	ax			;save registers
	mov	ax,[bp+4]		;load argument
	or	ax,ax			;
	je	frees1			;j. if zero
	push	es
	mov	es,ax			;non-zero, release it
	mov	ah,49h
	int	21h			;free segment
	pop	es
frees1:	pop	ax			;restore register
	pop	bp
	ret

frees	endp


	Code_EndS@
	end
