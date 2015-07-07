/*
 Copyright 1991 Robert Goldberg and Catspaw, Inc.

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
/	File:  MAIN.C		Version:  01.00
/	---------------------------------------
/
/	Contents:	Function main
*/


/*
/	This module contains the main function that gets control when
/	the spitbol compiler starts execution.  Responsibilities of
/	this function:
/
/	o  Save argc and argv parameters in global storage.
/
/	o  Determine if this execution reflects the invocation of
/	   of the compiler or of a load module and take appropriate
/	   actions.
/
/	   If invoked as compiler:  process command line arguments,
/	   set up input files, output file, initial memory allocation,
/	   and transfer control to compiler
/
/	   If invoked as load module:  reset various compiler variables
/	   whose values are no longer valid, re-establish dynamic area
/	   and transfer control to function that returns control to
/	   suspended spitbol program
/
/	HISTORY
/
/  V1.00 04-Jun-92 Split off from OSINT as a front-end module.
/  V1.01 30-Dec-96 Call swcinp after reloading SPX file.
/  V1.02 18-Mar-00 Don't interpret parameters following .spx on command
/                  line as file names.
*/
#define GLOBALS			/* global variables will be defined in this module */
#include "port.h"

#ifdef DEBUG
#undef DEBUG			/* Change simple -DDEBUG on command line to -DDEBUG=1 */
#define DEBUG 1
#else
#define DEBUG 0
#endif

#if DEBUG && (INTEL | BCC16)
#include <dos.h>
#if INTEL
unsigned int    _rotl(unsigned int, int);
#define _rotl(_a,_b)   _word_rol((unsigned int)_a,(unsigned int)_b)
#pragma _builtin_("_word_rol"==78)
unsigned int    _word_rol(unsigned int, unsigned int);
#endif					/* INTEL */
#if BCC16
#include <stdlib.h>
#endif					/* BCC16 */
#endif					/* DEBUG */

#if HIGHC
#include <string.h>
pragma On(PCC_msgs);
#endif					/* HIGHC */

#if INTEL | BCC16
#include <string.h>
#endif					/* INTEL | BCC16 */

#if INTEL
#include <extender.h>
#include "save.h"
#endif					/* INTEL */

#if MSDOS & (BCC16 | BCC32 | VCC)
#include <dos.h>
#endif

void setout Params(( void ));


main( argc, argv )
int	argc;
char	*argv[];

{
    int		i;

/*
/   Save command line parameters in global storage, in case they are needed
/   later.
*/
    gblargc = argc;
    gblargv = argv;

#if (INTEL | BCC32 | VCC | WINNT)
	init_custom();				/* Perform system specific initializations */
#endif					/* INTEL */

/*
/	Initialize buffers
*/
	stdioinit();
	ttyinit();
/*
/	Make sure sysdc gets to output stuff at least once.
*/
	dcdone = 0;

#if EXECFILE
#if EXECSAVE
	/* On some platforms we implement execfiles differently.  A save file
	 * is appended to the SPITBOL executable file, and will be read in
	 * just like a save file specified on the command line.
	 *
	 * We have to check for the presence of a save file in the executable.
	 */
	i = checksave(gblargv[0]);
	if (i) {
		inpptr = gblargv;
		if (getsave(i) != 1)
			__exit(1);
		close(i);

		/* set up things that normally would be retained in the
		 * a Unix exec file.
		 */
#if USEFD0FD1
		originp = dup(0);
#endif					/* USEFD0FD1 */
		readshell0 = 0;
#else					/* EXECSAVE */
	/*
	/   If this is a restart of this program from a load module, set things
	/   up for a restart.  Transfer control to function restart which actually
	/   resumes program execution.
	*/
    if ( lmodstk ) {
		if ( brk( (char *) topmem ) < 0 ) { /* restore topmem to its prior state	*/
	    	wrterr( "Insufficient memory to load." );
	    	__exit(1);
			}
#endif					/* EXECSAVE */

#if VAX
		setsigs();				/* set signals				*/
#endif							/* VAX */

		cmdcnt = 1;				/* allow access to command line args	*/
		inpptr = 0;				/* no compilation input files		*/
		inpcnt = 0;				/* ditto				*/
		outptr = 0;				/* no compilation output file		*/
		pathptr = (char *)-1L;	/* include paths unknown 	*/
		clrbuf();				/* no chars left in std input buffer	*/
		sfn = 0;
#if FLOAT
		hasfpu = checkfpu();	/* check for floating point hardware */
#endif					/* FLOAT */
#if (SUN4 | LINUX) & !EXECSAVE
		heapmove();				/* move the heap up					*/
		malloc_empty();			/* mark the malloc region as empty	*/
#endif					/* SUN4 | LINUX */
		zysdc();							/* Brag if necessary */
        restart( (char *)0L, lowsp );       /* call restart to continue execution */
	    }
#endif					/* EXECFILE */

#if BCC16
	/* Figure out the max permissible values for memincb and databts prior to getargs call */
	memincb = databts = 0xFFFE - FP_OFF(GET_DATA_OFFSET(END_MIN_DATA, void *)) - stacksiz;
#endif					/* BCC16 */

/*
 * 	Process command line arguments
 */
	inpptr = getargs(argc, argv);

    if ( inpptr )
        sfn = *inpptr;		/* pointer to first file name */
	else
   {
        zysdc();
        wrterr("");
        prompt();
   }

/*
/   Switch to proper input file.
*/
    swcinp( inpcnt, inpptr );

#if FLOAT
/*
 * test if floating point hardware present
 */
    hasfpu = checkfpu();
#endif					/* FLOAT */

#if VAX
/*
/   Set signals for execution.
*/
    setsigs();
#endif					/* VAX */

#if SAVEFILE | EXECSAVE
	switch (getsave(getrdfd())) {
		case 1:					/* save file loaded */
            inpcnt = 0;               /* v1.02 no more cmd line files */
            swcinp(inpcnt, inpptr );  /* v1.01 */
            restart( (char *)0L, lowsp );

		case 0:					/* not a save file */
#if RUNTIME
			wrterr("SPITBOL save (.spx) file only!");
#else					/* RUNTIME */
			break;
#endif					/* RUNTIME */

		case -1:				/* error loading save file */
			__exit(1);
		}
#endif					/* SAVEFILE | EXECSAVE */

/*
/	Setup output and issue brag message
*/
    setout();

#if !RUNTIME

/*
 *	Force the memory manager to initialize itself
 */
	if ((char *)sbrk(0) == (char *)-1) {
		wrterr( "Insufficient memory.  Try smaller -d, -m, or -s command line options." );
#if MSDOS & BCC32
		wrterr( "If using QEMM, disable QDPMI with 'QDPMI OFF' command." );
#endif
		__exit( 1 );
	    }

#if HIGHC | INTEL | OS2 | BCC32 | VCC
/*
/   Allocate stack
*/
    if ((lowsp = sbrk((uword)stacksiz)) == (char *) -1) {
		wrterr( "Stack memory unavailable." );
		__exit( 1 );
	    }
#endif					/* HIGHC | INTEL | OS2 */

/*
/   Allocate initial increment of dynamic memory.
/
*/
#if BCC16
    basemem = (char *)sbrk(0);
	memincb = databts = topmem - basemem;
#endif					/* BCC16 */

#if SUN4
	/* Allocate a buffer for mallocs.  Use the space between the
	 * end of data and the start of Minimal's static and dynamic
	 * area.  Because of virtual memory, we can use almost 4 megabytes
	 * for this region, and it has the secondary benefit of letting
	 * us have object sizes greater than the previous 64K.
	 */
	if (malloc_init( maxsize )) {
		wrterr( "Malloc initialization failure, contact Catspaw." );
		__exit( 1 );
		}
#endif          /* SUN4 */

    if ((basemem = (char *)sbrk((uword)memincb)) == (char *) -1) {
		wrterr( "Workspace memory unavailable." );
		__exit( 1 );
    	}
    topmem = basemem + memincb;
    maxmem = basemem + databts;


/*
/   All compiler registers are initially zero, except for XL and XR which
/   are set to top and bottom of heap.
*/
    SET_CP( 0 );
    SET_IA( 0 );
    SET_WA( 0 );
    SET_WB( 0 );
    SET_WC( 0 );
    SET_XR( basemem );
    SET_XL( topmem - sizeof(word) );

/*
/   Startup compiler.
*/
    startup( (char *)0L, lowsp );
#endif					/* !RUNTIME */

/*
/   Never returns. exit is via exit().
*/
}



/*
/	wrterr( s )
/
/	Write message to standard error, and append end-of-line.
*/
void wrterr(s)
char	*s;
{
#if EOL2
	static char eol[2] = {EOL1,EOL2};
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#else					/* EOL2 */
	static char eol[1] = {EOL1};
#endif					/* EOL2 */
	write( STDERRFD, s, length(s) );
	write( STDERRFD,  eol, sizeof(eol) );
}

/*
 * Setup output file.
 * Issue brag message if approriate
 *
 * This rather clumsy routine was placed here because of sequencing
 * concerns -- it can't be called with a save file until spitflag
 * has been reloaded.
 */
void setout()
{
/*
 *	Brag prior to calling swcoup
 */
	zysdc();

/*
/   Switch to proper output file.
*/
    swcoup( outptr );

/*
/   Determine if standard output is a tty or not, and if it is be sure to
/   inform compiler and turn off header.
*/
    spitflag &= ~PRTICH;
    if ( testty( getprfd() ) == 0 )
    {
		lnsppage = 0;
		spitflag |= (PRTICH | NOHEDR);
    }
}


#if DEBUG && (INTEL | BCC16)
void strout(char *s)
{
	union REGS rin, rout;
	char c;

	while ((c = *s++) != 0) {
		rin.h.ah = 2;
		rin.h.dl = c;
		intdos(&rin, &rout);
		}
}

void numout(unsigned n)
{
	union REGS rin, rout;
	char c;
	int i;
	for (i=sizeof(n)*2; i--;) {
		n = _rotl(n, 4);
		c = (n & 0xF) + '0';
		if (c > '9')
			c += 'A' - '9' - 1;
		rin.h.ah = 2;
		rin.h.dl = c;
		intdos(&rin, &rout);
		}
}

void crlf()
{
	strout("\r\n");
}
#endif					/* DEBUG */

