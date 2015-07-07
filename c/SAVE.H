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
/	File:   SAVE.H           Version 1.01
/	-------------------------------------
/
/	This header file provides information for writing the impure
/	portions of SPITBOL's data segments to a save file.
/
/ v1.01 3-Jun-91 MBE
/	Added memincb, maxsize, readshell0 & uarg to header, and
/	additional argument to specify whether these values should
/	override existing values, as would be the case for the
/	Intel MS-DOS version, where Save files are used to simulate
/	Exec files.
/
*/
#if SAVEFILE
#define VWBSHFT 4
#define VIASHFT 5
#define SaveVersion (4+((WORDBITS==16)<<VWBSHFT)+((IA32==0)<<VIASHFT))
#define UargSize 128

struct svfilehdr {
	unsigned long magic1;	/* recognizer for object file	*/
	unsigned long magic2;	/* line terminators 			*/
	unsigned char version;	/* version of runtime needed	*/
	unsigned char system;	/* system version				*/
	short int spare;		/* spare cells to dword boundary*/
	char	serial[8];		/* compiler serial number		*/
	char	headv[8];		/* version string 				*/
	char	iov[12];		/* i/o version string 			*/
    unsigned long timedate; /* date and time of creation    */
	long	flags;			/* spitflag word 				*/
	uword	stacksiz;		/* total size of stack area		*/
	char	*stbas;			/* base of stack at save time	*/
	uword	stacklength;	/* size of stack area in use	*/
	uword	sec3size;		/* size of constant section		*/
	char	*sec3adr;		/* address of constant section	*/
	uword	sec4size;		/* size of work section			*/
	char	*sec4adr;		/* address of work section		*/
	uword	statoff;		/* static offset=hshtb-basemem	*/
	uword   dynoff;			/* dynamic offset=dnamb-basemem	*/
	uword	heapsize;		/* size of heap after collection*/
	char	*heapadr;		/* address of heap				*/
	char	*topmem;		/* previous top of heap			*/
	uword	databts;		/* largest allowed size of heap */
	uword	sec5size;		/* size of SPITBOL code			*/
	char	*sec5adr;		/* address of SPITBOL code		*/
	uword	memincb;		/* memory increment size		*/
	uword	maxsize;		/* maximum object size			*/
	word	compress;		/* 0 = no compress, >0 = code word size (bits) */
	word	uarglen;		/* length of -u command string	*/
	};

#define OURMAGIC1 0xfaa5a5fa
#define OURMAGIC2 0x0d0a0d0a
#endif					/* SAVEFILE */

#if APPLE
#define PAGESIZE	1
#define L2PGSZ		0		/* Log base 2 of page size */
#endif					/* APPLE */

#if ATT73
#define T_START		0x80000			/* text start address	*/
#define D_START		0xA0000			/* data start address	*/
#define PAGESIZE	1024
#define L2PGSZ		10		/* Log base 2 of page size */
#endif					/* ATT73 */

#if DSI
#define	PAGESIZE	1
#define L2PGSZ		0		/* Log base 2 of page size */
#define T_START 	0x4000
#endif					/* DSI */

#if HPUX
#define	PAGESIZE	EXEC_PAGESIZE
#define T_START 	0x0
#endif					/* HPUX */

#if HIGHC | INTEL | OS2 | BCC32 | VCC
#define	PAGESIZE	4096
#define L2PGSZ		12		/* Log base 2 of page size */
#endif					/* HIGHC | INTEL | OS2 */

#if BCC16
#define	PAGESIZE	512
#define L2PGSZ		9		/* Log base 2 of page size */
#endif					/* BCC16 */

#if NCRXP
#define T_START		0x8000			/* text start address	*/
#define D_START		0x18000	??		/* data start address	*/
#define	OURMAGIC	NCRROMAGIC		/* 16-bit magic		*/
#define PAGESIZE	1
#define L2PGSZ		0		/* Log base 2 of page size */
#endif					/* NCRXP */

#if NCR32
#define T_START		0x0			/* text start address	*/
#define D_START		0x10000	??		/* data start address	*/
#define	OURMAGIC	NCRT32MAGIC		/* 32-bit magic		*/
#define PAGESIZE	1
#define L2PGSZ		0		/* Log base 2 of page size */
#endif					/* NCR32 */

#if SUN2 | SUN3 | HPBSD
#define	PAGESIZE	PAGSIZ
#define T_START     0x2020
#endif					/* SUN2 | SUN3 | HPBSD */

#if SUN4
#define	PAGESIZE	PAGSIZ
#define T_START     SEGSIZ   /* N_MAGIC, no dynamic loading */
#endif					/* SUN4 */

#if UNISYS
#define T_START		0x80000			/* text start address	*/
#define D_START		0xA0000			/* data start address	*/
#define PAGESIZE	1
#define L2PGSZ		0		/* Log base 2 of page size */
#endif					/* UNISYS */

#if VAX
#define	PAGESIZE	1024
#define L2PGSZ		10		/* Log base 2 of page size */
#endif					/* VAX */

#if ATT73
						/* file header		*/
struct filehdr fheader = {MC68KPGMAGIC,		/* magic number		*/
			  2,			/* number of sections	*/
			  0,			/* time supplied later	*/
			  0,			/* symtab pointer	*/
			  0,			/* no. symtab entries	*/
			  sizeof(AOUTHDR),	/* size of a.out header	*/
						/* Flag word:		*/
			  F_RELFLG + F_EXEC +	/* no reloc info, exec.	*/
			  F_LNNO + F_LSYMS +	/* no line no. or symbs	*/
			  F_AR32W +		/* specify byte order	*/
			  (F_M68010 * F_CPU) +	/* 68010 opcodes only	*/
			  (F_SOFT * F_FPU)	/* software floating pt	*/
			  };

						/* a.out header		*/
struct aouthdr header = {AOUT3MAGIC,		/* magic number		*/
			 0,			/* version stamp	*/
			 0,			/* text size supplied	*/
			 0,			/* data size supplied	*/
			 0,			/* no bss area		*/
			 T_START,		/* entry point		*/
			 T_START,		/* text start adr	*/
			 D_START		/* data start adr	*/
			};

						/* text section header	*/
struct scnhdr theader = {_TEXT,			/* section name		*/
			 T_START,		/* physical address	*/
			 T_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 0x400,			/* ptr to text in file	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_TEXT		/* flags		*/
			};

						/* data section header	*/
struct scnhdr dheader = {_DATA,			/* section name		*/
			 D_START,		/* physical address	*/
			 D_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 0,			/* ptr to data later	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_DATA		/* flags		*/
			};
#endif					/* ATT73 */

#if UNISYS
#define HDRSIZE sizeof(struct filehdr)+sizeof(struct aouthdr)+3*sizeof(struct scnhdr)
						/* file header		*/
struct filehdr fheader = {MC68KROMAGIC,		/* magic number		*/
			  3,			/* number of sections	*/
			  0,			/* time supplied later	*/
			  0,			/* symtab pointer	*/
			  0,			/* no. symtab entries	*/
			  sizeof(AOUTHDR),	/* size of a.out header	*/
						/* Flag word:		*/
			  F_RELFLG + F_EXEC +	/* no reloc info, exec.	*/
			  F_LNNO + F_LSYMS +	/* no line no. or symbs	*/
			  F_AR32W +		/* specify byte order	*/
			  044000		/* UNISYS purpose unkown*/
			  };

						/* a.out header		*/
struct aouthdr header = {0410,			/* magic number-RO text	*/
			 0,			/* version stamp	*/
			 0,			/* text size supplied	*/
			 0,			/* data size supplied	*/
			 0,			/* no bss area		*/
			 T_START,		/* entry point		*/
			 T_START,		/* text start adr	*/
			 D_START		/* data start adr(upd.)	*/
			};

						/* text section header	*/
struct scnhdr theader = {_TEXT,			/* section name		*/
			 T_START,		/* physical address	*/
			 T_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 HDRSIZE,		/* ptr to text in file	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_TEXT		/* flags		*/
			};

						/* data section header	*/
struct scnhdr dheader = {_DATA,			/* section name		*/
			 D_START,		/* physical address	*/
			 D_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 0,			/* ptr to data later	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_DATA		/* flags		*/
			};
						/* bss section header	*/
struct scnhdr bheader = {_BSS,			/* section name		*/
			 0,			/* physical address	*/
			 0,			/* virtual address	*/
			 0,			/* size 0		*/
			 0,			/* not used		*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_BSS		/* flags		*/
			};
#endif					/* UNISYS */

#if NCR
						/* file header		*/
struct filehdr fheader = {OURMAGIC,		/* magic number		*/
			  3,			/* number of sections	*/
			  0,			/* time supplied later	*/
			  0,			/* symtab pointer	*/
			  0,			/* no. symtab entries	*/
			  sizeof(AOUTHDR),	/* size of a.out header	*/
						/* Flag word:		*/
			  F_RELFLG + F_EXEC +	/* no reloc info, exec.	*/
			  F_LNNO + F_LSYMS +	/* no line no. or symbs	*/
			  F_AR32W		/* specify byte order	*/
			  };

						/* a.out header		*/
struct aouthdr header = {AOUT1MAGIC,		/* magic number-RO text	*/
			 0,			/* version stamp	*/
			 0,			/* text size supplied	*/
			 0,			/* data size supplied	*/
			 0,			/* no bss area		*/
			 T_START,		/* entry point		*/
			 T_START,		/* text start adr	*/
			 D_START		/* data start adr	*/
			};

						/* text section header	*/
struct scnhdr theader = {_TEXT,			/* section name		*/
			 T_START,		/* physical address	*/
			 T_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 sizeof(struct filehdr)	/* ptr to text in file	*/
			  +sizeof(struct aouthdr) /* = file headers	*/
			  +3*sizeof(struct scnhdr),  /* + section hdrs	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_TEXT		/* flags		*/
			};

						/* data section header	*/
struct scnhdr dheader = {_DATA,			/* section name		*/
			 D_START,		/* physical address	*/
			 D_START,		/* virtual address	*/
			 0,			/* size supplied later	*/
			 0,			/* ptr to data later	*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_DATA		/* flags		*/
			};

						/* bss section header	*/
struct scnhdr bheader = {_BSS,			/* section name		*/
			 0,			/* physical address	*/
			 0,			/* virtual address	*/
			 0,			/* size 0		*/
			 0,			/* not used		*/
			 0,			/* relocation ptr	*/
			 0,			/* lnno ptr		*/
			 0,			/* no. reloc entries	*/
			 0,			/* no. line numbers	*/
			 STYP_BSS		/* flags		*/
			};
#endif					/* NCR */
