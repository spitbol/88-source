/*
  Copyright 1988-1998 Mark Emmer, Catspaw, Inc.  All Rights Reserved.
  Use Of this source code is governed by a BSD-style
  license that can be found in the LICENSE file.
*/

/**************************************************************************\
*                                                                          *
*  SYSBCC16.C - 16-bit Borland C version of common system calls for SPITBOL*
*                                                                          *
\**************************************************************************/

#define PRIVATEBLOCKS
#include "port.h"
#include <dos.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <float.h>
#undef sbrk
#include <malloc.h>

unsigned _Cdecl _stklen = 2048;		/* OSINT stack size in bytes */
static union REGS r;
static struct SREGS s;

static void far *malloced_mem = NULL;

enum {
	For_reading = 0,
	For_writing,
	For_updating
	} Open_method_type;

/* Bits in DOS Extended Open/Create mode word */
#define OPEN_FLAGS_FAIL_ON_ERROR       0x2000  /* --1- ---- ---- ---- */
#define OPEN_FLAGS_WRITE_THROUGH       0x4000  /* -1-- ---- ---- ---- */

/* DOS Extended Open/Create flags */
#define FILE_OPEN       0x0001
#define FILE_TRUNCATE   0x0002
#define FILE_CREATE     0x0010
	/* this nibble applies if file already exists                xxxx */
#define OPEN_ACTION_FAIL_IF_EXISTS     0x0000  /* ---- ---- ---- 0000 */
#define OPEN_ACTION_OPEN_IF_EXISTS     0x0001  /* ---- ---- ---- 0001 */
#define OPEN_ACTION_REPLACE_IF_EXISTS  0x0002  /* ---- ---- ---- 0010 */

   /*  this nibble applies if file does not exist           xxxx      */
#define OPEN_ACTION_FAIL_IF_NEW        0x0000  /* ---- ---- 0000 ---- */
#define OPEN_ACTION_CREATE_IF_NEW      0x0010  /* ---- ---- 0001 ---- */

/* DOS Extended Open/Create file attributes */
#define FILE_NORMAL     0x0000
#define FILE_READONLY   0x0001
#define FILE_HIDDEN     0x0002
#define FILE_SYSTEM     0x0004
#define FILE_VOLUME		0x0008
#define FILE_DIRECTORY  0x0010
#define FILE_ARCHIVED   0x0020

#define MethodMask	(O_RDONLY | O_WRONLY | O_RDWR)


/* Size and offset of fields of a structure.  Probably not portable. */
#define FIELDSIZE(str, fld) (sizeof(((str *)0)->fld))
#define FIELDOFFSET(str, fld) ((unsigned long)(char *)&(((str *)0)->fld))


#define DEBUG 0

struct MZHDR {						/* Header at beginning of EXE file   */
   unsigned short  signature;       /* Signature: 'MZ'                   */
   unsigned short  size_mod;        /* File size (bytes) modulo 512      */
   unsigned short  size_blks;       /* File size in 512-byte blocks      */
   unsigned short  reloc_items;     /* Number of relocation items        */
   unsigned short  header_size;     /* Size of header in paragraphs      */
   unsigned short  minalloc;		/* Minimum no. of paragraphs wanted  */
   unsigned short  maxalloc;		/* Maximum no. of paragraphs wanted  */
   unsigned short  stack_seg;		/* Displace of stack seg in paras.   */
   unsigned short  start_sp;		/* Offset in SP register             */
   unsigned short  checksum;		/* Word checksum                     */
   unsigned short  start_ip;		/* IP register offset                */
   unsigned short  code_seg;		/* Code segment displacement in paras*/
   unsigned short  reloc;			/* Displace of first relocation item */
   unsigned short  overlay;			/* Overlay number (used as SAVE flag)*/
   };

extern unsigned short mindata_ds;

/* supply the default control word for the numeric co-processor (see 
 * BC4\SOURCE\RTL\CTRL386\COMMON\DEFLT87.C
 *
 * By masking overflow and zero, 8087 will produce infinity, which is
 * detected by OVR_.
 */
unsigned int _RTLENTRY _EXPDATA _default87 =
    RC_NEAR+PC_64+IC_AFFINE+EM_UNDERFLOW+EM_INEXACT+EM_DENORMAL+EM_OVERFLOW+EM_ZERODIVIDE;


/* Borland C++ 4.x */
double __matherr(_mexcep why, char *fun, double *arg1p, double *arg2p, double retval)
{
   extern double inf;	/* infinity */
   switch (why)
   {
   	case DOMAIN:	 	/* argument domain error -- log (-1)        */
   		return retval;	/* should be detected by Minimal code		*/
    case SING:       	/* argument singularity  -- pow (0,-2))     */
		return inf;     /* should never happen						*/
    case OVERFLOW:   	/* overflow range error  -- exp (1000)      */
		return inf;     /* infinity detected by OVR					*/
    case UNDERFLOW:  	/* underflow range error -- exp (-1000)     */
		return 0.0;		/* quietly return 0							*/
    case TLOSS:			/* total loss of significance -- sin(10e70) */
    case PLOSS:      	/* partial loss of signif. -- not used      */
		return retval;  /* ????										*/
    case STACKFAULT:    /* floating point unit stack overflow       */
		return inf;		/* ????										*/
   }
   return inf;			/* should never get here					*/																	  
}

void _fperror() {}

/* keep Borland C++ 3.0 and 4.0 from loading stream I/O system */
void _setupio() {}
/* keep Borland C++ 4.0 from loading class libraries. */
/* Stub out all exception handling.  Alternately, link in Borland's noxx.cpp module. */
void __ErrorMessage(const char *msg) { wrterr(msg);}
void _ExceptInit() {}
void _abort(void) { _exit(3); }
void abort() {_abort();}

/* The following satisfies a library reference chain that goes something
 * like this:
 *  lseek and open call __IOerror and __DOSerror in library file ioerror.cas.
 *  ioerror.cas references variable _sys_nerr in library file perror.c.
 *  perror.c calls fputs, bringing in the stream I/O system.
 */
int _Cdecl _sys_nerr = 48;  /* highest defined system error number + 1 */

/* This corrupted version of clock() does not subtract startup time from
 * current clock reading.  That's OK, because SPITBOL does it itself.
 * This avoids pulling in all sorts of time zone and daylight savings
 * code.
 */
clock_t clock(void)
{
   clock_t NowTime;
   unsigned char DayFlip;

asm   xor    ah,ah
asm   int    1ah                        /* call BIOS for timer ticks       */

asm   mov    byte ptr DayFlip, al       /* record whether day has changed  */

asm   mov    word ptr NowTime, DX       /* save return values              */
asm   mov    word ptr NowTime+2, CX     /*  which is current time in ticks */


   if (_AL != 0)                        /* did day change just now?        */
   {
      /* Set the BIOS midnight flag to 1 again, then call DOS get-date
       * function to force DOS to update its internal date counter.
       */
      *((unsigned char far *)MK_FP(0x40,0x70)) = 1;
      _AH = 0x2a;                       /* get DOS date to force DOS       */
      geninterrupt(0x21);               /*  to recognize date change       */
   }

   if (DayFlip)
      NowTime += 0x1800b0L;             /* number of clock ticks in day    */

   return NowTime * (CLOCKS_PER_SEC / 18.2065);		
                                        /* return in units of CLOCKS_PER_SEC */
}

/* mk_mp
 * 
 * Make a far pointer into the Minimal heap from a near pointer.
 * If the pointer is 0 or 1, then a 0 segment is returned,
 * otherwise the Minimal data segment is returned.
 */
void *mk_mp(void near *minp)
{
	if (minp <= (void near *)1)
		return MK_FP(0,(unsigned)minp);
	else
		return MK_FP(mindata_ds,(unsigned)minp);
}

void exit_custom(int code)
{
   if (malloced_mem)
      free(malloced_mem);
}


int dos(unsigned char fn)
   {
   r.h.ah = fn;
   intdos(&r, &r);
   if (r.x.cflag) {
      errno = _doserrno;		/* Note: DOS error number, not errno.h number! */
      return -1;
      }
   else
      return 0;
   }

int dosx(unsigned char fn)
   {
   r.h.ah = fn;
   intdosx(&r, &r, &s);
   if (r.x.cflag) {
      errno = _doserrno;		/* Note: DOS error number, not errno.h number! */
      return -1;
      }
   else
      return 0;
   }


/* Verify that a file or directory exists.  Mode is ignored.
 * Returns 0 if file or directory exists, -1 if it doesn't.
 */
int access(char *Name, int mode)
   {
   segread(&s);
   s.ds = FP_SEG(Name);
   r.x.dx = FP_OFF(Name);
   r.h.al = 0;
   return dosx(0x43);
   }


/*  sbrkx(incr) - adjust the break value by incr.
 *  Returns the new break value, or -1 if unsuccessful.
 *
 *  Layout of memory after allocation.  Data from INTER.ASM and V37.ASM
 *  is copied to this region after it is allocated.
 *
 *  ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» mindata_ds:0
 *  º             ³  2 header words used by malloc  º
 *  º             ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ mindata_ds:minbdata
 *  º             ³                                 º (malloc begins here)
 *  º  INTER.ASM: ³                                 º
 *  º             ³   Data for INTER.ASM            º
 *  º             ³                                 º
 *  º             ³                                 º
 *  º             ³                                 º
 *  ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º            MINIMAL data for V37.ASM           º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ END_MIN_DATA
 *  º                                               º
 *  º          STACK (size stacksiz, even)          º
 *  º                                               º
 *  ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º          HEAP (size memincb, even)            º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  º                                               º
 *  ×ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ endofmem
 *  º 2 unused bytes to round up to end of segment  º
 *  ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
 *
 */
void *sbrkx(incr)
long incr;
   {
   void *result;
   extern int far minbdata;
   static unsigned long endofmem;
   static unsigned int base = 0;		/* base of the sbrk region */
   static unsigned int curr;
   unsigned int  zminimal;				/* size of V37DATA */
   unsigned int far *p;
   unsigned int far *q;
   if (!base)
   {									/* if need to initialize     */
      endofmem = FP_OFF(GET_DATA_OFFSET(END_MIN_DATA, void *));
	  zminimal = (unsigned int)endofmem;

	  endofmem += (uword)stacksiz;
	  endofmem += (uword)memincb;

	  if (endofmem >= 0x10000L ||
	   (q = (unsigned int far *)malloc(endofmem - FP_OFF(&minbdata))) == 0) {
         wrterr("Insufficient memory for heap and stack");
		 __exit(1);
	     }	 
	  malloced_mem = (void far *)q;		/* remember malloc'ed memory */
      mindata_ds = FP_SEG(q);
	  
	  /* Minimal stack is below heap */
      lowsp = MK_FP(mindata_ds, zminimal);
	  curr = base = FP_OFF(lowsp) + stacksiz;
	  topmem = MK_FP(mindata_ds, endofmem);

	  p = (unsigned int far *)&minbdata;
	  zminimal -= FP_OFF(&minbdata);
	  zminimal >>= 1;
	  while (zminimal--)
         *q++ = *p++;
   }

   if ((unsigned long)(uword)curr + incr > endofmem)
      return (void *)-1;
   result = MK_FP(mindata_ds,curr);
   curr += incr;
   return result;
   }


/*  brk(addr) - set the break address to the given value.
 *  returns 0 if successful, -1 if not.
 * 
 *  Be careful about sign extension in sbrk argument.
 */
int brk(addr)
void *addr;
   {
   
   if ((char *)addr >= (char *)sbrkx(0))
      addr = sbrkx((uword)addr - (uword)sbrkx(0));
   else
      addr = sbrkx((char *)addr - (char *)sbrkx(0));
   return addr == (void *)-1 ? -1 : 0;
   }



/* Close file whose handle is "F". */
int close(int F)
   {
   r.x.bx = F;
   if (dos(0x3e))
      return -1;
   else
      return 0;
   }

/* Duplicate file whose handle is "F". */
int dup(int F)
   {
   r.x.bx = F;
   if (dos(0x45))
      return -1;
   else
      return r.x.ax;
   }


/* Reposition the file pointer assoicated with "F" to location "Loc" */
/* according to the method specified in "Method". */
/* Return the new position. */
long lseek(int F, long Loc, int Method)
   {
   if (chrdev(F)) return -1;
   r.h.al = Method;
   r.x.bx = F;
   r.x.cx = Loc >> 16;
   r.x.dx = Loc;
   if (dos(0x42))
      return -1L;
   return ((unsigned long)r.x.dx << 16) | r.x.ax;
   }


#define DOS_EACCES 5
#define DOS_ENOENT 2

int dos_version(void)
   {
   dos(0x30);
   return r.h.al;
   }

int dos_creat(char *Name)
   {
   segread(&s);
   s.ds = FP_SEG(Name);
   r.x.dx = FP_OFF(Name);
   r.x.cx = 0;						/* attribute */
   dosx(0x3c);
   return r.x.ax;
   }

static int dos_open(char *Name, int Share)
   {
   segread(&s);
   s.ds = FP_SEG(Name);
   r.x.dx = FP_OFF(Name);
   r.h.al = (unsigned char)Share;
   dosx(0x3d);
   return r.x.ax;
   }

static int dos_extended_open(char *Name, int OpenMode, int Attributes, int Flags)
   {
   segread(&s);
   s.ds = FP_SEG(Name);
   r.h.al = 0;
   r.x.bx = (unsigned)OpenMode;
   r.x.cx = (unsigned)Attributes;
   r.x.dx = (unsigned)Flags;
   r.x.si = FP_OFF(Name);
   dosx(0x6C);							/* extended open/create */
   return r.x.ax;
   }

/* Open file "Name" for reading, writing, or updating.  Mode supplies
 * the sharing modes (IO_DENY_XX), IO_PRIVATE and IO_EXECUTABLE flags.
 * Modified by Catspaw to approximate the Unix version of open, which
 * allows O_CREAT and O_TRUNC.  Also take care of the fact that DOS function
 * 60 (create) does not work for CON, PRN, etc.
 */
int spit_open(char *Name, int Method, int Mode, int Action)
   {
   int F;
   int OpenMode;
   int save = errno;
   errno = 0;
   
   if ((Method & MethodMask) == O_RDONLY) {	/* if opening for read only */
      Method &= ~(O_CREAT | O_TRUNC);		/* guarantee these bits off */
      OpenMode = For_reading;
      }
   else if ((Method	& MethodMask) == O_WRONLY) /* if opening for write only */
      OpenMode = For_writing;
   else										/* else opening for read/write */
      OpenMode = For_updating;
   
   OpenMode |= ((Mode & IO_DENY_MASK) << 4);	/* set sharing mode */
   OpenMode |= Mode & IO_PRIVATE;
   if (dos_version() < 4) {
      if (Method & O_TRUNC) {					/* if truncate specified */
         F = dos_creat(Name);					/*  create and truncate file. */
	     if (!errno) {
	  	   close(F);                           /* close file and reopen with */
		   Method &= ~O_TRUNC;					/* correct sharing modes	*/
	  	   }	
         else if (errno == DOS_EACCES) {			/* might be char dev */
            errno = 0;
            F = dos_open(Name, OpenMode);
            }
         }
      if ((Method & O_TRUNC) == 0) {			/* no truncate */
         F = dos_open(Name, OpenMode);
         if ((errno == DOS_ENOENT) && (Method & O_CREAT)) {
            errno = 0;						/* did not exist - create it */
            F = dos_creat(Name);
            }
         }
      }
   else {
   	  /* Use extended open if DOS 4.x or higher */
      if (Action & IO_WRITE_THRU)
         OpenMode |= OPEN_FLAGS_WRITE_THROUGH;	/* turn off O/S buffering */
	  F = dos_extended_open(Name, OpenMode, FILE_NORMAL, Action & IO_EXIST_ACTION_MASK /* Flags */);
      }	   
   
   if (errno)
      return -1;
   else {
      errno = save;
      return F;
      }
   }

/*
 * Test if file whose handle is "F" is a charcter device.
 * Return low-order device byte if character device, 0 if not.
 */
int chrdev(int F)
   {
   r.x.bx = F;
   r.h.al = 0;				/* ioctl get status */
   if (dos(0x44))
      return 0;
   if (r.h.dl & 0x80)
      return (unsigned)r.h.dl;	/* character device */
   else
      return 0;					/* disk device */
   }

/*
 * Test if file whose handle is "F" is a charcter device AND console input.
 * Return 0 if so, -1 if not.
 */
int cindev(int F)
   {
   if ((chrdev(F) & 0x81) == 0x81)
      return 0;				/* character console in */
   else
      return -1;			/* nope */
   }

/*
 * Test if file whose handle is "F" is a charcter device AND console output.
 * Return 0 if so, -1 if not.
 */
int coutdev(int F)
   {
   if ((chrdev(F) & 0x82) == 0x82)
      return 0;				/* character console out */
   else
      return -1;			/* nope */
   }


/*
 *  Set/clear rawmode for file whose handle is "F".
 *  Mode is 0 to clear, non-zero to set.
 */
void rawmode(int F, int mode)
   {
   int info;

   info = chrdev(F);
   if (!info)
      return;							/* if not character device */
   r.x.dx = info & 0x00DF;		   		/* clear DH, clear raw bit */
   if (mode)
      r.h.dl |= 0x20;					/* set raw bit if should   */
   r.x.bx = F;
   r.h.al = 1;							/* ioctl set device info   */
   dos(0x44);
   }

/* Read "Cnt" bytes from file "F" into buffer whose address is "Buf".
 * Returns the number of bytes actually read.
 *
 *  NOTE THAT readfar() TAKES A FAR POINTER TO THE BUFFER
 */
uword readfar(int F, void FAR *Buf, uword Cnt)
   {
   segread(&s);
   s.ds = FP_SEG(Buf);
   r.x.dx = FP_OFF(Buf);
   r.x.cx = Cnt;
   r.x.bx = F;
   if (dosx(0x3F))
      return -1;
   else
      return r.x.ax;
   }

/* Read "Cnt" bytes from file "F" into buffer whose address is "Buf".
 * Returns the number of bytes actually read.
 */
word read(int F, void *Buf, uword Cnt)
   {
#ifdef __FARDATA__
   return readfar(F, Buf, Cnt);
#else
   r.x.dx = (unsigned)Buf;
   r.x.cx = Cnt;
   r.x.bx = F;
   if (dos(0x3F))
      return -1;
   else
      return r.x.ax;
#endif					/* __FARDATA__ */
   }


/* Rename a file.  Return 0 if OK */
int rename(char *oldname, char *newname)
   {
   segread(&s);
   s.ds = FP_SEG(oldname);
   r.x.dx = FP_OFF(oldname);
   s.es = FP_SEG(newname);
   r.x.di = FP_OFF(newname);
   return dosx(0x56);
   }



/* Shell to another program.
 * 
 * Note that for convenience, we require the caller to provide the
 * arg1 character count in arg1[-1].  See dosys.c.
 */
int spawnl(int mode, char *path, char *arg0, char *arg1, ...)
   {
   struct PBLOCK {
      unsigned short envb_seg;
	  char far 		 *cmd;
	  struct fcb far *fcb1;
	  struct fcb far *fcb2;
	  } pb;
   struct fcb dummy;

   pb.envb_seg = 0;
   pb.cmd = arg1-1;
   pb.fcb1 = pb.fcb2 = &dummy;
   dummy.fcb_drive = 0xfe;			/* illegal drive letter */

   segread(&s);
   s.ds = FP_SEG(path);
   r.x.dx = FP_OFF(path);
   s.es = FP_SEG(&pb);
   r.x.bx = FP_OFF(&pb);
   r.h.al = 0;
   r.h.ah = 0x4B;
   intdosx(&r, &r, &s);
   if (r.x.cflag) {
      errno = r.x.ax;		/* Note: DOS error number, not errno.h number! */
      return -1;
      }
   dos(0x4D);				/* Get child return code */
   return r.h.al;
   }


/* Erase file from directory. */
int unlink(char *Name)
   {
   segread(&s);
   s.ds = FP_SEG(Name);
   r.x.dx = FP_OFF(Name);
   return dosx(0x41);
   }


/* Write "Cnt" bytes from buffer whose address is "Buf" into file "F".
 *
 *  NOTE THAT writefar() TAKES A FAR POINTER TO THE BUFFER
 */
uword writefar(int F, void FAR *Buf, uword Cnt)
   {
   segread(&s);
   s.ds = FP_SEG(Buf);
   r.x.dx = FP_OFF(Buf);
   r.x.cx = Cnt;
   r.x.bx = F;
   if (dosx(0x40))
      return -1;
   else
      return r.x.ax;
   }

/* Write "Cnt" bytes from buffer whose address is "Buf" into file "F".
 *
 */
word write( File_handle F, void *Buf, uword Cnt )
   {
#ifdef __FARDATA__
   return writefar(F, Buf, Cnt);
#else
   r.x.dx = (unsigned)Buf;
   r.x.cx = Cnt;
   r.x.bx = F;
   if (dos(0x40))
      return -1;
   else
      return r.x.ax;
#endif					/* __FARDATA__ */
   }


/*
 * localtime()
 * 
 * This is a replacement for the library version of this routine.
 * We do this because TurboC's localtime routine is in a module
 * together with asctime, bringing it in even though we don't use
 * it.  asctime in turn brings in sprintf and all its baggage.
 * 
 * Note that this is a restricted version of localtime, and only
 * provides the results needed by SYSDT.
 */
static struct tm t;
struct tm *localtime(const time_t *timer)
   {
   dos(0x2a);						/* Get DOS date */
   t.tm_year = r.x.cx - 1900;
   t.tm_mon = r.h.dh - 1;
   t.tm_mday = r.h.dl;
   dos(0x2c);						/* Get DOS time */
   t.tm_hour = r.h.ch;
   t.tm_min = r.h.cl;
   t.tm_sec = r.h.dh;
   return &t;
   }
/*
 * time()
 * 
 * This replacement for the library version simple returns a 0.
 * It is only used by sysxi for stashing in the header of a save file.
 */
time_t time(time_t *timer)
   {
   if (timer)
      *timer = 0;
      return 0;
   }



/*
 * checksave - Check if we are being started from a save file.
 * 
 * Input: Name of file to inspect.
 * 
 * Returns 0 if not, else it returns the file handle of a file
 * positioned to the first byte of the save information.
 */
int checksave(char *namep)
{
   int fd;
   uword size;
   struct MZHDR mzhdr;
   long position;
   
   fd = spit_open(namep, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE, IO_OPEN_IF_EXISTS);
   if (fd == -1)
      return 0;

   size = read(fd, (void *)&mzhdr, sizeof(struct MZHDR));
   if (size == sizeof(struct MZHDR) && mzhdr.overlay) {
      position = 0x200L*mzhdr.size_blks + mzhdr.size_mod - 0x200L;
      if (lseek(fd, position, 0) == position)
         return fd;
      }

   close(fd);					/* Failure */
   return 0;
}


/*
 * openexe - open SPITBOL's exe file (the one we are executing from)
 * 			 for read-only input.
 */
int openexe(char *name)
{
   return spit_open( name, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE, IO_OPEN_IF_EXISTS );
}

/*
 * saveend - Write save file and any closing information.
 * 
 * After copying the SPITBOL executable to the target file, saveend
 * writes the Save file and updates any other information in the target file.
 * 
 * Here after copying body of SPITBOL EXE file.  Now ready to append
 * the save file.
 */
word saveend(word *stkbase, word stklen)
{
   return putsave(stkbase, stklen);	/* append save file */
}


/* savestart - perform actions prior to writing save file
 * 			   for an executable module.
 * 
 * We read the information from the SPITBOL executable header,
 * and return the number of bytes of the EXE file to copy.
 * 
 * Input: bufp - pointer to buffer containing the first
 * 		  size - size bytes from the file.
 * 		  fd   - input file.  The file is positioned just past the
 * 		         first size bytes.
 * 
 * Output: Number of total bytes to copy from the file, or 0 if
 * 		   there's something wrong with the file.
 * 
 *         Prior to copying the rest of the file, the size bytes at
 *		   bufp should be written out by the caller.  That is,
 * 		   any changes made in the buffer by savestart are written
 * 		   to the new file.
 *
 */
long savestart(int fd, char *bufp, unsigned int size)
{
#define mzp ((struct MZHDR *)bufp)
   mzp->overlay = 1;
   return 0x200L*mzp->size_blks + mzp->size_mod - 0x200L;
}

#if EXTFUN
typedef struct xnblk XFNode, *pXFNode;

static union block *scanp;			/* used by scanef/nextef */
static pXFNode xnfree = (pXFNode)0;	/* list of freed blocks */

extern IATYPE far f_2_i(double ra);
extern double far i_2_f(IATYPE ia);
extern double far f_add(double arg, double ra);
extern double far f_sub(double arg, double ra);
extern double far f_mul(double arg, double ra);
extern double far f_div(double arg, double ra);
extern double far f_neg(double ra);
extern double far f_atn(double ra);
extern double far f_chp(double ra);
extern double far f_cos(double ra);
extern double far f_etx(double ra);
extern double far f_lnf(double ra);
extern double far f_sin(double ra);
extern double far f_sqr(double ra);
extern double far f_tan(double ra);

static APDF flttab = {
	(double far (*)())f_2_i,		/* float to integer */
	i_2_f,			/* integer to float */
	f_add,			/* floating add */
	f_sub,			/* floating subtract */
	f_mul,			/* floating multiply */
	f_div,			/* floating divide */
	f_neg,			/* floating negage */
	f_atn,			/* arc tangent */
	f_chp,			/* chop */
	f_cos,			/* cosine */
	f_etx,			/* exponential */
	f_lnf,			/* natural log */
	f_sin,			/* sine */
	f_sqr,			/* square root */
	f_tan			/* tangent */
	};
	
misc miscinfo = {0x105,				/* internal version number */
#if IA32
	t_tc32,							/* environment */
#else
	t_tc16,
#endif
	0,								/* spare */
	0,								/* number of arguments */
	0,								/* pointer to type table */
	0,								/* pointer to XNBLK */
	0,								/* pointer to EFBLK */
	(APDF *)flttab,					/* pointer to flttab */
	};								/* DS, CS filled later */

extern void callback(pXFNode pnode);
extern word callextfun(struct efblk *efb, union block **sp, word nargs);


/*
 * callef - procedure to call external function.
 *
 *	result = callef(efptr, xsp, nargs)
 *
 *	   efptr	pointer to efblk
 *	   xsp		pointer to arguments
 *	   nargs	number of arguments
 *	   result	 0 - function should fail
 *				-1 - insufficient memory to convert arg (not used)
 *				     or function not found.
 *				-2 - improper argument type (not used)
 *				other - block pointer to function result
 *
 * Note the duplication of the initialization code from loadef().  This is because
 * on the reload of a save file, we could come to callef without every calling loadef.
 *
 */
union block *callef(struct efblk *efb, union block **sp, word nargs)
   {
   union block *result;
   pXFNode pnode = MK_MP(efb->efcod, pXFNode);
   static initsels = 0;
   static word far (*pTYPET)[];
   word type, length;
   char far *p;
   char *q;
   word far *r;
   word *s;
   
   if (pnode == NULL)
      return (union block *)-1L;

   if (!initsels) {						/* one-time initializations */
   	  /* Kludge:  TYPET is in C's data space */
      pTYPET = (word far (*)[])MK_FP(FP_SEG(&initsels),GET_DATA_OFFSET(TYPET,uword));
	  miscinfo.ptyptab = pTYPET;		/* pointer to table of data types */
      miscinfo.spds = FP_SEG(miscinfo.ptyptab);
	  miscinfo.spcs = FP_SEG(callef);
      initsels++;
      }  
   
   miscinfo.pefblk = efb;				/* save efblk ptr in misc area */
   miscinfo.pxnblk = pnode;
   miscinfo.nargs = nargs;

/* Fiddle the xn1st and xnsave words in the xnblk.  If either is zero,
 * it is left alone.  If either is non-zero, it is decremented.  This
 * has the effect of providing the function with a 1 or a 0 if this is
 * the first call or a subsequent call respectively.
 */
   if (pnode->xnu.ef.xn1st)
      pnode->xnu.ef.xn1st--;
   if (pnode->xnu.ef.xnsave)
      pnode->xnu.ef.xnsave--;

   type = callextfun(efb, sp, nargs);
   result = (union block *)pTSCBLK;
   switch (type) {
	  
	  case BL_XN:						/* XNBLK    external block 						*/
		 result->xnb.xnlen = ((result->xnb.xnlen + sizeof(word) - 1) &
		  -sizeof(word)) + FIELDOFFSET(struct xnblk, xnu.xndta[0]);
	  case BL_AR:	 					/* ARBLK	array								*/
	  case BL_CD:						/* CDBLK	code								*/
	  case BL_EX:						/* EXBLK	expression							*/
	  case BL_IC:						/* ICBLK	integer								*/
	  case BL_NM:						/* NMBLK	name								*/
	  case BL_P0:						/* P0BLK	pattern, 0 args						*/
	  case BL_P1:						/* P1BLK	pattern, 1 arg						*/
	  case BL_P2:						/* P2BLK	pattern, 2 args						*/
	  case BL_RC:						/* RCBLK	real								*/
	  case BL_SC:						/* SCBLK	string								*/
	  case BL_SE:						/* SEBLK	expression							*/
	  case BL_TB:						/* TBBLK	table								*/
	  case BL_VC:						/* VCBLK	vector (array)						*/
	  case BL_XR:						/* XRBLK	external, relocatable contents		*/
	  case BL_PD:						/* PDBLK	program defined datatype			*/
	  	 result->scb.sctyp = (*pTYPET)[type];
         break;
		       
	  case BL_NC:	 					/* return result block unchanged */
         break;
	  
	  case BL_FS:						/* string pointer at TSCBLK.str */
		 result->fsb.fstyp = (*pTYPET)[BL_SC];
		 p = result->fsb.fsptr;
		 length = result->fsb.fslen;
		 if (!length)
		 	break;						/* return null string result */
         MINSAVE();
         SET_WA(length);
         MINIMAL(ALOCS);				/* allocate string storage */
         result = XR(union block *);
         MINRESTORE();
		 q = result->scb.scstr;
		 while (length--)
		 	*q++ = *p++;
		 break;
	  
	  case BL_FX:						/* pointer to external data at TSCBLK.str */
		 length = ((result->fxb.fxlen + sizeof(word) - 1) &
		  -sizeof(word)) + FIELDOFFSET(struct xnblk, xnu.xndta[0]);
	     if (length > GET_MIN_VALUE(MXLEN,word)) {
		    result = (union block *)0;
			break;
		    }
		 r = result->fxb.fxptr;
         MINSAVE();
         SET_WA(length);
         MINIMAL(ALLOC);				/* allocate block storage */
         result = XR(union block *);
         MINRESTORE();
	     result->xnb.xnlen = length;	 
		 result->xnb.xntyp = (*pTYPET)[BL_XN];
		 s = result->xnb.xnu.xndta;
		 length = (length - FIELDOFFSET(struct xnblk, xnu.xndta[0])) / sizeof(word);
	     while (length--)
		    *s++ = *r++;
	     break;
	  
   	  case FAIL:						/* fail */
      default:
		 result = (union block *)0;
   	     break;
	  }	
   return result;
   }

/*
 * loadef - load external function
 *
 *	result = loadef(fd,filename)
 *
 *	Input:
 *	   fd		file descriptor of file to load
 *	   filename	zero-terminated string of file name to load
 *	Output:
 *	 result	 0 - I/O error
 *			-1 - function doesn't exist (not used)
 *			-2 - insufficient memory
 *			other - pointer to XNBLK that points in turn
 *			        to the loaded code (here as void *).
 */
void *loadef(word fd, char *filename)
   {
   register pXFNode pnode;
   char rbuf[2];

   if (xnfree) {							/* Are these any free nodes to use? */
      pnode = xnfree;						/* Yes, seize one */
      xnfree = (pXFNode)pnode->xnu.ef.xnmemp;
      }
   else {
      MINSAVE();							/* No */
      SET_WA(sizeof(XFNode));
      MINIMAL(ALOST);						/* allocate from static region */
      pnode = XR(pXFNode);					/* get node to hold information */
      MINRESTORE();
      }
   
   pnode->xntyp = TYPE_XNT;					/* B_XNT type word */
   pnode->xnlen = sizeof(XFNode);			/* length of this block */
   pnode->xnu.ef.xncall = (int far (*)())0;	/* fill in node */
   pnode->xnu.ef.xnmemp = (void *)0;
   pnode->xnu.ef.xn1st = 2;					/* flag first call to function */
   pnode->xnu.ef.xnsave = 0;				/* not reload from save file */
   pnode->xnu.ef.xncbp = (void far (*)())0;		/* no callback prior to writing save file */

   if (read(fd, (void *)&rbuf, 2) == 2 && rbuf[0] == 'M' && rbuf[1] == 'Z' )
      goto loadfail;
   pnode->xnu.ef.xnsiz = lseek(fd, 0, 2);	/* get size of COM file */
   if (pnode->xnu.ef.xnsiz == -1 || pnode->xnu.ef.xnsiz >= 0x10000)		/* I/O error or >= 64K */
      goto loadfail;
   pnode->xnu.ef.xnmemp = (int far (*)())malloc(pnode->xnu.ef.xnsiz+16);
   if (!pnode->xnu.ef.xnmemp)
      goto loadfail;
   pnode->xnu.ef.xncall = MK_FP(FP_SEG(pnode->xnu.ef.xnmemp)+1-0x10,0x100); /* must have offset=100h */
   pnode->xnu.ef.xnds = pnode->xnu.ef.xnes = pnode->xnu.ef.xncs = FP_SEG(pnode->xnu.ef.xncall);
   pnode->xnu.ef.xnsp = pnode->xnu.ef.xnss = 0;				/* COM file with use SPITBOL's stack */
   lseek(fd, 0, 0);							/* rewind COM file */
   if (read(fd, (void *)pnode->xnu.ef.xncall, pnode->xnu.ef.xnsiz) != pnode->xnu.ef.xnsiz) {
   	  free(pnode->xnu.ef.xnmemp);
      goto loadfail;
      }  
   return (void *)pnode;					/* Return node to store in EFBLK */
   
loadfail:
   pnode->xnu.ef.xnmemp = (void *)xnfree;	/* put back on free list */
   xnfree = pnode;
   return (void *)0;
   }

/*
 * nextef - return next external function block.
 *
 * 		length = nextef(.bufp, io);
 *
 * Input:
 * bufp = address of pointer to be loaded with block pointer
 * io = -1 = scanning memory
 *		0 if loading functions
 *		1 if saving functions or exiting SPITBOL
 *
 * Output:
 *  for io = -1:
 *		length = pointer to XNBLK
 *				 0 if done
 *		bufp   = pointer to EFBLK.
 *
 *  for io = 0,1:
 * 		length = length of function's memory block
 *	    		 0 if done
 *	    		 -1 if unable to allocate memory (io=0 only)
 *		bufp   = pointer to function body.
 *
 * When io = 1, we invoke any callback routine established by the
 * external function if it wants to be notified when SPITBOL is
 * shutting down.  xnsave set to -1 to preclude multiple callbacks.
 *
 * When io = 0, the routine will allocate the memory needed to
 * hold the function when it is read from a disk file.
 *
 * When io = -1, nextef takes no special action, and simple returns the
 * address of the next EFBLK found.
 *
 * The current scan point is in scanp, established by scanef.
 */
void *nextef( unsigned char FAR **bufp, int io )
   {
   union block *dnamp;
   word ef_type = TYPE_EFC;
   void *result = 0;
   word type, blksize;
   pXFNode pnode;

   MINSAVE();
   for (dnamp = GET_MIN_VALUE(DNAMP,union block *);
    scanp < dnamp; scanp = MK_MP(MP_OFF(scanp,uword)+blksize, union block *)) {
      type = scanp->scb.sctyp;				/* any block type lets us access type word */
      SET_WA(type);
      SET_XR(scanp);
      MINIMAL(BLKLN);						/* get length of block in bytes */
      blksize = WA(word);
      if (type != ef_type)					/* keep searching if not EFBLK */
         continue;
      pnode = MK_MP(scanp->efb.efcod, pXFNode);	/* it's an EFBLK; get address of XNBLK */
      if (!pnode)							/* keep searching if no longer in use */
         continue;

      switch (io) {
         case -1:
            result = (void *)pnode;				/* return pointer to XNBLK */
            *bufp = (unsigned char FAR *)scanp;	/* return pointer to EFBLK */
            break;
         case 0:
            pnode->xnu.ef.xnmemp = (void *)malloc(pnode->xnu.ef.xnsiz+16);	/* allocate memory */
            if (!pnode->xnu.ef.xnmemp) {
               result = (void *)-1;
               break;							/* break out of switch */
			   }      
			pnode->xnu.ef.xncall = MK_FP(FP_SEG(pnode->xnu.ef.xnmemp)+1-0x10,0x100);
            pnode->xnu.ef.xnds = pnode->xnu.ef.xnes = pnode->xnu.ef.xncs = FP_SEG(pnode->xnu.ef.xncall);
            pnode->xnu.ef.xnsave = 2;					/* set flag to indicate reload */
            *bufp = (unsigned char FAR *)pnode->xnu.ef.xncall;
     	    result = (void *)(pnode->xnu.ef.xnsiz);	  /* size of code */
			break;
         case 1:
		    if (pnode->xnu.ef.xncbp)				/* is there a callback routine */
			   callback(pnode);
            *bufp = (unsigned char FAR *)pnode->xnu.ef.xncall;
     	    result = (void *)(pnode->xnu.ef.xnsiz);	  /* size of code */
            break;
         }
      scanp = MK_MP(MP_OFF(scanp,uword)+blksize, union block *);	/* point to next block */
      break;									/* break out of for loop */
      }
   MINRESTORE();
   return result;
   }


/*
 * scanef - prepare to scan memory for external function blocks.
 */
void scanef()
   {
   scanp = GET_MIN_VALUE(DNAMB,union block *);
   }


/*
 * unldef - unload an external function
 */
void unldef(struct efblk *efb)
   {
   pXFNode pnode, pnode2;
   unsigned char FAR *bufp;

   pnode = MK_MP(efb->efcod, pXFNode);
   if (pnode == NULL)
      return;
   
   callback(pnode);
   
   efb->efcod = 0;						/* remove pointer to XNBLK */
   

/*
 * scan efblks to see if any other function is using this code.  If no
 * other users, it is safe to free the module.  Two functions could use
 * the same code in the case where a DLL exports more than one function
 * name.
 *
 * Note that the scan will not find our own EFBLK because we zero'ed
 * the efcod word above, and nextef() ignores EFBLKs with null XNBLK
 * pointers.
 */
   scanef();					/* prepare to scan for external functions */
   while ((pnode2 = (pXFNode)nextef(&bufp, -1)) != (pXFNode)0)
      if (pnode == pnode2)
	     return;				/* still in use, don't release it */
   
   free(pnode->xnu.ef.xnmemp);
   
   pnode->xnu.ef.xnmemp = (void *)xnfree; /* put back on free list */
   xnfree = pnode;
   }

#endif 					/* EXTFUN */

#if EXECFILE | SAVEFILE
/*
 *-----------
 *
 *	makeexec - C callable make executable file function
 *
 *	Allows C function zysbx() to create executable files
 *	(a.out files) in response to user's -w command option.
 *
 *	SPITBOL performed a garbage collection prior to calling
 *	SYSBX, so there is no need to duplicate it here.
 *
 *	Then zysxi() is invoked directly to write the module.
 *
 *	int makeexec( struct scblk *scptr, int type);
 *
 *	Input:	scptr = Pointer to SCBLK for load module name.
 *			type = Type (+-3 or +-4)
 *	Output:	Result value <> 0 if error writing a.out.
 *		Result = 0 if type was |4| and no error.
 *		No return if type was |3| and file written successfully.
 *		When a.out is eventually loaded and executed, the restart
 *		code jumps directly to RESTART in the Minimal source.
 *		That is, makeexec is not resumed, since that would involve
 *		preserving the C stack and registers in the load module.
 *
 *		Upon resumption, the execution start time and garbage collect
 *		count are reset appropriately by restart().
 *
 */
int makeexec( scptr, type )
struct scblk FAR *scptr;
int type;
{
	word	save_wa, save_wb, save_ia, save_xr;
	int		result;

	/* save zysxi()'s argument registers (but not XL)  */
	save_wa = reg_wa;
	save_wb = reg_wb;
	save_ia = reg_ia;
	save_xr = reg_xr;

	reg_wa = (word)scptr;
	reg_xl = 0;
	reg_ia = type;
	reg_wb = 0;
	reg_xr = GET_DATA_OFFSET(HEADV,word);

	/*  -1 is the normal return, so result >= 0 is an error */
	result = zysxi() + 1;

	reg_wa = save_wa;
	reg_wb = save_wb;
	reg_ia = save_ia;
	reg_xr = save_xr;
	return result;
}
#endif	/* EXECFILE | SAVEFILE */
