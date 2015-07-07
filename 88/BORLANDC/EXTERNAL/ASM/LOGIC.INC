
* Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
* Use of this source code is governed by a BSD-style
* license that can be found in the LICENSE file.

* LOGIC.INC
*
* Perform logical or arithmetic operations on integers and strings
*
* Load from SPITBOL with statement:
*	LOAD('LOGIC(INTEGER,ARG2,ARG3)')
*
*	Input:	First Argument = Operation Specifier:
*			1 = Return Logical NOT of ARG2
*			2 = Return ARG2 AND  Arg3
*			3 = Return ARG2 OR   Arg3
*			4 = Return ARG2 XOR  Arg3
*			5 = Return ARG2 NAND Arg3
*			6 = Return ARG2 NOR  Arg3
*			7 = Return ARG2 + ARG3 (overflow ignored)
*			8 = Return ARG2 - ARG3 (overflow ignored)
*			9 = Return ARG2 * ARG3 (unsigned, 16-bit ARG3 only)
*		       10 = Return ARG2 / ARG3 (unsigned, 16-bit ARG3 only)
*		       11 = Return ARG2 shifted left ARG3 bits
*		       12 = Return ARG2 shifted right ARG3 bits (unsigned)
*		       13 = Return ARG2 shifted right ARG3 bits (sign extension)
*		       14 = Return ARG2 rotated left ARG3 bits
*		       15 = Return ARG2 rotated right ARG3 bits
*		       16 = Return integer from digit string in ARG2, base ARG3
*		       17 = Return digit string from integer in ARG2, base ARG3
*			      (conversion base defaults to 16 if ARG3 is 0)  
*
*	ARG2 and ARG3 may be two strings, two integers, or an integer
*			and a string.
*
*	Output:	Result of operation.
*		Function fails if:
*		1. Operation specifier out of range
*		2. Arg2 and Arg3 are strings of different length
*		3. Arg2 or Arg3 are strings longer than 512 characters.
*		4. The arguments are not strings or integers
*
*
*	The common names for these functions are also defined:
*
*	NOT, AND, OR, XOR, NAND, NOR, UPLUS, UMINUS, UMUL, UDIV
*
*	Additional conversion functions included:
*	  HI(X)		hex digit string to integer
*	  IH(X)		integer to hex digit string
*	  DIB(X,B)	digit string to integer according to base B
*	  IDB(X,B)	integer to digit string according to base B
*
	TERMINAL = ~LOAD('LOGIC(INTEGER,ARG2,ARG3)')
+		   "Can't load LOGIC external function"	:S(END)

	DEFINE('LOGICDEF(PROTO)NAME,ARGS')	:(LOGICDEF_END)
LOGICDEF PROTO ? BREAK('(') . NAME  BAL . ARGS  =  NAME
	CODE(NAME  ' '  PROTO  '		:S(RETURN)F(FRETURN)')
	DEFINE(NAME  ARGS)			:(RETURN)
LOGICDEF_END
*
	LOGICDEF("NOT(X)        = LOGIC(1,X)")
	LOGICDEF("AND(X1,X2)    = LOGIC(2,X1,X2)")
	LOGICDEF("OR(X1,X2)     = LOGIC(3,X1,X2)")
	LOGICDEF("XOR(X1,X2)    = LOGIC(4,X1,X2)")
	LOGICDEF("NAND(X1,X2)   = LOGIC(5,X1,X2)")
	LOGICDEF("NOR(X1,X2)    = LOGIC(6,X1,X2)")
	LOGICDEF("UPLUS(X1,X2)  = LOGIC(7,X1,X2)")
	LOGICDEF("UMINUS(X1,X2) = LOGIC(8,X1,X2)")
	LOGICDEF("UMUL(X1,X2)   = LOGIC(9,X1,X2)")
	LOGICDEF("UDIV(X1,X2)   = LOGIC(10,X1,X2)")
	LOGICDEF("SHL(X1,X2)    = LOGIC(11,X1,X2)")
	LOGICDEF("SHR(X1,X2)    = LOGIC(12,X1,X2)")
	LOGICDEF("SAR(X1,X2)    = LOGIC(13,X1,X2)")
	LOGICDEF("ROL(X1,X2)    = LOGIC(14,X1,X2)")
	LOGICDEF("ROR(X1,X2)    = LOGIC(15,X1,X2)")
	LOGICDEF("HI(X)         = LOGIC(16,CONVERT(X,'STRING'))")
	LOGICDEF("IH(X)         = LOGIC(17,CONVERT(X,'INTEGER'))")
	LOGICDEF("DIB(X,B)      = LOGIC(16,CONVERT(X,'STRING'),B)")
	LOGICDEF("IDB(X,B)      = LOGIC(17,CONVERT(X,'INTEGER'),B)")
*
	UNLOAD("LOGICDEF")
