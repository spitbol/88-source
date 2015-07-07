/*
 Copyright (C)  1988-1998  Mark Emmer

 SPITBOL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
/	File:  PROMPT.C		Version:  01.00
/	---------------------------------------
/
/	Contents:	Function prompt
*/

#include "port.h"

/*	prompt() - used to give user usage info in command line versions.
 *
 */
void prompt()
{
#if RUNTIME
	wrterr("usage: spitrun [options] file[.spx] [program arguments]");
#else					/* RUNTIME */

#if SAVEFILE
	wrterr("usage: spitbol [options] files[.spt or .spx] [args to HOST(2)]");
#else					/* SAVEFILE */
	wrterr("usage: spitbol [options] files[.spt] [args to HOST(2)]");
#endif					/* SAVEFILE */

#endif					/* RUNTIME */

#if RUNTIME
#if MSDOS | OS2
   wrterr("options (use - or / to specify):   (# is a decimal number)");
#elif WINNT
   wrterr("options (use - or / to specify):   (# is a decimal number)");
#else             /* MSDOS | OS2 | WINNT */
	wrterr("options: (# is a decimal number)");
#endif               /* MSDOS | OS2 | WINNT */
  wrterr("-u \"string\" data string available to program");
#if SUN3
		wrterr("-v skip O/S vadvise calls");
#endif					/* SUN */
		wrterr("-#=file   associate file with I/O channel #");
#else					/* RUNTIME */
	wrterr("source files are concatenated, filename '-' is standard input/output");
#if BCC16
	wrterr("# is a decimal number.  Append \"k\" for kilobytes.");
#else					/* BCC16 */
	wrterr("# is a decimal number.  Append \"k\" for kilobytes, \"m\" for megabytes.");
#endif					/* BCC16 */
#if MSDOS | OS2
   wrterr("options (use - or / to specify):   (# is a decimal number)");
#elif WINNT
   wrterr("options (use - or /  to specify):");
#else             /* MSDOS | OS2 | WINNT */
	wrterr("options:");
#endif               /* MSDOS | OS2 | WINNT */
#if BCC16
	wrterr("-i# #bytes heap size");
#else					/* BCC16 */
	wrterr("-d# #bytes max heap            -i# #bytes initial heap size & enlarge amount");
#endif
	wrterr("-m# #bytes max object size     -s# #bytes stack size");
	wrterr("-c compiler statistics         -x execution statistics");
	wrterr("-a same as -lcx                -l normal listing");
	wrterr("-p listing with wide titles    -z listing with form feeds");
   wrterr("-o=file[.lst]  listing file    -h suppress version ID/date in listing");
	wrterr("-g# lines per page             -t# line width in characters");
	wrterr("-b suppress signon message     -e errors to list file only");
	wrterr("-k run with compilation error  -n suppress execution");
  wrterr("-f no case-folding             -u \"string\" data passed to HOST(0)");

#if SUN3
	wrterr("-v skip O/S vadvise calls");
#endif					/* SUN3 */

#if EXECFILE & SAVEFILE
#if MSDOS | OS2 | WINNT
	wrterr("-w write execution (.exe) file -y write save (.spx) file");
#else             /* MSDOS | OS2 | WINNT */
	wrterr("-w write load (.out) module    -y write save (.spx) file");
#endif               /* MSDOS | OS2 | WINNT */
#endif					/* EXECFILE & SAVEFILE */

#if SAVEFILE & !EXECFILE
	wrterr("-y write save (.spx) file");
#endif					/* SAVEFILE & !EXECFILE */

		wrterr("-r INPUT from source file following END statement");
        wrterr("-T=file  write TERMINAL output to file");
        wrterr("-#=file[options]  associate file with I/O channel #");
#if VAX
		wrterr("option defaults: -d1m -i16k -m64k -s8k -g60 -t120");
#endif					/* VAX */

#if HIGHC | INTEL | BCC32 | VCC | OS2 | SUN4 | LINUX | WINNT
    wrterr("option defaults: -d64m -i128k -m4m -s128k -g60 -t120");
#endif					/* HIGHC | INTEL | OS2 | SUN4 | LINUX */

#if BCC16
		wrterr("option defaults: -i56k -m9000 -s2830 -g60 -t120");
#endif					/* BCC16 */

#if !HIGHC & !INTEL & !VAX & !BCC16 & !OS2 & !SUN4 & !LINUX & !WINNT
    wrterr("option defaults: -d64m -i128k -m64k -s128k -g60 -t120");
#endif					/* !HIGHC & !INTEL & !VAX & !BCC16 & !OS2 */

#endif					/* RUNTIME */

	wrterr("");
	wrterr("SPITBOL is not a public domain program.  For licensing information contact:");
	wrterr("Catspaw, Inc., P.O. Box 1123, Salida, CO 81201 USA  719-539-3884  FAX:539-4830");
   wrterr("e-mail: sales@SNOBOL4.com   http://www.SNOBOL4.com");

	__exit(0);
}
