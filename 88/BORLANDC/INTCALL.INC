*  Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
*  Use of this source code is governed by a BSD-style
*  license that can be found in the LICENSE file.

*  INTCALL.INC - 8088 version
*
*  Include file to allow the user's program to call an MS-DOS or BIOS
*  software interrupt.
*
*  INTCALL is called with an interrupt number and up to nine arguments which
*  provide the machine registers identified in the function prototype, below.
*  Null or omitted arguments will cause the previous value used for that
*  register to be retained (initial value 0).
*
*  Arguments should either be integers, or of a data type that can be
*  converted to an integer.
*
*  The function returns a ten-element array containing the registers
*  resulting from the interrupt call.  The variables R_xx provide
*  handy indices into this array.  Note that the R_FLG element of the array
*  does not appear in the incoming argument list, but is returned in
*  the register array.
*
*  For implementation reasons, it is necessary to create a copy of each
*  array entry prior to making the host call.  This allows the HOST function
*  to directly modify an integer block in memory without damaging any other
*  variables that might be pointing to this value.
*

	DEFINE("INTCALL(N,AX,BX,CX,DX,SI,DI,BP,DS,ES)")

	REGISTERS = ARRAY(10,0)
	    R_AX = 1
	    R_BX = 2
	    R_CX = 3
	    R_DX = 4
	    R_SI = 5
	    R_DI = 6
	    R_BP = 7
	    R_DS  = 8
	    R_ES  = 9
	    R_FLG = 10					:(INTCALL_END)

INTCALL
   REGISTERS<R_AX> = COPY((DIFFER(AX) CONVERT(AX,"INTEGER"), REGISTERS<R_AX>))
   REGISTERS<R_BX> = COPY((DIFFER(BX) CONVERT(BX,"INTEGER"), REGISTERS<R_BX>))
   REGISTERS<R_CX> = COPY((DIFFER(CX) CONVERT(CX,"INTEGER"), REGISTERS<R_CX>))
   REGISTERS<R_DX> = COPY((DIFFER(DX) CONVERT(DX,"INTEGER"), REGISTERS<R_DX>))
   REGISTERS<R_SI> = COPY((DIFFER(SI) CONVERT(SI,"INTEGER"), REGISTERS<R_SI>))
   REGISTERS<R_DI> = COPY((DIFFER(DI) CONVERT(DI,"INTEGER"), REGISTERS<R_DI>))
   REGISTERS<R_BP> = COPY((DIFFER(BP) CONVERT(BP,"INTEGER"), REGISTERS<R_BP>))
   REGISTERS<R_DS>  = COPY((DIFFER(DS)  CONVERT(DS,"INTEGER"),  REGISTERS<R_DS>))
   REGISTERS<R_ES>  = COPY((DIFFER(ES)  CONVERT(ES,"INTEGER"),  REGISTERS<R_ES>))
   REGISTERS<R_FLG> = COPY(REGISTERS<R_FLG>)
   INTCALL = HOST(210,CONVERT(N,"INTEGER"),REGISTERS)			:S(RETURN)F(FRETURN)
INTCALL_END
