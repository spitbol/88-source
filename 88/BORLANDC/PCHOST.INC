* Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
* Use of this source code is governed by a BSD-style
* license that can be found in the LICENSE file.

* PCHOST()
*
* Function to use PCHOST instead of HOST to resolve different
* numbering system on Robert Dewar's PC-SPITBOL and Catspaw SPITBOLs.
*
* After including this file, or copying it into your program, just
* call PCHOST instead of HOST, using PC-SPITBOL host numbers, and
* they are mapped to Catspaw SPITBOL's numbering system if necessary.
*
* This is written in a rather tortuous way for one reason:
*   If used with 8086 PC-SPITBOL, some programs may already be
*   critically short of memory.  We only define the PCHOST
*   function if this is Catspaw SPITBOL.  Since there are no
*   labels, the entire code body is collectable under PC-SPITBOL.
*
* Acknowledgement:  Peter-Arno Coppen of Nimejen, The Netherlands
*  pointed out the need for this and provided this code.
*
* If 8086 PC-SPITBOL, merely OPSYN PCHOST to existing HOST function.
* Otherwise, define function PCHOST that performs this mapping of
* PC-SPITBOL HOST numbers to Catspaw SPITBOL HOST numbers:
*
*	1 --> 0		Command line parameters
*	2 --> 4		Fetch environment string
*	3 --> 200	Sound tone	(may not be implemented)
*	4 --> 201	Play music	(may not be implemented)
*
	(?(HOST() ? FENCE "IBM PC") OPSYN("PCHOST","HOST") ,
+	 ?CODE("PCHOST A1 = (GT(A1,4) A1,EQ(A1,1) 0,EQ(A1,2) 4 ,"
+					"EQ(A1,3) 200,201) ;"
+	      " PCHOST = HOST(A1,A2,A3,A4,A5) :S(RETURN)F(FRETURN)")
+	 DEFINE("PCHOST(A1,A2,A3,A4,A5)") )
