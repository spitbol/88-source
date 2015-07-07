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
/	File:  COMPILE.C		Version:  01.01
/	---------------------------------------
/
/	Contents:	Function compile
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
/   Overview of header files used by interface:
/
/	osint.h	    contains items necessary for the interfacing of
/		    the C language interface to the assembly language
/		    compiler
/
/	port.h	    contains system dependent items likely to be changed
/		    when porting interface to another system.  [Contains
/		    compiler flag definitions.]
/
/	spitblks.h  contains structure definitions for blocks created
/		    by spitbol within it's dynamic area.  [Contains
/		    typedef for word!]
/
/	spitio.h    contains structure definitions for blocks created
/		    by the OS interface necessary to perform file I/O
/
*/

#include "port.h"
#if APPLE
#include "compiler.h"
#include "PEdit.h"
#include <fcntl.h>

#define KVCOM (*kvcom_ptr)
#define KVDMP (*kvdmp_ptr)
#define KVFTR (*kvftr_ptr)
#define KVPFL (*kvpfl_ptr)
#define CSWFL (*cswfl_ptr)
#define R_FCB (*fcb_ptr)
/*
/   Functions and variables required by this module.
*/
extern struct chfcb *R_FCB;

/*
/    Global data areas needed by compiler.
*/
jmp_buf	return_jump;
int	cmdcnt;				/*  command count								*/
int	gblargc;			/*  argc from command line						*/
char	**gblargv;		/*  argv from command line						*/
char	*uarg;			/*  -u argument from command line				*/
long	start_time;		/*  starting execution time (1/60th seconds)	*/


/*
/   Information to be given to compiler
*/
word	lnsppage;		/*  lines per page for listings					*/
word	pagewdth;		/*  width of output line for listings			*/
word	spitflag;		/*  flags to be given to compiler				*/

/*
/   Memory variables that control compiler's dynamic area and stack.
*/
word	memincb;		/*  meminc converted to bytes					*/
char	*basemem;		/*  base of dynamic memory						*/
char	*topmem;		/*  current top of dynamic memory				*/
char	*maxmem;		/*  maximum top of dynamic memory				*/
word	maxsize;		/*  maximum size element in dyn. memory			*/
word	stacksiz;		/*  maximum size of stack in bytes				*/
char	*initsp;		/*  initial sp value							*/
Handle	comp_handle = NULL;

/*
/   Variables that describe access to standard input and output files.
*/
int		inpcnt;			/*  number of input files						*/
char	**inpptr;		/*  pointer to input file in argv array			*/
char	*outptr;		/*  pointer to output listing file	(not used)	*/
char    *sfn;			/*  current source file name 					*/
int		readshell0;		/*  interlock default reading of fd 0 (not used)*/
int     first_record;	/*  non-zero before first record read			*/
int		executing;		/*  flag compilation or execution				*/
int		originp;		/*  dup of original fd 0						*/
int		justended;		/*  END trigger for zysrd (not used)			*/
int		curfile;		/* current file position (swcinp)				*/
int		errflag;		/* Nonzero if error in swcoup					*/
int		origoup;		/* Dup of shell's std output (swcoup)			*/
int		oupState;		/* Current state of swcoup						*/
int 	nesting;		/*  depth of include file nesting		  		*/
int		provide_name;	/* instructs sysrd to deliver filename to caller*/
int		read_following;	/* Attach stdin to source file after END stmt	*/


/*
/   lmodstk is set when creating a load module.  On the subsequent
/   execution of a load module, the presence of a non-zero value in
/   lmodstk determines that the execution is indeed of a load module.
*/
char	*lmodstk;

/************************************** EXTERNALS *************************/
extern CompResult	compResult;			/* return values from compiler */
#ifdef ExternalFunctions
extern	void		unloadXF(void);
#endif
extern void			whereami(struct scblk **fname, word *line);
extern WindInfoPtr runWptr;
extern char *StkLowPt : 0x110;	 /* Lowest stack as measured in VBL task [pointer] */
extern word KVCOM;
extern word KVDMP;
extern word KVFTR;
extern word KVPFL;
extern word CSWFL;

/********************************* STATICS ************************************/
/* phases of compiler */
enum {
   translate = 0,
   initialize,
   execution,
   nphases
   };

static int phase = -1;
static int errorcodes[nphases] = {CompErr, InitialErr, ExeErr};
static long		comp_size = -1;
static Ptr		comp_ptr = NULL;
static Handle	heap_handle;
static Handle	stack_handle;
static char 	*saveStkLowPt;


/****************************** FORWARD REFERENCES *********************************/
static void unload_compiler(void);
static char *load_compiler(Size heap_size);

void
Compile(pcompOpt, paramPtr)
pCompOpt  pcompOpt;
StringPtr paramPtr;
	{
/*	register char	*cp; */
	register int i;
	int myRefNum = CurResFile();	/* Record MaxSPITBOL's resource file number */

	phase = initialize;
	heap_handle = NULL;
	stack_handle = NULL;

	if (setjmp(return_jump)) {					/* establish direct return path */
		/* Here when compiler returns via longjmp().
		   We are back to the system stack too! */	
		unload_compiler();
#ifdef ExternalFunctions
		unloadXF();						/* unload all external functions */
#endif
		closerf(myRefNum);				/* close all resource forks */
		close(originp);					/* close source file	*/
		return;
		}

	/*
	/   Allocate initial increment of dynamic memory and zero it.
	/
	/   The code to clear memory can probably be done faster, but ....
	*/
	spitflag = pcompOpt->compflags;				/*  flags to be given to compiler		*/

	for (i = nEMemoryItems; i--; )				/* setup memincb, maxsize, stacksiz		*/
    	*((long *)memadr(i,EMemoryID)) = pcompOpt->ememsizes[i];
	
	basemem = load_compiler( (Size) memincb );
	maxmem = topmem = basemem + memincb;
/*
 *  No need to clear memory.
 	for( cp = basemem ; cp < topmem ; cp += sizeof( word ) )
		*( (word *) cp ) = 0;
*/
	/* Open stdin, stdout */
	if (OpenStdio())
		return;

	/* Open input file */
	sfn = compResult.file;
	originp = spit_open(sfn, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE,
	 IO_OPEN_IF_EXISTS);
	if (originp < 0) {
		myInitCursor(nil);
		CtoPstr(sfn);
		ParamText(sfn,"","","");
		myAlert(SrcOpenAlrt, (ProcPtr)nil);
		PtoCstr(sfn);
		return;
		}

	/* Open resource fork of source file. */
	openres(sfn,0);

	/* Assign message/listing file */
	origoup = STDLSTNO;

/*
 * Test if stdin file and source file are one and the same.  If so, the user wants to
 * have the INPUT variable read data following the END statement.
 */
	CtoPstr(sfn);
	read_following = EqualString(*confStr[ProgInNum],sfn,false,true);
	PtoCstr(sfn);
	
/*
/	Reinitialize global variables that
/	may have been modified in the previous run.
*/
	uarg = (char *) paramPtr;					/*  parameter string for HOST(0)		*/
	readshell0 = 0;								/*  interlock default reading of fd 0	*/

	inpcnt = 1;									/*  one input file						*/

	justended = 0;								/*  END trigger for zysrd				*/
	inpptr = 0;									/* no compilation input files			*/
	inpcnt = 1;									/* ditto								*/
	pINPBUF->next = 0;							/* no chars left in std input buffer	*/
	pINPBUF->fill = 0;							/* ditto								*/
   pINPBUF->offset = (FILEPOS)0;
   pINPBUF->curpos = (FILEPOS)0;
	first_record = 2;
	executing = 0;
	curfile = 0;
	errflag = 0;
	oupState = 0;
	nesting = 0;
	provide_name = 1;
	lnsppage = PAGE_DEPTH;						/*  lines per page for listings			*/
	pagewdth = PAGE_WIDTH;						/*  width of output line for listings	*/
	SetState(CompState, runWptr);

	/*
	/   If -w invoked, must make sure NOEXEC is off so that SYSBX is invoked
	/   by the compiler.  Execution will not occur after the a.out file is
	/   written in any case.
	*/
	if ( spitflag & WRTEXE ) spitflag &= ~NOEXEC;

	/*
	/   Switch to proper input file.
	*/
	swcinp( inpcnt, inpptr );

	/*
	/   Switch to proper output file.
	*/
	swcoup( outptr );

	/*
	/   Suppress Spitbol's pagination in listings.
	*/
	spitflag |= PRTICH;

	/*
	/	Get starting execution time (1/60th seconds)
	*/
	start_time = TickCount();


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
	/   Startup compiler.  Remember its actual load address in case try to
	/					   reuse it later.  Have to know it is in the same place.
	*/
	phase = translate;
	comp_ptr = (char *)StripAddress((long)*comp_handle);
	
	startup( comp_ptr, (char *) (stack_handle != NULL ?
	       (char *)(StripAddress((long)*stack_handle)+stacksiz) : 0L) );
}



/* Note start of execution */
void
ExePhase()
{
	phase = execution;
}

/*
 * DisplayStmt - Display source statement where compiler is currently executing.
 *
 * Used during user-triggered Pause or Stop to show progress of program.
 */
void
DisplayStmt(wPtr)
WindInfoPtr wPtr;
{
	struct scblk *fname;
	word	lineno;
	Str255	fnStr;
	WindInfoPtr wPtr2;
	
	if (phase != execution)
		return;

	whereami(&fname, &lineno);
	if (wPtr && lineno) {
		if (fname->len) {								/* If file name provided */
			strncpy((char *)&fnStr[1],fname->str,(int)fname->len);
			fnStr[0] = fname->len;
			if (wPtr2 = DoLoadFile(fnStr)) {
				UpdateAllWindows();					/* Redraw window */
				wPtr = wPtr2;
				}
			}

		if (wPtr) {
			/* If the source code window is no longer the front window, select it */
			if (FrontWindow() != (WindowPtr) wPtr)
				mySelectWindow((WindowPtr)wPtr);
				
			/* line = -1 is a flag to not change positioning on file */
			if (lineno)
				XYSet(0L, lineno, wPtr); /* Position at line */
			}
		}
}
		

/*
 * DoStopItem - While the compiler was running, the user has selected Stop from the
 *				Compile menu.
 */
void
DoStopItem(wPtr)
WindInfoPtr wPtr;
{
	extern char brkpnd;
	if (!brkpnd) {
		brkpnd++;
		return;
		}
	brkpnd = 0;

	DisplayStmt(wPtr);
	compResult.rcode = Stopped;
	compResult.line = WB(long);				/* line number	*/
	SET_WA( 0 );
	SET_WB( 1 );
	SET_XL(R_FCB);							/* Compiler's fcb chain */
	zysej();								/* no return	*/
}


/*
 *	Load the compiler and allocate its heap, and maybe an alternate stack.
 *	It should be in the resource file as purgeable CODE resource name "SPITBOL Compiler"
 */
static char *
load_compiler(heap_size)
Size	heap_size;
{
	Ptr cp;

	if ( stack_handle ) {
		DisposHandle( stack_handle );
		stack_handle = NULL;
		}

	if ( heap_handle ) {
		DisposHandle( heap_handle );
		heap_handle = NULL;
		}
		
	/* Get size of compiler first time through */
	if ( !comp_handle || !*comp_handle || comp_size == -1 ) {
		SetResLoad(false);
		comp_handle = GetNamedResource('CODE',"\pSPITBOL Compiler");
		comp_size = SizeResource(comp_handle);
		SetResLoad(true);
		if (!comp_handle || comp_size == -1)
			quit(355);
		}
	
	/*
	 * If it already is in memory, and it hasn't moved from the last time we
	 * used it, it means the relocation that was performed was valid and it
	 * can be serially reused.  If it is in memory but at a different location,
	 * the relocation is invalid, and it will have to be reloaded from disk.
	 *
	 * First we try to move it to the top of memory, because that's we're it is
	 * likely to match it's old location.
	 *
	 * comp_handle is guaranteed to be non-zero at this point.
	 */
	HUnlock( comp_handle );				/* May have been locked by DoCompile() */
 	MoveHHi( comp_handle );
	if ((cp = (char *)StripAddress((long)*comp_handle)) == comp_ptr) {
		if (cp)
			/* This is pure paranoia.  But having determined that the existing
			 * copy is in the right place, we want to be absolutely certain that
			 * it doesn't move before it is used.  Basically, we don't trust
			 * the GetResource() call below.
			 */
			HLock( comp_handle );
		}
	else {
		/* It moved.  Release it and force a reload */
		ReleaseResource(comp_handle);
		comp_ptr = NULL;
		comp_handle = NULL;
		}
	
	/* If not in memory, see if there is room to load it */
	if ( !*comp_handle && !SpaceAvailable(comp_size))
		quit(352);

	/* Make sure compiler is in memory */
	comp_handle = GetNamedResource('CODE',"\pSPITBOL Compiler");
	if (!GoodResource(comp_handle))
 		quit(352);

	/*
	 * Move it up in the heap, and lock it.
	 */
 	MoveHHi( comp_handle );
	HLock( comp_handle );

	/*  Decide if desired execution stack will fit in the existing system stack, or
	 *  if a new block must be allocated to hold it.
	 *  Find distance between current sp and heap limit.  Allow 4K slop for comfort,
	 *  since during a Pause, the rest of the interface will run on this stack.
	 */
	if (StackSpace() < (stacksiz + 4096)) {
		/* No room.  Allocate a block to hold stack during Minimal execution.
		   Not used for SYSxx routines, so stack slop not needed here */
		if (!SpaceAvailable(stacksiz) ||
		    !GoodNewHandle(stack_handle = NewHandle(stacksiz)))
			quit(350);

 		/* Move compiler stack to top of application heap, and lock it */
 		MoveHHi( stack_handle );
 		HLock( stack_handle );
 		
 		/* Disable the stack sniffer too. */
		saveStkLowPt = StkLowPt;		/* save old stack sniffer */
		StkLowPt = (char *) 0L;			/* Disable stack sniffer */
		}
	else
		stack_handle = NULL;			/* Use the existing system stack */


	if (!SpaceAvailable(heap_size) ||
		!GoodNewHandle(heap_handle = NewHandle(heap_size)))
		quit(351);

 	/* Move compiler heap to top of application heap, and lock it */
 	MoveHHi( heap_handle );
 	HLock( heap_handle );



	/* Return pointer to compiler's heap	*/
	return (char *) StripAddress((long)*heap_handle);
}


/*
 *	Discard compiler's heap and stack, if any
 */
static void
unload_compiler()
{
	if ( comp_handle != NULL )
		HUnlock( comp_handle );

	if ( heap_handle != NULL ) {
		DisposHandle( heap_handle );
		heap_handle = NULL;
		}

	if ( stack_handle != NULL ) {			/* release any private stack previously allocated */
		DisposHandle( stack_handle );
		stack_handle = NULL;
		StkLowPt = saveStkLowPt;			/* reenable stack sniffer */
		}
}


void exit(i)
register int i;
{
	/*
	 * If rcode is no NoErr, we've been through one of our error routines, and know
	 * what sort of error to return.  If it's still NoErr, this is an normal exit
	 * we'll just go with the user's &CODE value.
	 */
	if (compResult.rcode == NoErr)
		compResult.rcode = i;

	/* Try to be more specific if system just quit */
	if (compResult.rcode == Quit && compResult.msg[0] != '\0')
	compResult.rcode = errorcodes[phase];

	_closeall();
	longjmp(return_jump,1);
	}



/*
/	quit(n)
/
/	exit with error number n.
*/
void quit(n)
int		n;
{
	StringHandle hstr;
	
	hstr = GetString(n);					/* Get error text */
	if (GoodResource((Handle)hstr)) {
		CopyStr(*hstr, (StringPtr) compResult.msg);
		PtoCstr(compResult.msg);
		ReleaseResource(hstr);
		}
	else
		compResult.msg[0] = '\0';
	compResult.errno = n;
	compResult.file[0] = '\0';
	compResult.rcode = Quit;
	exit(1);
}


/*
/    getnum() converts an ASCII string to an integer AND returns a pointer
/    to the character following the last valid digit.  Ignores leading and trailing blanks.
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

register char	*cp;
word	*ip;

{
    word	result = 0;

	while( *cp == ' ' )
		cp++;
    while( *cp >= '0'  &&  *cp <= '9' )
		result = result * 10 + *cp++ - '0';
	while( *cp == ' ' )
		cp++;

    *ip = result;
    return  cp;
}


/*
 * setoptions - install options into the compiler.  This must be done _after_
 *				the compiler has initialized itself.
 */
void
setoptions(flags)
register word flags;
{
	KVCOM = flags & IUSTRG ? 1L : 0L;
	KVDMP = flags & FULDMP ? 2L : (flags & PRTDMP ? 1L : 0L);
	KVFTR = flags & FTRACE ? -1L : 0L;
	KVPFL = flags & PROFIL ? 1L : 0L;
	CSWFL = flags & NOFAIL ? 0L : 1L;
}
#endif
