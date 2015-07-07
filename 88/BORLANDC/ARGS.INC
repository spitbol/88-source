*  Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
*  Use of this source code is governed by a BSD-style
*  license that can be found in the LICENSE file.

*  ARGS.INC - Copy user command line arguments into an array.
*
*  This is a C-like method of handling command-line arguments.
*
*  Include this file at the beginning of your prgram.
*
*  After the include, variable ARGC contains the number of user arguments.
*
*  Array ARGV contains the arguments.  ARGV[1] through ARGV[ARGC] contain
*  the command lines arguments.  ARGV[0] contains the program name.
*
*  This method works with both the compile-and-run method of execution,
*  and with load modules (EXE files).  For example, if your program
*  is PROCESS.SPT, then
*
*  SPITBOL PROCESS A B C
*
*  sets ARGC to 3, and ARGV has these values:
*
*	ARGV[0] = "PROCESS"
*	ARGV[1] = "A"
*	ARGV[2] = "B"
*	ARGV[3] = "C"
*
*

*  Get index of first user argument, then count number of arguments
*  by using HOST(2) until it fails.
	ARGC = HOST(3)
A1	ARGC = ?HOST(2, ARGC + 1) ARGC + 1	:S(A1)
	ARGC = ARGC - HOST(3) + 1
*
*  Now build the array with subscripts 0 to ARGC.
	ARGV = ARRAY("0:" ARGC)
	ARGV[0] = HOST(2,HOST(3) - 1)

*  Copy the user command line arguments into ARGV
	ARGC = -1
A2	ARGC = ARGC + 1
	ARGV[ARGC + 1] = HOST(2,HOST(3) + ARGC)	:S(A2)
