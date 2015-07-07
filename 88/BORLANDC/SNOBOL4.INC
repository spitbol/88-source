* Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
* Use of this source code is governed by a BSD-style
* license that can be found in the LICENSE file.

* SNOBOL4.INC
*
* Include file for SPITBOL to provide some SNOBOL4+ functions absent
* from SPITBOL.
*
* Contents:
*   AMP_PARM()		Equivalent to &PARM
*   ASC(S)		Ordinal value of ASCII character.
*   ENVIRONMENT(S)	Lookup environment variable S
*   EXECUTE(S1,S2)	Execute another program
*   FREEZE(T)		Dummy - not needed in SPITBOL
*   PATHNAME(UNIT)	Provide file name associated with UNIT.
*   REMOVE(S1,S2)	Remove characters in S2 from S1
*   SEEK(UNIT,N,I)	Set file position
*   SETBREAK(LABEL)	Emulated via SETEXIT.
*   STATEMENTS(N)	Implemented for N=0, error for N=1
*   TELL(UNIT)		Report file position
*   TRUNCATE(UNIT)	Not implemented - produce error message
*   VALUE(NAME)		Return value of object
*
* Skip entire file if not SPITBOL.
*
	DIFFER(.NAME,"NAME")				:F(SNOBOL4_INC_END)
*
*
* AMP_PARM() - Returns the entire command line (SNOBOL4+'s &PARM)
*
	DEFINE("AMP_PARM()I")				:(AMP_PARM_END)
AMP_PARM AMP_PARM = AMP_PARM " " HOST(2,I = I + 1)	:S(AMP_PARM)F(RETURN)
AMP_PARM_END
*
*
* ASC(S) returns ASCII value of first character of string S
*
	DEFINE('ASC(S)')				:(ASC_END)
ASC	&ALPHABET ? BREAK(*SUBSTR(S,1,1)) @ASC		:S(RETURN)F(FRETURN)
ASC_END
*
*
*
* ENVIRONMENT(S) returns string corresponding to environment variable S.
*
	DEFINE("ENVIRONMENT(S)")			:(ENVIRONMENT_END)
ENVIRONMENT ENVIRONMENT = HOST(4,S)			:S(RETURN)F(FRETURN)
ENVIRONMENT_END
*
*
* EXECUTE(S1,S2) execute another program.
*
	DEFINE("EXECUTE(S1,S2)")			:(EXECUTE_END)
EXECUTE	EXECUTE = HOST(1,S1,S2)				:S(RETURN)F(FRETURN)
EXECUTE_END
*
*
* FREEZE(T) is not needed in SPITBOL.
*
	DEFINE("FREEZE(T)")				:(FREEZE_END)
FREEZE							:(RETURN)
FREEZE_END
*
*
* PATHNAME(UNIT) returns the file name associated with an I/O unit.
*
*  Bug: Does not return the name of a file that has been specified on
*	the command line or via an environment variable.
*
	LOAD("PATHNAME(FILE)STRING")			:S(PATHNAME_END)
	DEFINE("PATHNAME(CHANNEL)")
	CODE("PATHNAME TERMINAL = 'PATHNAME external function "
+		"not loaded' :(FRETURN)")
PATHNAME_END
*
*
*
* REMOVE(S1, S2) returns the string S1 with the characters in S2 deleted.
*
* Operates in both anchored and unanchored mode.
*

	DEFINE("REMOVE(REMOVE,S)F")			:(REMOVE_END)
REMOVE	IDENT(S)					:S(RETURN)
	REMOVE ? FENCE BREAK(*S) . F SPAN(*S) = F	:S(REMOVE)F(RETURN)
REMOVE_END
*
* SAVE(UNIT) emulates the SNOBOL4+ save function.
*
	DEFINE("SAVE(UNIT)S")				:(SAVE_END)
SAVE	&ERRTYPE = ~(S = PATHNAME(UNIT)) 106    	:S(FRETURN)
	ENDFILE(UNIT)
	SAVE = (IDENT(EXIT(-4,S)) 1, INPUT(.SV_OUT,UNIT,S)) :(RETURN)
SAVE_END
*
*
* SEEK(UNIT, N, I) sets the position of a file.
*
	DEFINE("SEEK(UNIT,N,I)")			:(SEEK_END)
SEEK	SEEK = SET(UNIT,N,I)				:S(RETURN)F(FRETURN)
SEEK_END
*
*
* SETBREAK(LABEL) traps user interrupts.
*
* Note: This intercepts SETEXIT calls, and passes those not
* related to control-C to the original SETEXIT handler.  Thus is
* is not completely transparant, and if you subsequently call
* SETEXIT yourself, the SETBREAK trap will be lost.  If you are
* using SETEXIT, it is better to integrate the test for error 320
* into your SETEXIT code, rather than continuing to use SETBREAK.
*
	DEFINE("SETBREAK(LABEL)")
	SETBREAK_LAB =
	SETBREAK_EXIT =					:(SETBREAK_END)
SETBREAK SETBREAK_LAB = LABEL
	DIFFER(LABEL)					:S(SETBRK1)
* Here if turning SETBREAK off
	(DIFFER(SETBREAK_EXIT) SETEXIT(SETBREAK_EXIT))
	SETBREAK_EXIT =					:(RETURN)
* Here if turning SETBREAK on
SETBRK1	SETBREAK_EXIT = SETEXIT(.SETBRK2)
	&ERRLIMIT = &ERRLIMIT + 1			:(RETURN)
* Here when SETEXIT is fired.
SETBRK2	DIFFER(&ERRTYPE,320)				:S($SETBREAK_EXIT)
	&ERRLIMIT = &ERRLIMIT + 1
	SETEXIT(.SETBRK2)				:($SETBREAK_LAB)
SETBREAK_END
*
*
* STATEMENTS(N) returns real-valued &STCOUNT and &STFCOUNT.
*
	DEFINE("STATEMENTS(N)")				:(STATEMENTS_END)
STATEMENTS STATEMENTS = EQ(N,0) CONVERT(&STCOUNT,"REAL") :S(RETURN)
	TERMINAL = "STATEMENTS(" N ") NOT AVAILABLE IN SPITBOL" :(FRETURN)
STATEMENTS_END
*
*
*
* TELL(CHANNEL) returns the position of a file.
*
	DEFINE("TELL(UNIT)")				:(TELL_END)
TELL	TELL = SET(UNIT,0,1)				:S(RETURN)F(FRETURN)
TELL_END
*
*
* THAW(TABLE) is not needed in SPITBOL.
*
	DEFINE("THAW(T)")				:(THAW_END)
THAW							:(RETURN)
THAW_END
*
*
* TRUNCATE(UNIT) not implemented in SPITBOL.
*
	DEFINE("TRUNCATE(UNIT)")			:(TRUNCATE_END)
TRUNCATE TERMINAL = "TRUNCATE(" UNIT ") NOT IMPLEMENTED" :(FRETURN)
TRUNCATE_END
*
*
* VALUE(NAME) returns the contents of NAME.
*
	DEFINE("VALUE(NAME)")				:(VALUE_END)
VALUE	VALUE = $NAME					:(RETURN)
VALUE_END
*
* ---------------------  end of SNOBOL4.INC ------------------------
*
SNOBOL4_INC_END

