	page	80,132
	title	FLOAT - Emulated Floating Point for External Functions
	name	FLOAT

;	Copyright 1988-1998 Mark Emmer, Catspaw, Inc. All rights reserved.
;	Use of this source code is governed by a BSD-style
;	license that can be found in the LICENSE file.

;
;	This file contains special values that are unique to Borland's
;	emulated floating point library.  When a SPITBOL external function
;	is assembled with the /e switch to use the emulator, the assembler
;	generates true 80x87 opcodes and adds these special fixup values
;	to them.  When linked with this file, the opcodes are converted by
;	the linker to INT instructions that invoke the emulator.  The
;	values here were obtained by scanning PUBDEFs in the Borland file
;	EMU.LIB, and could change in future releases of Borland products.
;
;	When the INT instruction is executed, the emulator will simulate
;	the corresponding opcode if no 80x87 is present, or if 87=N is
;	found in the program's environment.  If an 80x87 is present (and
;	not disabled with the 87=N environment string), the INT instruction
;	in the external function is replaced with a true 80x87 opcode, and
;	the instruction is re-executed.
;
;	Note that if an external function is built with the emulated
;	instructions, executed on a machine with an 80x87, and then
;	saved in a SAVE file with the EXIT function, it will be saved
;	with the true 80x87 opcode (as modified by the emulator), and
;	future use of the function when the Save file is reloaded will
;	only function properly if executed on a machine with an 80x87.
;
	PUBLIC	FIARQQ
	PUBLIC	FICRQQ
	PUBLIC	FIDRQQ
	PUBLIC	FISRQQ
	PUBLIC	FIWRQQ
	PUBLIC	FIERQQ
	PUBLIC	FJARQQ
	PUBLIC	FJCRQQ
	PUBLIC	FJSRQQ

FIARQQ	equ	0FE32H
FICRQQ	equ	00E32H
FIDRQQ	equ	05C32H
FISRQQ	equ	00632H
FIWRQQ	equ	0A23DH
FIERQQ	equ	01632H
FJARQQ	equ	04000H
FJCRQQ	equ	0C000H
FJSRQQ	equ	08000H

	end
