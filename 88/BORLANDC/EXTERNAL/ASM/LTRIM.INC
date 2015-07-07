* Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
* Use of this source code is governed by a BSD-style
* license that can be found in the LICENSE file.

* LTRIM.INC - perform a TRIM on the left end of a string
*
*
*   Usage:
*	Result = LTRIM(S1)
*   or  Result = LTRIM(S1,S2)                                   
*
*   Where:
*	S1 = String to trim
*	S2 = optional one-character string with character to trim
*	
*	If S2 is omitted or null, then blanks and tab characters are
*	trimmed from the beginning of S1.
*
*	If S2 is longer than 1 character, only the first character
*	is used.
*
*   Output:
*	Left trimmed copy of first argument
*
	TERMINAL = ~LOAD("LTRIM(STRING,STRING)STRING")
+		"Cannot load LTRIM external function"	:S(END)
