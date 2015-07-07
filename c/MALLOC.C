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
/	File:  MALLOC.C        Version:  01.08
/	--------------------------------------
/
/	Contents:	Functions calloc, malloc, free, realloc
/
/	This module provides crude memory allocation support.  It is
/	only needed on systems with library routines that insist on
/	calling malloc, such as localtime() on the Sun 4.
/
/	If the system's malloc routines were used, they would call
/	sbrk *after* the initial heap was allocated, thus interfering
/	with any further heap expansion.
/
/	On other systems, we leave the routines defined as a sanity check
/	for unexpected calls to malloc.
/
/   1.01	25-Jun-90	HIGHC version of malloc was recording total
/						block size in header word, instead of block
/						size visible to user, as realloc() expected.
/
/	1.02	01-Jan-91	HIGHC version eliminated.  malloc and realloc
/						were used by mw_init_environment and _putenv.
/						This resulted in a variable number of allocations
/						(dependent upon the number of environment variables)
/						which in turn shifted the basemem address for the
/						SPITBOL heap.  This made save files not always
/						reloadable if the runtime saw a different environment
/						size than the system that created it.
/
/	1.03	08-May-91	Reinstalled for HIGHC version 2.3.  Code has been
/						added to the Save file reload logic to adjust the
/						size of the stack to force the heap to start at
/						the same address, regardless of how many mallocs
/						were done first.
/
/	1.04	15-Jul-91	Enlarged ALLOCSIZE from 840 to 2048 to fix
/						SPARC problem found by Rolf Stachowitz.
/
/   1.05    26-Sep-91   Went to a managed free list for OS/2.  Switched
/						SUN4 version to use this allocator as well.
/
/   1.06    15-Oct-91	Kludge for spawn() and system() bug in Intel
/					    library.  During either of these functions, the
/						library makes one paired malloc/free call that MUST
/						allocate from extended memory.  Use mallocSys flag
/						set my code in dosys and doexec to tell malloc/free
/						to use sbrk/brk instead of regular free list.
/
/	1.07	11-Oct-92	Add malloc_init function for SUN4 to allocate
/						a large region below the Minimal static/dynamic
/						areas.
/
/	1.08	14-May-95	AIX uses sparse memory allocation techniques, where
/						we can malloc 64 MB without committing any memory
/						pages or disk swapping space until the memory is
/						actually touched by the program.  Therefore, malloc
/						can continue to be used by other system activities,
/						such as the load function.  Omit this module from
/						SPITBOL build under AIX.
/
/	1.09	20-Oct-95	Dynamic build under SPARC calls malloc before malloc_init
/						has been called.  Change morecore to accommodate.95
*/


#include "port.h"

#undef DEBUG
#define DEBUG 0

#if HIGHC
#include <stdlib.h>
#include <string.h>
#endif					/* HIGHC */

#if INTEL
#undef sbrk
#include <stdlib.h>
extern void *dosAlloc(unsigned nbytes);
extern void dosFree(void *block);
extern void *dosRealloc(void *block, unsigned nbytes);
#define ALLOCSIZE 4096			/* size of request to system for more memory */
#endif					/* INTEL */

#if BCC16 | BCC32 | VCC | SUN4 | LINUX
typedef int size_t;
#endif					/* BCC16 | SUN4 | LINUX */

#if SUN4 | LINUX
#define ALLOCSIZE 4096				/* grab amount for morecore()		*/
typedef double Align;				/* for alignment of allocated space */
#define AlignSize sizeof(Align)
#endif					/* SUN4 | LINUX */

#if HIGHC23 | INTEL | OS2 | WINNT
typedef long Align;			/* for alignment of allocated space */
#define AlignSize sizeof(Align)
#endif					/* HIGHC23 | INTEL | OS2 */

#if OS2
#define ALLOCSIZE 4096			/* size of request to system for more memory */
#include <stddef.h>
extern void *dosAlloc(unsigned nbytes);
#pragma linkage(calloc,optlink)
#pragma linkage(free,optlink)
#pragma linkage(malloc,optlink)
#pragma linkage(realloc,optlink)
#endif					/* OS2 */

#if INTEL | OS2 | SUN4 | LINUX | WINNT
union header {					/* free block header */
	struct {
		union header *ptr;		/* next free block */
		unsigned size;			/* size of this free block in header units */
		} s;
	Align x;					/* force alignment of blocks */
};
#ifndef NULL
#define NULL  (void *)0
#endif

typedef union header HEADER;
static HEADER base;				/* empty list to get started */
static HEADER *allocp = (HEADER *)NULL;	/* last allocated block */
#endif					/* INTEL | OS2 | SUN4 | LINUX */

#if SUN4 | LINUX
static HEADER *malloclow = (HEADER *)NULL; 	/* start of region, 0 = non-initialized */
static HEADER *mallochigh;			/* end of region */
static HEADER *alloc_bp;				/* next free position */
#endif					/* SUN4 | LINUX */



#if SUN4 | LINUX
void malloc_empty()
{
	alloc_bp = malloclow;
	allocp = (HEADER *)NULL;	/* nothing is allocated */
}

/* Allocate a large block of memory to be used by the memory allocation
 * routines in this file.  The memory begins at the end of C's data region,
 * and extends up to but not including the address provided in endadr.
 */
int malloc_init(endadr)
word endadr;
{
	malloclow = (HEADER *)(((long)sbrk(0) + AlignSize - 1) & ~(AlignSize - 1));
	mallochigh = (HEADER *)endadr;
	malloc_empty();

	return brk((char *)endadr);
}
#endif					/* SUN4 | LINUX */

#if DEBUG
typedef struct memchain {
	size_t bytes;
	struct memchain *next;
	struct memchain *prev;
	unsigned int sentinal;
	} MEMCHAIN, *PMEMCHAIN;

#define SENTINAL_INT 0xEEEEEEEEL
PMEMCHAIN memchainhead = 0;
void d_add_to_ll(PMEMCHAIN mptr, size_t bytes);
void *d_malloc(size_t size);
void d_free(void *ptr);

void *malloc(size_t size)
{
	PMEMCHAIN mptr;

	if (size < 1) {
		strout("Called malloc() with < 1 bytes: ");
		numout(size);
		crlf();
		}
	mptr = d_malloc(size + sizeof(MEMCHAIN) + sizeof(int));
	if (!mptr) {
		strout("malloc() failed: ");
		numout(size);
		crlf();
		}
	d_add_to_ll(mptr, size);
	return mptr+1;
}

void free(void *fptr)
{
	PMEMCHAIN mptr;

	if (!fptr) {
		strout("freeing null pointer");
		crlf();
		}

	for (mptr = memchainhead; mptr; mptr = mptr->next)
		if (mptr + 1 == (PMEMCHAIN)fptr)
			break;

	if (!mptr) {
		strout("freeing unallocated memory at: ");
		numout((long)fptr);
		crlf();
		}

	if (mptr->sentinal != SENTINAL_INT) {
		strout("free(): Beginning sentinal damaged--contains: ");
	 	numout(mptr->sentinal);
		crlf();
		}

	if (*(int *)((char *)fptr + mptr->bytes) != SENTINAL_INT) {
		strout("free(): Ending sentinal damaged--contains: ");
	 	numout(*(int *)((char *)fptr + mptr->bytes));
		crlf();
		}

	if (mptr->prev)
		mptr->prev->next = mptr->next;
	else
		memchainhead = mptr->next;

	if (mptr->next)
		mptr->next->prev = mptr->prev;

	d_free(mptr);
}

void d_add_to_ll(PMEMCHAIN mptr, size_t bytes)
{
	void *vptr;

	vptr = mptr + 1;
	mptr->bytes = bytes;
	mptr->next = memchainhead;
	if (memchainhead)
			memchainhead->prev = mptr;
	mptr->prev = 0;
	memchainhead = mptr;
	*(int *)((char *)vptr + bytes) = SENTINAL_INT;
	mptr->sentinal = SENTINAL_INT;
}

#define free(x) d_free(x)
#define malloc(x) d_malloc(x)
#endif					/* DEBUG */


void *malloc(size)
size_t size;
{
#if HIGHC23
	register void *xbase;
    size = (size + sizeof(size_t) + AlignSize - 1) & ~(AlignSize - 1);	/* round up */
	if ((xbase = sbrk((uword)size)) != (void *)-1) {
		*((size_t *)xbase) = size - sizeof(size_t);	/* record allocation size */
		return (void *)((unsigned)xbase + sizeof(size_t));
		}
	else
#endif					/* HIGHC23 */
#if INTEL | OS2 | SUN4 | LINUX | WINNT
	/* Because OS/2's storage allocator allocates in blocks with a minimum
	 * size of 4K bytes, we cannot use it for each of the small allocations
	 * usually made of malloc.  Instead, we will use the storage allocator
	 * from Kernighan & Ritchie, "The C Programming Language", 1st ed.
	 */
	register HEADER *p, *q;
	HEADER *morecore Params((unsigned nunits));
	unsigned nunits;

#if INTEL
	void *result;

	if (mallocSys) {						/* see dosys.c for explanation */
		result = sbrkx((uword)size);
		if (result == (void *)-1) {
			result = (void *)0;				/* failure */
			mallocSys = 0;
			}
		return result;
		}
#endif					/* INTEL */

	nunits = 1+(size+sizeof(HEADER)-1)/sizeof(HEADER);
	if ((q = allocp) == (HEADER *)NULL) {	/* no free list yet */
		base.s.ptr = allocp = q = &base;
		base.s.size = 0;
		}
	for (p=q->s.ptr; ; q=p, p=p->s.ptr) {
		if (p->s.size >= nunits) {			/* big enough */
			if (p->s.size == nunits)		/* exactly */
				q->s.ptr = p->s.ptr;
			else {							/* allocate tail end */
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
				}
			allocp = q;
			return ((void *)(p+1));
			}
		if (p == allocp)					/* wrapped around free list */
			if ((p = morecore(nunits)) == (HEADER *)NULL)
				break;
		}
#endif					/* INTEL | OS2 | SUN4 | LINUX */

		{
		wrterr( "Internal system error--MALLOC" );
		__exit(1);
		return (void *) 0;
		}
}

/* calloc().  By C specifications, elements are initialized to 0.
 */
void *calloc(nelem, elsize)
unsigned int nelem;
size_t elsize;
{
	void *result;
	size_t size = nelem * elsize;

	result = malloc(size);
	if (result)
		memset(result, 0, size);

	return result;
}


void free(ptr)
void *ptr;
{
#if HIGHC | BCC16 | BCC32 | VCC
	wrterr( "Internal system error--FREE" );
	__exit(1);
#endif					/* HIGHC | BCC16 */
#if INTEL | OS2 | SUN4 | LINUX | WINNT
	register HEADER *p, *q;

#if INTEL
	if (mallocSys) {						/* see dosys.c for explanation */
		brk(ptr);
		mallocSys = 0;						/* only do this once */
		return;
		}
#endif					/* INTEL */

	p = (HEADER *)ptr - 1;				/* point to header */
	for (q = allocp; !(p > q && p < q->s.ptr); q=q->s.ptr)
		if (q >= q->s.ptr && (p > q || p < q->s.ptr))
			break;						/* at one end or other */

	if (p+p->s.size == q->s.ptr) {		/* join to upper neighbor */
		p->s.size += q->s.ptr->s.size;
		p->s.ptr = q->s.ptr->s.ptr;
		}
	else
		p->s.ptr = q->s.ptr;
	if (q+q->s.size == p) {				/* join to lower neighbor */
		q->s.size += p->s.size;
		q->s.ptr = p->s.ptr;
		}
	else
		q->s.ptr = p;
	allocp = q;
#endif					/* INTEL | OS2 | SUN4 | LINUX */
}


void *realloc(ptr, size)
void *ptr;
size_t size;
{
#if HIGHC23 | INTEL | OS2 | SUN4 | LINUX | WINNT
	size_t csiz;
	void *newmem;

	/* Some libraries allow realloc to be called with a null ptr
	 *  argument, in which case it is treated as a simple malloc.
	 */
	if (ptr == (void *)NULL)
		return malloc(size);

#if HIGHC23
	newmem = (void *)((unsigned)ptr - sizeof(size_t));
	csiz = *((size_t *)newmem);
	if (size <= csiz)
		return ptr;
	else {				/* more space needed */
		if ((newmem = malloc(size)) != NULL) {
			memcpy(newmem,ptr,(int)csiz);
#endif					/* HIGHC23 */


#if INTEL | OS2 | SUN4 | LINUX | WINNT
	/* Size of block availble to user */
	csiz = sizeof(HEADER) * (((HEADER *)ptr-1)->s.size - 1);
	if (size <= csiz)
		return ptr;
	else {				/* more space needed */
		if ((newmem = malloc(size)) != NULL) {
			memcpy(newmem,ptr,(int)csiz);
			free(ptr);
#endif					/* INTEL | OS2 | SUN4 | LINUX */
            return newmem;
            }
		}
#endif					/* HIGHC23 | INTEL | OS2 | SUN4 | LINUX */

	wrterr( "Internal system error--REALLOC" );
	__exit(1);
	return (void *) 0;
}



#if INTEL | OS2 | SUN4 | LINUX | WINNT
#define NALLOC (ALLOCSIZE/sizeof(HEADER))

HEADER *morecore(nu)				/* ask system for memory */
unsigned nu;
{
	unsigned rnu;
	void *cp;
	register HEADER *up;

	rnu = NALLOC * ((nu+NALLOC-1) / NALLOC);


#if INTEL | OS2 | WINNT
	cp = dosAlloc(rnu * sizeof(HEADER));
#endif					/* INTEL | OS2 */

#if SUN4 | LINUX
	if (!malloclow)
	{
		/* malloc not initialized yet (called by startup code prior to main() */
		cp = (void *)(((long)sbrk(0) + AlignSize - 1) & ~(AlignSize - 1));
		brk((char *)cp + rnu*sizeof(HEADER));
	}
	else
	{
		if (alloc_bp + rnu > mallochigh)
			cp = (void *)NULL;
		else
		{
			cp = alloc_bp;
			alloc_bp += rnu;
		}
	}
#endif					/* SUN4 | LINUX */

	if (cp == NULL)						/* no space at all */
		return (HEADER *)NULL;
	up = (HEADER *)cp;
	up->s.size = rnu;
	free((void *)(up+1));
	return allocp;
}
#endif					/* INTEL | OS2 | SUN4 | LINUX */

