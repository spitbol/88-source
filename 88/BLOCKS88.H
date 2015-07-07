/*
 *  Copyright 1988-1998, Mark Emmer, Catspaw, Inc.  All rights reserved.
 *  Use of this source code is governed by a BSD-style
 *  license that can be found in the LICENSE file.
*/

/*----------------------------  blocks88.h  --------------------------------*/
#ifndef __blocks88__
#define __blocks88__

/*
 * Definitions of SPITBOL data blocks available to C-language
 * external functions to be called from SPITBOL-88.
 *
 * V1.00	05-25-92 06:45am
 *			8088 version derived from 80386 version.
 *
 * V1.20	10-Sep-92
 *			Add string buffer blocks.
 *
 * V1.27	01-Feb-93
 *			Add fcbmod word to fcb structure.
 *
 * (c) Copyright 1992-1993 Catspaw, Inc.   All Rights Reserved.
 *
 * SPITBOL BLOCKS
 *
 * All SPITBOL objects are encapsulated into blocks of memory.
 * The first word of each block identifies the block type in a
 * curious way.  Rather than containing a simple integer type
 * code, it contains the address of a subroutine that performs
 * some particular action on the block, such as placing it on
 * the stack.  Because each block type uses a different subroutine,
 * the subroutine addresses serve as a type identifier.
 *
 * Each subroutine is proceeded in memory by a one-byte integer
 * type code (given below).  Thus to obtain a simple ordinal
 * type code for a block pointed to by an address in pblk, use the following:
 *
 * block	*pblk;
 * unsigned	typecode;
 *   ...
 * 	typecode = *((unsigned char *)((long)pblk - 1));
 *
 *
 * Here's a visualization of how SPITBOL stores data blocks and identifies
 * their type by pointing to unique sections of code:
 *
 *
 *
 * +---------------------+               +------------------ +
 * |      Type Word      |----+          | Integer type byte |
 * +---------------------+    |          +-------------------+--------+
 * |      Block Data     |    +--------->|   Program Code for this    |
 *...                   ...              |   Block Type               |
 * |                     |               |                            |
 * +---------------------+               +----------------------------+
 *
 *
 * Given an integer type, the Type Word can be obtained by
 * looking it up in a table provided to external functions on the
 * stack in ptyptab.  For example, if the locator information structure is
 * passed to the function as an argument called "info", use the following:
 *
 * unsigned typecode;
 * word		typeword;
 *  ...
 *   typeword = (*info.ptyptab)[typecode];
 */

/*
 * BLOCK CODES FOR ACCESSIBLE DATATYPES
 *
 * These blocks may appear in an argument list if left untranslated
 * by the LOAD function definition.
 */

enum {
	BL_AR =	0,				/* ARBLK	ARRAY								*/
	BL_CD,					/* CDBLK	CODE								*/
	BL_EX,					/* EXBLK	EXPRESSION							*/
	BL_IC,					/* ICBLK	INTEGER								*/
	BL_NM,					/* NMBLK	NAME								*/
	BL_P0,					/* P0BLK	PATTERN, 0 args						*/
	BL_P1,					/* P1BLK	PATTERN, 1 arg						*/
	BL_P2,					/* P2BLK	PATTERN, 2 args						*/
	BL_RC,					/* RCBLK	REAL								*/
	BL_SC,					/* SCBLK	STRING								*/
	BL_SE,					/* SEBLK	EXPRESSION							*/
	BL_TB,					/* TBBLK	TABLE								*/
	BL_VC,					/* VCBLK	VECTOR (array)						*/
	BL_XN,					/* XNBLK	EXTERNAL, non-relocatable contents	*/
	BL_XR,					/* XRBLK	EXTERNAL, relocatable contents		*/
	BL_BC,					/* BCBLK	BUFFER CONTROL						*/
	BL_PD,					/* PDBLK	PROGRAM DEFINED DATATYPE			*/
	BL__D					/* NUMBER OF BLOCK CODES FOR DATA				*/
	};

/*
 * OTHER BLOCK CODES
 *
 * These blocks will never appear in an argument list, but are
 * listed here for completeness.
 */

enum {
	BL_TR = BL__D,			/* TRBLK	TRACE							*/
	BL_BF,					/* BFBLK	BUFFER							*/
	BL_CC,					/* CCBLK	CODE CONSTRUCTION				*/
	BL_CM,					/* CMBLK	COMPILER TREE NODE				*/
	BL_CT,					/* CTBLK	CHARACTER TABLE					*/
	BL_DF,					/* DFBLK	DATATYPE FUNCTION				*/
	BL_EF,					/* EFBLK	EXTERNAL FUNCTION				*/
	BL_EV,					/* EVBLK	EXPRESSION VARIABLE				*/
	BL_FF,					/* FFBLK	FIELD FUNCTION					*/
	BL_KV,					/* KVBLK	KEYWORD VARIABLE				*/
	BL_PF,					/* PFBLK	PROGRAM-DEFINED FUNCTION		*/
	BL_TE					/* TEBLK	TABLE ELEMENT					*/
	};

/*
 * Structure of common SPITBOL blocks:
 * 	    Integer, Real, String, and File blocks.
 *
 *  	These structures are part of the "blocks" union that can be applied
 * 	    to the result area to determine where to store required return
 *      information.
 */

/*
 *  	Structure of ICBLK (integers)
 */

struct icblk {
	word 	ictyp;					/* type word						*/
	iatype 	icval;					/* integer value					*/
	};


/*
 * 		Structure of RCBLK (reals)
 */

struct rcblk {
	word	rctyp;					/* type word						*/
	double	rcval;					/* real value						*/
	};


/*
 *  	Structure of SCBLK (strings)
 */

struct scblk {
	word	sctyp;					/* type word						*/
	word	sclen;					/* string length					*/
	char	scstr[1];				/* start of string					*/
	};

/*
 *   	Structure for returning a far string
 */

struct fsblk {
	word	fstyp;					/* type word						*/
	word	fslen;					/* string length					*/
	char far *fsptr;				/* far pointer to string			*/
	};


/*
 *   	Structure for returning a far external block
 */

struct fxblk {
	word	fxtyp;					/* type word						*/
	word	fxlen;					/* external data length				*/
	void far *fxptr;				/* far pointer to external data		*/
	};


/*
 * FILE CONTROL BLOCK
 *
 * The user may provide the word "FILE" for any argument in
 * the LOAD function prototype.  When a call is made to the
 * external function with an I/O associated variable in this argument
 * position, SPITBOL will provide a pointer to the file control
 * block instead of the value of the variable.
 *
 * The file control block (FCB) points to an I/O block with
 * additional information.  In turn, the I/O block points to any
 * buffer used by the file.
 *
 * This block is obtained for every file except those associated
 * with INPUT, OUTPUT, or TERMINAL.  Note that these FCB's are
 * unrelated to MS-DOS FCB's.  File control blocks do not have
 * their own type word, but appear as XRBLKs with the following structure:
 */

struct fcblk {
	word			fcbtyp;	 		/* type word (XRBLK)				*/
	word			fcblen;	 		/* size of block, in bytes			*/
	word			fcbrsz;	 		/* SPITBOL record size				*/
	struct ioblk near *fcbiob;		/* near pointer to IOBLK			*/
	word			fcbmod;			/* 1 if text mode, 0 if binary mode	*/
	};

/*
 *   CHFCB - chain of FCBs block
 *
 *   For every FCB created by OSINT, the compiler creates a CHFCB pointing
 *   to the FCB and links it onto a chain of CHFCBs.  At EOJ the head of this
 *   CHFCB chain is passed to the interface function SYSEJ so that all files
 *   can be closed.
 */

struct	chfcb {
	word	typ;				/*  type word			*/
	word	len;				/*  block length		*/
	struct	chfcb near	*nxt;	/*  pointer to next chfcb	*/
	struct	fcblk near	*fcp;	/*  pointer to fcb		*/
	};


/*
 * I/O BLOCK
 *
 * An I/O block is pointed to by the fcbiob field of a file control block.
 */

struct ioblk {
	word			iobtyp;	   		/* type word (XRBLK)				*/
	word			ioblen;			/* size of IOBLK in bytes			*/
	struct scblk near *iobfnm;		/* near ptr to SCBLK with filename	*/
	word			iobpid;			/* pipe id (not used for DOS)		*/
	struct bfbblk near *iobbfb; 	/* near pointer to BFBBLK			*/
	word			iobfdn;			/* MS-DOS file descriptor number 	*/
	word			iobflg1;		/* flags 1 (see below)				*/
	word			iobflg2;		/* flags 2 (see below)				*/
	word			iobeol1;		/* end of line character 1			*/
	word			iobeol2;		/* end of line character 2			*/
	word			iobshare; 		/* sharing mode						*/
	word			iobaction;		/* file open actions				*/
	};

/*
 * 	Bits in iobflg1 dword
 */
#define IO_INP	0x00000001			/* input file							*/
#define IO_OUP	0x00000002			/* output file							*/
#define IO_APP	0x00000004			/* append output to existing file		*/
#define IO_OPN	0x00000008			/* file is open							*/
#define IO_COT	0x00000010			/* console output to non-disk device	*/
#define IO_CIN	0x00000020			/* console input from non-disk device	*/
#define IO_SYS	0x00000040			/* -f option used instead of name		*/
#define IO_WRC	0x00000080			/* output without buffering				*/

/*
 * 	Bits in iobflg2 dword
 */
#define IO_PIP	0x00000001			/* pipe (not used in MS-DOS)			*/
#define IO_DED	0x00000002			/* dead pipe (not used in MS-DOS)		*/
#define IO_ILL	0x00000004			/* illegal I/O association				*/
#define IO_RAW	0x00000008			/* binary I/O to character device			*/
#define IO_LF 	0x00000010			/* ignore line feed if next character	*/
#define IO_NOE	0x00000020			/* no echo input						*/
#define IO_ENV	0x00000040			/* filearg1 mapped via environment var	*/
#define IO_DIR  0x00000080		    /* buffer is dirty (needs to be written)*/
#define	IO_BIN	0x00000100			/* binary I/O */

/* Private flags used to convey sharing status when opening a file */
#define IO_COMPATIBILITY	0x00
#define IO_DENY_READWRITE	0x01
#define IO_DENY_WRITE		0x02
#define IO_DENY_READ		0x03
#define IO_DENY_NONE		0x04
#define IO_DENY_MASK		0x07		/* mask for above deny mode bits*/
#define	IO_EXECUTABLE		0x40		/* file to be marked executable */
#define IO_PRIVATE			0x80		/* file is private to current process */

/* Private flags used to convey file open actions */
#define IO_FAIL_IF_EXISTS		0x00
#define IO_OPEN_IF_EXISTS		0x01
#define IO_REPLACE_IF_EXISTS	0x02
#define IO_FAIL_IF_NOT_EXIST	0x00
#define IO_CREATE_IF_NOT_EXIST	0x10
#define IO_EXIST_ACTION_MASK	0x13	/* mask for above bits */
#define IO_WRITE_THRU       	0x20	/* writes complete before return*/


/*
 * I/O BUFFER BLOCK
 *
 * An I/O buffer block (BFBBLK) is pointed to by an IOBLK.
 */

#if SETREAL
typedef double FILEPOS;     /* real file positions */
#else
typedef long FILEPOS;       /* 32-bit file positions */
#endif

struct bfbblk {
	word	bfbtyp;					/* type word (XNBLK)					*/
	word	bfblen;					/* size of BFBBLK, in bytes				*/
	word	bfbsiz;					/* size of buffer in bytes				*/
	word	bfbfil;					/* number of bytes currently in buffer	*/
	word	bfbnxt;					/* offset of next buffer char to r/w	*/
	long	bfboff;					/* file position of first byte in buf	*/
	long	bfbcur;					/* physical file position				*/
	char	bfbbuf[1];				/* start of buffer						*/
	};


/*
 * Structure of EFBLK (external function).  A pointer to this block
 * is passed to the external function in the stack in info.pefblk.
 */

struct efblk {
	word			fcode;			/* type word							*/
	word   			fargs;			/* number of arguments					*/
	word			eflen;			/* block length							*/
	word			efuse;			/* usage count							*/
	struct xnblk near *efcod;		/* pointer to XNBLK, see below			*/
	struct vrblk near *efvar;		/* pointer to VRBLK with function name	*/
	word			efrsl;			/* result type  (see below)				*/
	word			eftar[1];		/* array of argument types, one per arg	*/
	};

/*
 * efrsl and eftar[] contain small integer type codes as follows:
 */

#define noconv	0					/* argument remains unconverted			*/
#define constr	1					/* convert argument to string			*/
#define conint	2					/* convert argument to integer			*/
#define conreal	3					/* convert argument to real				*/
#define confile	4					/* produce fcb associated with variable	*/


/*
 * Structure of XNBLK allocated for external function
 * A pointer to this structure is passed to the external function
 * in the stack in pxnblk.
 *
 * This structure is used two ways:
 *   1.	 As a general structure in which the user can place private
 * 		 data and have it maintained by SPITBOL.
 *
 *   2.  As a particular structure in which information about each
 * 		 external function is maintained.
 */

struct xnblk {
	word	xntyp;					/* type word							*/
	word	xnlen;					/* length of this block					*/
	union {							/* two uses for rest of block:			*/
		word	xndta[1];			/* 1. user defined data starts here		*/
		struct ef {                 /* 2. external function information		*/
			int far (*xncall)();	/*	  entry point of function			*/
			void far *xnmemp;		/*	  base address of function memory	*/
			long	xnsiz;			/*    size of COM file					*/
			short	xnip;			/*	  transfer IP						*/
			short	xncs;			/*    transfer CS						*/
			short	xnsp;			/*	  transfer SP, 0 = SPITBOL's stack  */
			short	xnss;			/*	  transfer SS, 0 = SPITBOL's stack	*/
			short	xnds;			/*	  transfer DS						*/
			short	xnes;			/*	  transfer ES						*/
			short	xn1st;			/*    non-zero = first-ever call		*/
			short	xnsave;			/*    non-zero = first call after reload*/
			void far (*xncbp)();	/*    callback function prior to exiting*/
			} ef;
		} xnu;
	};

/*
 * Simplified access to xn1st and xnsave words in xnblk via pointer to
 * miscellaneous info area in pmisc.
 */

#define first_call ((*((*pmisc).pxnblk)).xnu.ef.xn1st)
#define reload_call ((*((*pmisc).pxnblk)).xnu.ef.xnsave)


/*
 * Other selected blocks of interest:
 *
 *
 * ARRAY BLOCK
 *
 * An array block (ARBLK) represents an array value other than one
 * with one dimension whose lower bound is one (see VCBLK).
 */

struct arblk1 {	   					/* One dimensional array				*/
	word			arblk;			/* type word (ARBLK)                    */
	word			aridv;			/* identifier value						*/
	word			arlen;			/* length of ARBLK in bytes				*/
	word			arofs;			/* offset in arblk to arpro field		*/
	word			arndm;			/* number of dimensions					*/
	iatype			arlbd;			/* low bound (first subscript)			*/
	iatype			ardim;			/* dimension (first subscript)			*/
	struct scblk near *arpro;		/* array prototype string				*/
	union block	 near *arvls[1];	/* start of values in row-wise order	*/
	};

struct arblk2 {						/* Two dimensional array				*/
	word			arblk;	  		/* type word (ARBLK)                    */
	word			aridv;	  		/* identifier value						*/
	word			arlen;			/* length of ARBLK in bytes				*/
	word			arofs;			/* offset in arblk to arpro field		*/
	word			arndm;			/* number of dimensions					*/
	iatype			arlbd;			/* low bound (first subscript)			*/
	iatype			ardim;			/* dimension (first subscript)			*/
	iatype			arlb2;			/* low bound (second subscript)			*/
	iatype			ardm2;			/* dimension (second subscript)			*/
	struct scblk near *arpro;		/* array prototype string				*/
	union block	 near *arvls[1];	/* start of values in row-wise order	*/
	};

#define ndim	3					/* For example, 3-dimensional array		*/
struct arblkn {						/* N-dimensional array					*/
	word			arblk;			/* type word (ARBLK)                    */
	word			aridv;			/* identifier value						*/
	word			arlen;			/* length of ARBLK in bytes				*/
	word			arofs;			/* offset in arblk to arpro field		*/
	word			arndm;		  	/* number of dimensions					*/
	struct {
		iatype	arlbd;				/* low bound (first subscript)			*/
		iatype	ardim;				/* dimension (first subscript)			*/
		} 			bounds[ndim]; 	/* adjust for number of dimensions		*/
	struct scblk near *arpro;	  	/* array prototype string				*/
	union block	near *arvls[1];		/* start of values in row-wise order	*/
	};

/*
 * 	BUFFER CONTROL BLOCK
 *
 *	A buffer control block (BCBLK) is created by the BUFFER
 *	function, and serves as an indirect control header for the
 *	buffer. It contains the number of characters currently
 *	stored in the buffer.
 */
struct bcblk {
	word	bctyp;					/* type word							*/
	word	bcidv;					/* identifier value						*/
	word	bclen;					/* number of chars in use in bfblk		*/
	word	bcbuf;					/* pointer to bfblk						*/
	};


/*
 * 	STRING BUFFER BLOCK
 *
 *	A string buffer block (BFBLK) contains the actual buffer
 *	memory area. It specifies the largest string that can be
 *	stored in the buffer.
 */
struct bfblk {
	word	bftyp;					/* type word							*/
	word	bfalc;					/* allocated size of buffer				*/
	char	bfchr[1];				/* characters of string					*/
	};


/*
 * CODE BLOCK
 * 
 * A code block (CDBLK) is present for every source statement.
 */

struct cdblk {
	word			cdjmp;			/* ptr to routine to execute statement	*/
	word			cdstm;			/* statement number						*/
	word			cdsln;			/* source file line number				*/
	word			cdlen;			/* length of CDBLK in bytes				*/
	union {
		struct cdblk near *cdnxt;	/* if failure exit is next statement	*/
		struct vrblk near *cdlab;	/* if failure exit is a simple label	*/
		char		 near *cdnof;	/* no failure exit (-NOFAIL mode)		*/
		word		  cddir;		/* failure exit is complex or direct	*/
		}			cdfal;			/* Failure exit							*/
	word			cdcod[1];		/* executable pseudo-code				*/
	};


/*
 * NAME BLOCK
 *
 * A name block (NMBLK) is used whereever a name must be stored as
 * a value following use of the unary dot operator.
 */

struct nmblk {
	word			nmtyp;		  	/* type word (NMBLK)					*/
	union block near *nmbas;		/* base pointer for variable			*/
	word			nmofs;			/* offset within block for variable		*/
	};


/*
 * TABLE BLOCK
 *
 * A table block (TBBLK) is used to represent a table value.
 * It comprises a list of buckets, each of which may point to
 * a chain of TEBLKs.  TBBUK entries either point to the first
 * TEBLK on the chain or they point to the TBBLK itself to
 * indicate the end of the chain.  The number of buckets can
 * be deduced from tblen.
 */

struct tbblk {
	word			tbtyp;			/* type word (TBBLK)					*/
	word			tbidv;			/* identifier value						*/
	word			tblen;			/* length of TBBLK in bytes				*/
	union block  near *tbinv;		/* default initial lookup value			*/
	struct teblk near *tbbuk[1];	/* start of hash bucket pointers		*/
	};


/*
 * TABLE ELEMENT BLOCK
 *
 * A table element (TEBLK) is used to represent a single entry in
 * a table.
 */

struct teblk {
	word			teblk;			/* type word (TEBLK)					*/
	union block	 near *tesub;		/* subscript value						*/
	union block  near *teval; 		/* table element value					*/
	struct teblk near *tenxt;		/* next TEBLK or TBBLK if end of chain	*/
	};


/*
 * VARIABLE BLOCK
 *
 * A variable block (VRBLK) is used to hold a program variable.
 */

struct vrblk {
	word			vrget;			/* routine to load variable onto stack	*/
	word			vrsto;			/* routine to store stack top into var.	*/
	union block	 near *vrval;		/* variable value						*/
	word			vrtra;			/* routine to transfer to label			*/
	union block	 near *vrlbl;		/* pointer to code for label			*/
	union block	 near *vrfnc;		/* function block if name is function	*/
	struct vrblk near *vrnxt;		/* next vrblk on hash chain				*/
	word			vrlen;			/* length of name						*/
	char			vrchs[1];		/* characters of name					*/
	};


/*
 * VECTOR BLOCK
 *
 * A vector block (VCBLK) is used to represent an array value which has
 * one dimension whose lower bound is one. All other arrays are
 * represented by ARBLKs.  The number of elements can be deduced
 * from vclen.
 */

struct vcblk {
	word			vctyp;			/* type word (VCBLK)					*/
	word			vcidv;			/* identifier value						*/
	word			vclen;			/* length of vcblk in bytes				*/
	union block	near *vcvls[1];		/* start of vector values				*/
	};


/*
 * UNION OF ALL BLOCKS
 *
 * A block is merely a union of all the block types enumerated here.
 *
 */

union block {
	struct arblk1	arb1;
	struct arblk2	arb2;
	struct arblkn	arbn;
	struct bcblk	bcb;
	struct bfblk	bfb;
	struct cdblk	cdb;
	struct efblk	efb;
	struct fcblk	fcb;
	struct fsblk	fsb;
	struct fxblk	fxb;
	struct icblk	icb;
	struct ioblk	iob;
	struct nmblk	nmb;
	struct rcblk	rcb;
	struct scblk	scb;
	struct tbblk	tbb;
	struct teblk	teb;
	struct vcblk	vcb;
	struct vrblk	vrb;
	struct xnblk	xnb;
	};

#endif						/* __blocks88__ */
/*------------------------  end of blocks88.h  -------------------------------*/
