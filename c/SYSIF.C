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
/  File:  SYSIF.C    Version:  01.03
/	-----------------------------------
/
/	Contents:	Function zysif
/
/	V1.01	01-25-90
/			If include file cannot be opened, restore fd 0.
/
/	V1.02	02-16-91
/			Call clrbuf() after closing the existing file.
/
/  V1.03 04-27-97
/        Look for include file in the directory where SPITBOL resides, and
/        in the directory of the first source file.
*/

/*
/	zysif - start/stop using include file
/
/	zysif stacks the current input stream and opens a new include file.
/		It is also called when an EOF is read to restore the stacked file.
/
/	Parameters:
/	    XL	 pointer to SCBLK with name of file.
/	 		 0 to end use of file.
/	    XR - pointer to vacant SCBLK that will receive the name of the
/			 file finally opened, after looking in other directories.
/	Returns:
/	    XR - scblk filled in with full path name and length.
/	Exits:
/	     1 - could not find file
/
*/

#include "port.h"

#if APPLE
#include "PEdit.h"
#endif					/* APPLE */


#if BSD42 | SUNOS
#include <sys/file.h>
#endif

#if MSDOS | OS2 | SYSTEM5 | IRIX | AIX | APPLE | SOLARIS | LINUX
#include <fcntl.h>
#endif

static	void	openprev Params((void));

/*
/  Helper function to back up one file in the include nesting.
*/

static void openprev()
{
	fd = inc_fd[--nesting];		/* Unstack one level */
#if USEFD0FD1
	dup(fd);					/* Create fd 0 for previous file */
	close(fd);					/* Release dup'ed fd of old file */
	fd = 0;
#else					/* USEFD0FD1 */
	setrdfd( fd );	  			/* Make it the current input stream */
#endif					/* USEFD0FD1 */
	clrbuf();

	doset( getrdiob(),inc_pos[nesting],0 );	/* Position file where left off */
}

zysif()
{
	register struct scblk *fnscb = XL (struct scblk *);
	register struct scblk *pnscb = XR (struct scblk *);
	register char *savecp;
	char savechar, filebuf[256];
	char *file;

	if (fnscb) {
		/* Here to nest another include file */
		if (nesting == INCLUDE_DEPTH)			/* Is there room in array? */
			return EXIT_1;

		inc_pos[nesting] = doset(getrdiob(),0L,1);	/* Record current position */
#if USEFD0FD1
		inc_fd[nesting++] = dup(0);			/* Save current input file */
		close(0);							/* Make fd 0 available */
#else					/* USEFD0FD1 */
		inc_fd[nesting++] = getrdfd();		/* Record current input stream */
#endif					/* USEFD0FD1 */
		clrbuf();
		savecp = fnscb->str + fnscb->len;	/* Make it a C string for now. */
		savechar = *savecp;
		*savecp = '\0';
		file = fnscb->str;
		fd = spit_open( file, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE,
		IO_OPEN_IF_EXISTS );	/* Open file */
      if (fd < 0)
      {
         /* If couldn't open, try alternate paths via SNOLIB */
         initpath(SPITFILEPATH);
			file = filebuf;
         while (trypath(fnscb->str,file))
         {
				fd = spit_open(file, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE, IO_OPEN_IF_EXISTS);
				if (fd >= 0)
					break;
         }
      }
      if (fd < 0)
      {
         /* If still not open, look in directory where SPITBOL resides. */
         int i = pathlast(gblargv[0]) - gblargv[0];
         if (i)
         {
            mystrncpy(filebuf, gblargv[0], i);
            mystrcpy(&filebuf[i], fnscb->str);
            fd = spit_open(filebuf, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE, IO_OPEN_IF_EXISTS);
         }
      }
      if (fd < 0 && sfn && sfn[0])
      {
         /* If still not open, look in directory where first source file resides. */
         int i = pathlast(sfn) - sfn;
         if (i)
         {
            mystrncpy(filebuf, sfn, i);
            mystrcpy(&filebuf[i], fnscb->str);
            fd = spit_open(filebuf, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE, IO_OPEN_IF_EXISTS);
         }
      }
		if ( fd >= 0 ) {  				/* If file opened OK */
			cpys2sc(file,pnscb,pnscb->len);
#if !USEFD0FD1
#if APPLE
			openres(file,0);			/* make include file's resource fork available */
#endif					/* APPLE */
			setrdfd( fd );				/* Make it the current input stream */
#endif					/* !USEFD0FD1 */
			*savecp = savechar;			/* Restore saved char */
			}
		else {  						/* Couldn't open file */
			*savecp = savechar;			/* Restore saved char */
			openprev();					/* Restore input file we just closed */
			return EXIT_1;				/* Fail */
			}
		}
/*
/  EOF read.  Pop back one include file.
*/
	else {
		if (nesting > 0) {				/* Make sure don't go too far	*/
			close(fd);					/* Close last include file		*/
			openprev();					/* Reopen previous include file	*/
			}
		}

#if MSDOS | OS2 | WINNT
#if USEFD0FD1
	if ( cindev( 0 ) == 0 )		/* Test for character input */
#else
	if ( cindev(getrdfd()) == 0 )		/* Test for character input */
#endif
	    getrdiob()->flg1 |= IO_CIN;
	else
		getrdiob()->flg1 &= ~IO_CIN;
#endif               /* MSDOS | OS2 | WINNT */

	return NORMAL_RETURN;
}
