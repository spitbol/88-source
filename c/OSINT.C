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
/	o  Save argc and argv parameters in global storage
/
/	o  Determine if this execution reflects the invocation of
/	   of the compiler or of a load module and take appropriate
/	   actions
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
*/
#define GLOBALS
#include "port.h"

#if DEBUG
#include <dos.h>
#if INTEL
/* Debugging Stuff for Intel */
unsigned int    _rotl(unsigned int, int);
#define _rotl(_a,_b)   _word_rol((unsigned int)_a,(unsigned int)_b)
#pragma _builtin_("_word_rol"==78)
unsigned int    _word_rol(unsigned int, unsigned int);
#endif					/* INTEL */
#if BCC16 | BCC32 | VCC
#include <stdlib.h>
#endif
#endif					/* DEBUG */

#if HIGHC
#include <string.h>
pragma On(PCC_msgs);
#endif					/* HIGHC */

#if INTEL
#include <string.h>
#endif					/* INTEL */

#if BCC16 | BCC32 | VCC
#include <string.h>
#endif					/* BCC16 */


#if INTEL
#include <extender.h>
#include "save.h"
#endif					/* INTEL */


void setout Params(( void ));


void main( argc, argv )
int	argc;
char	*argv[];

{
    int		i;
    register char *cp, *p;

/*
/   Save command line parameters in global storage, in case they are needed
/   later.
*/
    gblargc = argc;
    gblargv = argv;

/*
/	Initialize buffers
*/
	stdioinit();
	ttyinit();
/*
/	Make sure sysdc gets to output stuff at least once.
*/
	dcdone = 0;

#if INTEL
	init_custom();				/* Perform system specific initializations */
#endif					/* INTEL */

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
		originp = dup(0);
		readshell0 = 0;
#else					/* EXECSAVE */
	/*
	/   If this is a restart of this program from a load module, set things
	/   up for a restart.  Transfer control to function restart which actually
	/   resumes program execution.
	*/
    if ( lmodstk ) {
		if ( brk( (char *) topmem ) < 0 ) { /* restore topmem to its prior state	*/
	    	wrterr( "Insufficient memory to load" );
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

		zysdc();							/* Brag if necessary */
		restart( (char *)0L, lowsp );		/* call restart to continue execution	*/
	    }
#endif					/* EXECFILE */

/*
/   Here we are for a normal startup of the compiler.  Process all
/   command line options.  NOTE:  the value of the loop control variable
/   i is modified within the loop.
/
/   spitbol is invoked as
/
/	spitbol [options] [input-files]
/
/   where each option string begins with a '-' (or '/' for MSDOS & OS/2)
/
/   A single '-' represents the standard file provided by the shell
*   and is treated as an input-file or output file as appropriate.
*/

/*  Establish default line width and page depth prior to examining
/   arguments.  (Previous values undisturbed on lmodstk restart.)  V1.08
*/
	lnsppage = PAGE_DEPTH;	/*  lines per page for listings		*/
	pagewdth = PAGE_WIDTH;	/*  width of output line for listings	*/

    for( i = 1 ; i < argc ; i++ )
    {
	cp = argv[i];		/* point to next cmd line argument	*/

	/*
	/   If this command line argument does not start with a '-
	/   OR is a single '-', treat is as the first filename.
	*/
#if MSDOS | OS2 | WINNT
	if ( (*cp != '-' && *cp != '/')  ||  (*cp == '-' && !cp[1]) )
#else             /* MSDOS | OS2 | WINNT */
	if ( *cp != '-'  ||  !cp[1] )
#endif               /* MSDOS | OS2 | WINNT */
	{
	    if ( !inpptr )
	        inpptr = argv + i;	/* inpptr -> first filename pointer	*/
	    break;		/* break out of for loop		*/
	}

	/*
	/   Here to process option string.  Allow more than one option
	/   to be specified after the '-'.  For example, "-aez" and
	/    "-s24kae"
	*/
	++cp;
	while( *cp )
	    switch( *cp++ )
	    {
		/*
		/	-?  display option summary
		*/
		case '?':
			prompt();
			break;
#if !RUNTIME
	    /*
	    /   -a	turn on all listing options except header
	    */
	    case 'a':
		spitflag &= ~(NOLIST | NOCMPS | NOEXCS);
		break;

		/*
	    /   -b	suppress signon message when reloading save file
	    */
	    case 'b':
		spitflag |= NOBRAG;
		break;

	    /*
	    /   -c	turn on compilation statistics
	    */
	    case 'c':
		spitflag &= ~NOCMPS;
		break;

#if !BCC16
		/*
	    /   -dnnn	set maximum size of dynamic area in bytes
	    */
	    case 'd':
		cp = optnum( cp, &databts );
		break;
#endif					/* !BCC16 */

	    /*
	    /   -e don't send errors to terminal
	    */
	    case 'e':
		spitflag &= ~ERRORS;
		break;

	    /*
	    /   -f	don't fold lower case to upper case
	    */
	    case 'f':
		spitflag &= ~CASFLD;
		break;

	    /*
	    /   -gddd	set page length in lines  V1.08
	    */
	    case 'g':
		cp = getnum( cp, &lnsppage );
		break;

	    /*
	    /   -h	suppress version header in listing
	    */
	    case 'h':
		spitflag |= NOHEDR;
		break;

	    /*
	    /   -iddd	set memory expansion increment (bytes)
	    */
	    case 'i':
		cp = optnum( cp, &memincb );
		break;

	    /*
	    /   -k	run inspite of compilation errors
	    */
	    case 'k':
		spitflag &= ~NOERRO;
		break;

	    /*
	    /   -l	turn on compilation listing
	    */
	    case 'l':
		spitflag &= ~NOLIST;
		break;

	    /*
	    /   -mddd	set maximum size of object in dynamic area
	    */
	    case 'm':
		cp = optnum( cp, &maxsize );
		break;

	    /*
	    /   -n	suppress program execution
	    */
	    case 'n':
		spitflag |= NOEXEC;
		break;

	    /*
	    /   -o fff	set output file to fff
	    /   -o:fff & -o=fff also allowed.
	    */
	    case 'o':
		if ( *cp == ':' || *cp == '=' )
		{
			if (*(cp + 1))
			{
				outptr = ++cp;
				while (*++cp)
					;
			}
			else
				goto badopt;
		}
		else
		{
			outptr = argv[++i];
			if ( i == argc || (*outptr == '-' && outptr[1] != '\0')
#if MSDOS | OS2 | WINNT
			 || (*outptr == '/')
#endif               /* MSDOS | OS2 | WINNT */
					)
		    		goto badopt;	/* V1.08 */
		}
		break;

	    /*
	    /   -p	turn on long listing format
	    */
	    case 'p':
		spitflag |= LNGLST;
		spitflag &= ~NOLIST;
		break;

	    /*
	    /   -r	read INPUT from source program file
	    */
	    case 'r':
		readshell0 = 0;
		break;

	    /*
	    /   -s	set stack size in bytes
	    */
	    case 's':
#if BCC16
        {word oldss = stacksiz;
#endif
		cp = optnum( cp, &stacksiz );
#if BCC16
		/* additional stack space comes at the expense of memincb */
        memincb -= (stacksiz - oldss);
		/* additional stack space lets us enlarge &MAXLNGTH */
		maxsize += (stacksiz - oldss);
	    }
#endif
		break;

	    /*
	    /   -tddd	set line width in characters  V1.08
	    */
	    case 't':
		cp = getnum( cp, &pagewdth );
		break;
#endif					/* !RUNTIME */

	    /*
	    /   -u aaa	set user argument accessible via host()
	    */
	    case 'u':
		uarg = argv[++i];
		if ( i == argc )
		    goto badopt;	/* V1.08 */
		break;


#if SUN3
	    /*
	    /   -v	suppress vadvise calls from sysgc
	    */
	    case 'v':
		spitflag |= NOVADV;
		break;
#endif					/* SUN3 */

#if !RUNTIME
#if EXECFILE
	    /*
	    /   -w	write executable module after compilation
	    */
	    case 'w':
		spitflag |= WRTEXE;
		break;

#endif					/* EXECFILE */

	    /*
	    /   -x	print execution statistics
	    */
	    case 'x':
		spitflag &= ~NOEXCS;
		break;

#if SAVEFILE
	    /*
	    /   -y	write executable module after compilation
	    */
	    case 'y':
		spitflag |= WRTSAV;
		break;
#endif					/* SAVEFILE */

	    /*
	    /   -z	turn on standard listing options
	    */
	    case 'z':
		spitflag |= STDLST;
		spitflag &= ~NOLIST;
		break;
#endif					/* !RUNTIME */

	    /*
	    / -# fff	associate file fff with channel #
	    */
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		if (++maxf >= Ncmdf)
		{
		   wrterr( "Too many files on command line" );
		   __exit(1);
		}
		cp = getnum(cp - 1, &(cfiles[maxf].filenum));
		if ( *cp == ':' || *cp == '=' )
		{
			if (*(cp + 1))
			{
				cfiles[maxf].fileptr = ++cp;
				while (*++cp)
					;
			}
			else
				goto badopt;
		}
		else
		{
			p = cfiles[maxf].fileptr = argv[++i];
			if ( i == argc   ||  (*p == '-' && p[1] != '\0')
#if MSDOS | OS2 | WINNT
			 || (*p == '/')
#endif               /* MSDOS | OS2 | WINNT */
				 )
				goto badopt;	/* V1.08 */
		}
		break;

	    /*
	    /   anything else is an error
	    */
	    default:
badopt:		write( STDERRFD, "Illegal option -", 17 );
		write( STDERRFD,  (cp - 1), 1 );
		wrterr( "?" );
		__exit(1);			/* V1.08 */
	    }
    }

    inpcnt = argc - i;	/* inpcnt =  number of filenames	*/
    if ( inpptr )
        sfn = *inpptr;		/* pointer to first file name */

/*
/   Establish command counter for use by HOST(3) function
*/
    cmdcnt = i;

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

#if HIGHC | INTEL | OS2 | WINNT
/*
/   Allocate stack
*/
    if ((lowsp = sbrk((uword)stacksiz)) == (char *) -1) {
		wrterr( "Stack memory unavailable" );
		__exit( 1 );
	    }
#if 0
	/* Store into all newly allocated stack pages to commit them. */
	for (cp = lowsp; cp < lowsp+stacksiz; ) {
		*cp = 0;
		/* Advance to beginning of next hardware page */
		cp = (char *)(((unsigned)cp + 4096) & -4096);
		}
#endif
#endif					/* HIGHC | INTEL | OS2 */

/*
/   Allocate initial increment of dynamic memory.
/
*/
#if BCC16
    basemem = (char *)sbrk(0);
	memincb = databts = topmem - basemem;
#endif					/* BCC16 */
    if ((basemem = sbrk((uword)memincb)) == (char *) -1) {
		wrterr( "Workspace memory unavailable" );
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
}


/*
/    getnum() converts an ASCII string to an integer AND returns a pointer
/    to the character following the last valid digit.
/
/    Parameters:
/	cp	pointer to character string
/	ip	pointer to word receiving converted result
/    Returns:
/	Pointer to character following last valid digit in input string
/    Side Effects:
/	Modifies contents of integer pointed to by ip.
*/

char	*getnum( cp, ip )

char	*cp;
word	*ip;

{
    word	result = 0;

    while( *cp >= '0'  &&  *cp <= '9' )
	result = result * 10 + *cp++ - '0';

    *ip = result;
    return  cp;
}


/*
/   optnum() converts an ASCII string to an integer AND returns a pointer
/   to the character following the last valid digit.  optnum() is similar
/   to getnum() except that optnum accepts a trailing 'k' or 'm' to indicate
/   that the value should be scaled in units of 1,024 or 1,048,576.
/
/   Parameters:
/	cp	pointer to character string
/	ip	pointer to word receiving converted result
/   Returns:
/	Pointer to character following last valid digit in input string,
/	including a trailing k.
/   Side Effects:
/	Modifies contents of integer pointed to by ip.
*/

char	*optnum( cp, ip )

char	*cp;
word	*ip;

{
    cp = getnum( cp, ip );

    if ( *cp == 'k' )
    {
	++cp;
	*ip <<= 10;
    }

    if ( *cp == 'm' )
    {
	++cp;
	*ip <<= 20;
    }

    return  cp;
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


#if DEBUG
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

