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
 * extcall.c - MaxSPITBOL version to call external XCMD/XFCNs.
 *
 * This interface provides two types of calls, both accessible from LOAD():
 *  1.  Calls to XCMDs and XFCNs.
 *  2.  Calls to arbitrary code resources.
 *
 * XCMD/XFCN calls provide a string-based interface.
 * It is identified by invoking LOAD() with "XFCN:ResourceName" or "XCMD:ResourceName"
 * as the second argument
 * where:
 *  "type" is "XCMD" or "XFNC".
 *  "resource name" is used to select a resource by name.
 *
 * During the actual function call, up to 16 additional arguments are converted to
 * strings, copied to handles, and placed in the XCmdBlock structure.
 *
 * Code resource calls provide a raw interface, and are recognized by
 * the resource type *not* being "XCMD" or "XFCN".
 */

#include "comptype.h"
#include "features.h"

#ifdef ExternalFunctions
#include "SPITBOLXCmd.h"
#include "PEdit.h"

#include "port.h"

#if APPLE
#define malloc mlalloc
extern	long		lstrlen(char *s);
#endif


#ifdef WindowCallbacks
#include "wproto.h"
#endif

/* The following node is constructed in the SPITBOL static region as an XNBLK */
typedef struct XFNode {				/* record information about external function */
   word				xntyp;				/* B_XNT type word */
   word				xnlen;				/* length of this block */
   void 			(**hcode)();		/* handle to code */
   word				xnsiz;				/* size of function in bytes */
   struct XFNode	*xnnext;			/* To link nodes on xnfree list */
   word				native;				/* true if native function, false if XCMD/XFCN */
   } XFNode, *pXFNode;

static	struct scblk *xcmdcall(struct efblk *efb, union block **sp, word nargs,
		     		 void (*pcode)());
static union block *scanp;			/* used by scanef/nextef */
static pXFNode xnfree;				/* list of freed blocks */

XCmdBlockPtr pxBlock = NULL;		/* parameter record for XCMD/XFNC */

extern	void		CopyStr(StringPtr srcStr, StringPtr dstStr);
#ifdef WindowCallbacks
extern	void		enduserwin(void);
#endif
extern	void		endxcmd(void);
extern	Boolean		initxcmd(void);
extern	void		xcmdcallback(void);
extern	void		unloadXF(void);

/*
 * callef - procedure to call external function.
 *
 * Used to call XCMD/XFNCs or pure code resources.
 *
 * Possible returns:
 *   Success: pointer to block result.
 *   Failure: 0L.
 *   Insufficient memory: -1L
 *   Bad arg type: -2L
 *
 * TEMP VARIABLES MUST BE WORD ALIGNED AND COLLECTABLE.
 *
 * Note the duplication of the initialization code from loadef().  This is because
 * on the reload of a save file, we could come to callef without every calling loadef.
 *
 */
struct scblk *callef(efb, sp, nargs)
struct efblk *efb;
union block **sp;
word nargs;
   {
   void (*pcode)();
   void (**hcode)();
   struct scblk *result = 0;			/* initialize so collectable */
   pXFNode pnode;

   if (pxBlock == NULL) {
      if (!initxcmd())						/* initialize things */
         return (void *)-1;					/* insufficient memory */
      xnfree = (pXFNode)NULL;			 	/* empty list of freed XFNode */
      }
    
   pnode = MK_MP(efb->efcod,pXFNode);
   if (pnode == NULL)
      return (struct scblk *)0;

   hcode = pnode->hcode;
   if (hcode == NULL)
      return (struct scblk *)0;
         
   /* Move code up in memory, lock it, and initiate call */
   MoveHHi((Handle)hcode);
   HLock((Handle)hcode);
   
#ifdef WindowCallbacks
   extcallbusy(true);
#endif

   pcode = (void (*)())QuickStrip(*hcode);
   if (!pnode->native)
      result = xcmdcall(efb, sp, nargs, pcode);
   else {

#ifdef NativeExternals
      asm {
         move.l	a4,-(a7)		; save my a4
         move.l	pcode,a4		; setup a4 for external function
         }
      result = (struct scblk *)CallPascalL(efb, sp, nargs, xcodecallback, pcode);
      asm {
         move.l	(a7)+,a4		; restore a4
         }
#else
      result = (struct scblk *)0L;
#endif
      }

   HUnlock((Handle)hcode);

#ifdef WindowCallbacks
   extcallbusy(false);
#endif

   return result;
   }


/*
 * loadef - load external function
 *
 * library name is of the form "Type:ResourceName".
 *  Type defaults to XFCN.
 *  ResourceName defaults to function name.
 *
 * Returns 0 if I/O error, -1 if function doesn't exist, -2 if insufficient memory,
 * otherwise pointer to block containing information about function.
 *
 * The resource is loaded and marked NoPurge.
 */
void *loadef(fd, filename)
word fd;
char *filename;
   {
   register struct scblk *lnscb = XL (struct scblk *);
   register struct scblk *fnscb = XR (struct scblk *);
   char *savecp;
   char savechar;
   char *lp = lnscb->str;
   word len = lnscb->len;
   ResType type = 'XFCN';
   Handle h;
   pXFNode pnode;

   if (pxBlock == NULL) {
      if (!initxcmd())						/* initialize things */
         return (void *)-2;					/* insufficient memory */
      xnfree = (pXFNode)NULL;			 	/* empty list of freed XFNode */
      }
    
   if (len > sizeof(ResType) && lp[sizeof(ResType)] == ':') {
      type = *((ResType *)lp);				/* Unpack Type string if present */
      len -= sizeof(ResType);
      lp +=  sizeof(ResType);
      }

   if (len && *lp == ':') {					/* Remove leading colon, if any */
      lp++;
      len--;
      }
	
   if (len == 0) {							/* If no library name, use function name */
      lp = fnscb->str;
      len = fnscb->len;
      }

   savecp = lp + len;
   savechar = *savecp;
   *savecp = '\0';

   CtoPstr(lp);
   h = GetNamedResource(type, (StringPtr)lp);
   PtoCstr(lp);

   *savecp = savechar;						/* Restore saved char */

   if (!GoodResource(h))
      return (void *)-1L;					/* Function doesn't exist (or no room) */

   DetachResource(h);						/* Keep our own copy around */

   if (xnfree) {							/* Are these any free nodes to use? */
      pnode = xnfree;						/* Yes, seize one */
      xnfree = pnode->xnnext;
      }
   else {
#pragma options(!defer_adjust)
      MINSAVE();							/* No */
      SET_WA(sizeof(XFNode));
      MINIMAL(ALOST);						/* allocate from static region */
      pnode = XR(pXFNode);					/* get node to hold information */
      MINRESTORE();
      }
   
   pnode->xntyp = TYPE_XNT;					/* B_XNT type word */
   pnode->xnlen = sizeof(XFNode);			/* length of this block */
   pnode->hcode = (void (**)())h;			/* fill in node */
   pnode->native = !(type == 'XFCN' | type == 'XCMD');
   pnode->xnsiz = GetHandleSize(h);
   pnode->xnnext = (pXFNode)NULL;

   HNoPurge(h);								/* Keep code around */

   return (void *)pnode;					/* Return node to store in EFBLK */
   }

/*
 * nextef - return next external function block.
 *
 * 		length = nextef(.bufp, io);
 *
 * Input:
 * bufp = address of pointer loaded with block pointer
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
 * shutting down.  Callbacks presently not implemented
 *
 * The current scan point is in scanp, established by scanef.
 *
 * When io = -1, nextef takes no special action, and simple returns the
 * address of the next EFBLK found.
 */
void *nextef( unsigned char FAR **bufp, int io )
   {
   union block *dnamp;
   word ef_type = TYPE_EFC;
   void *result = 0;
   word type, blksize;
   pXFNode pnode;

#pragma options(!defer_adjust)
   MINSAVE();
   for (dnamp = GET_MIN_VALUE(DNAMP,union block *);
     scanp < dnamp; scanp = (union block *)((uword)scanp+blksize)) {
      type = scanp->scb.typ;				/* any block type lets us access type word */
      SET_WA(type);
      SET_XR(scanp);
      MINIMAL(BLKLN);						/* get length of block in bytes */
      blksize = WA(word);
      if (type != ef_type)					/* keep searching if not EFBLK */
         continue;
      pnode = MK_MP(scanp->efb.efcod,pXFNode);	/* it's an EFBLK; get address of XNBLK */
      if (!pnode)							/* keep searching if no longer in use */
         continue;

      switch (io) {
         case -1:
            result = (void *)pnode;				/* return pointer to XNBLK */
            *bufp = (unsigned char FAR *)scanp;	/* return pointer to EFBLK */
            break;
         case 0:
            pnode->hcode = (void (**)())NewHandle(pnode->xnsiz);	/* allocate memory */
            if (!GoodNewHandle((Handle)pnode->hcode)) {
               result = (void *)-1;
               break;							/* break out of switch */
               }								/* fall through to case 1 */
         case 1:
            *bufp = (unsigned char FAR *)QuickStrip(*(pnode->hcode)); /* ptr to code */
     	    result = (void *)(pnode->xnsiz);	  /* size of code */
            break;
         }
      scanp = (union block *)((uword)scanp+blksize);	/* point to next block */
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
 * xcmdcall - procedure for XCMD/XFNC call interface.
 *
 * returns 0 for failure, -1 if insufficient memory, -2 if improper argument type.
 *
 * TEMP VARIABLES MUST BE WORD ALIGNED AND COLLECTABLE.
 *
 */
static struct scblk *xcmdcall(efb, sp, nargs, pcode)
struct efblk *efb;
union block **sp;			/* pointer to last argument */
word nargs;
void (*pcode)();
   {
   word slen = 0;			/* initialize so collectable */
   word i, j;
   Handle h = 0;			/* initialize so collectable if no args */
   struct scblk *result = 0;
   struct scblk *arg;
   

   memfill((char *)pxBlock, 0, (unsigned long)sizeof(XCmdBlock));
   pxBlock->entryPoint = xcmdcallback;

   if (nargs > 16) {
      sp += (nargs - 16);		/* ignore extra arguments */
      nargs = 16;			/* no more than 16 arguments */
      }

   pxBlock->paramCount = nargs;

   for (i=nargs, j=0; i--; ) {
      arg = &(sp[i]->scb);		/* convert argument at *(sp + i) */
      if (arg->typ == TYPE_SCL) {

         /* copy string to newly allocated handle */
         if (PtrToHand(arg->str, &h, arg->len + 1) != 0) {
            result = (struct scblk *)-1L;		/* insufficient memory */
            break;
            }
         *(((char *)(*h)) + arg->len) = '\0'; /* guarantee zero terminator */
         pxBlock->params[j++] = h;
         }
      else {
         result = (struct scblk *)-2L;		/* bad argument type */
         break;
         }
      }

   if (!result) {				/* If no error converting arguments */
      asm {
         move.l	a4,-(a7)			; save my a4
         move.l	pcode,a4			; setup a4 for external function
         }
#pragma options(!defer_adjust)
      CallPascal(pxBlock, pcode);		/* call the XCMD or XFCN */
      asm {
         move.l	(a7)+,a4			; restore a4
         }
      if (pxBlock->passFlag)
         result = (struct scblk *)0;		/* fail if it requests a pass through */
      else {
         if (pxBlock->returnValue == NULL ||	/* if no result, or null string */
          (slen = lstrlen(*(pxBlock->returnValue))) == 0) {
            pTSCBLK->typ = TYPE_SCL;	/* return null string */
            pTSCBLK->len = 0;
            result = pTSCBLK;
            }
         else {					/* non-null result */
#pragma options(!defer_adjust)
         	MINSAVE();
         	SET_WA(slen);
            MINIMAL(ALOCS);
            result = XR(struct scblk *);
            MINRESTORE();
            memcopy(result->str, *(pxBlock->returnValue), slen);
             }
         }
      }
   if (pxBlock->returnValue != NULL)		/* discard XCMD's return handle */
      DisposHandle(pxBlock->returnValue);

   for (i = nargs; i--; )			/* dispose of parameter handles */
      if (pxBlock->params[i] != NULL)
         DisposHandle(pxBlock->params[i]);

   return result;
   }


/*
 * unldef - unload an external function
 */
void unldef(struct efblk *efb)
   {
   pXFNode pnode;
   void (**hcode)();

   pnode = MK_MP(efb->efcod, pXFNode);
   if (pnode == NULL)
      return;

   hcode = pnode->hcode;
   if (hcode == NULL)
      return;

   efb->efcod = 0;						/* remove pointer to XNBLK */
   
   pnode->xnnext = xnfree;				/* put on free list */
   xnfree = pnode;

   DisposHandle((Handle)hcode);			/* Discard the code. */
   }


/*
 * unloadXF - unload all external functions
 *
 * called when program returns to editor
 */
void unloadXF()
   {
   struct efblk *efb;

#ifdef WindowCallbacks
   enduserwin();				/* close all user windows */
#endif

   scanef();						/* prepare to scan for external functions */
   while (nextef((unsigned char FAR **)&efb, -1))
      unldef(efb);

   endxcmd();					/* release memory */
   }


#endif 					/* ExternalFunctions */
