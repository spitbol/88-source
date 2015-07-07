/*
 Copyright (c)  1991 Robert Goldberg and Catspaw, Inc.

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
/	File:  WRTAOUT.C	Version:  01.02
/	---------------------------------------
/
/	Contents:	Function openaout
/			Function wrtaout
/			Function seekaout
/			Function closeaout
/
/   These functions are used to write an executable "a.out" file containing
/   the currently executing spitbol program.
*/

#include "port.h"

#if APPLE
#include "PEdit.h"
#include "iproto.h"
extern int srcRefNum, spitRefNum, saveRefNum;
extern void closeSave(void);
#define clock() TickCount()
#else					/* APPLE */
#if DSI | HIGHC | INTEL | BCC16 | BCC32 | VCC | OS2
#include <time.h>
#else					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
#include <sys/types.h>
#include <sys/times.h>
#endif					/* DSI | HIGHC | INTEL | BCC16 | OS2 */
#endif					/* APPLE */

#if BSD42 | SUNOS
#include <sys/file.h>
#endif

#if MSDOS | OS2 | SYSTEM5 | APPLE | IRIX | AIX | SOLARIS  | LINUX
#include <fcntl.h>
#endif

#if SAVEFILE | EXECFILE

/*  openaout(file, tmpfnbuf, exe)
/
/   Parameters:
/	file = file name
/	tmpfnbuf = buffer where we can build temp file name
/	exe = IO_EXECUTABLE to mark file as executable, else 0
/   Returns:
/	0	successful. Variable aoutfd set to file descriptor.
/	-1	create error for "a.out"
*/
int openaout(fn, tmpfnbuf, exe)
char *fn;
char *tmpfnbuf;
int	exe;
{
	char			*p;
	unsigned int	m,n;

#if 0
	OSType saveFileType = RunFileType;
	OSType creatorType = CreatorType;
	long seconds;

	/* Try to delete existing "a.out" file. This will fail if we are running on
	 * the resource fork of this save file. This could happen with two successive
	 * EXIT(-4)'s in a program.  In this case, open a temp file instead.
	 */
	if ((usingTempFN = unlink(fn)) != 0) {
		GetDateTime(&seconds);
		NumToString(seconds, tempFN);		/* Create temp file name */
		InsStr((unsigned char *)"\ptemp",(unsigned char *)tempFN);
		PtoCstr(tempFN);
		fn = tempFN;
		}
#endif					/* APPLE */

	mystrcpy(tmpfnbuf, fn);
	n = (unsigned int)clock();
	m = n = n - ((n / 10000) * 10000);		/* put in range 0 - 9999 */
	for (;;) {
		p = pathlast(tmpfnbuf);				/* p = address we can append to */
		p = mystrcpy(p, "temp");
		p += stcu_d(p, n, 4);
		mystrcpy(p, ".tmp");
		if (access(tmpfnbuf, 0) != 0)
			break;
		n++;
		n = n - ((n / 10000) * 10000);		/* put in range 0 - 9999 */
		if (m == n)
			return -1;
	}

	if ( (aoutfd = spit_open( tmpfnbuf, O_WRONLY|O_TRUNC|O_CREAT,
	 IO_PRIVATE | IO_DENY_READWRITE | exe /* ? 0777 : 0666 */,
	 IO_REPLACE_IF_EXISTS | IO_CREATE_IF_NOT_EXIST )) < 0 )
		return	-1;
#if APPLE
	{
		OSType saveFileType = RunFileType;
		OSType creatorType = CreatorType;
		SetFileType(tmpfnbuf, (char*)&saveFileType, (char*)&creatorType);
	}
#endif					/* APPLE */
   fp = (FILEPOS)0;           /*   file position   */
	return 0;
}

/*
/   wrtaout( startadr, size )
/
/   Parameters:
/	startadr	FAR char pointer to first address to write
/	size		number of bytes to write
/   Returns:
/	0	successful
/	-2	error writing memory to a.out
/
/   Write data to a.out file.
*/
int wrtaout( startadr, size )
unsigned char FAR *startadr;
uword size;
{
	if ( (uword)writefar( aoutfd, startadr, size ) != size )
		return	-2;

	fp += size;			/*   advance file position	*/
	return 0;
}

#if EXECFILE
/*
/   seekaout( pagesize )
/
/   Parameters:
/	pagesize	power of two (e.g. 1024)
/   Returns:
/	0	successful
/  -3 LSEEK to pagesize-1 file position failed
/	-4	forced write to pagesize boundary failed
/
/   Seek and extend file to power of two boundary.
*/

int seekaout( pagesize )
long pagesize;
{
   register long excess;

/*
/   If fp not multiple of pagesize, force file size up to multiple.
/   Notice trick to force file size up:  seek to 1 character in front
/   of desired length, then write a single character at that position.
/   The file system will fill in seeked over characters.
*/
   if ( (excess = ((long)fp & (pagesize - 1))) != 0 )
	{
		excess	= pagesize - excess;
      if ( LSEEK( aoutfd, (FILEPOS)(excess-1), 1 ) < (FILEPOS)0 )
			return	-3;
		if ( write( aoutfd, "", 1 ) != 1 )
			return	-4;
      fp += (FILEPOS)excess;
	}

	return 0;
}
#endif					/* EXECFILE */


/*
/   closeaout(filename)
/
/   Parameters
/	filename
/   Returns:
/	none
/
/   Close "a.out" file and return.
*/

word closeaout(fn, tmpfnbuf, errflag)
char *fn;
char *tmpfnbuf;
word errflag;
{
#if APPLE
	closeSave();
#endif
	close( aoutfd );
	if (errflag == 0)
	{
		unlink(fn);							/* delete old file, if any */
		if (rename(tmpfnbuf, fn) != 0)
			errflag = -1;					/* if can't rename it */
	}
	if (errflag != 0)						/* if failing, delete temp file */
		unlink(tmpfnbuf);
#if APPLE
	srcRefNum = saveRefNum = openres(fn,0);	/* leave open for execution */
#endif
	return errflag;
}


#if SAVEFILE
/*
/   rdaout( fd, startadr, size ) - read in section of file created by wrtaout()
/
/   Parameters:
/	fd		file descriptor
/	startadr	char pointer to first address to read
/	size		number of bytes to read
/   Returns:
/	0	successful
/	-2	error reading from a.out
/
/   Read data from .spx file.
*/
int rdaout( fd, startadr, size )
int	fd;
unsigned char FAR *startadr;
uword size;
{
	if ( (uword)readfar( fd, startadr, size ) != size )
		return	-2;

	fp += size;			/*   advance file position	*/
	return 0;
}
#endif					/* SAVEFILE */

#if APPLE & SAVEFILE
/*	closeSave() - close resource fork of Save file if it is open */
void closeSave()
{
	if (saveRefNum != -1) {
		CloseResFile(saveRefNum);
		saveRefNum = -1;
		}
}



/*
/	resaout() - copy all open resources to file created by openaout().
/
/	When called, the resource chain looks like this:
/
/	System<--MaxSPITBOL<--Source<--Include1<--...<--IncludeN<--SaveFile
/
/  or
/
/	System<--MaxSPITBOL<--SaveFile1<--SaveFile2
/
/	We close all resource files as they are copied.  The program can still
/	run because the resources are now available from the save file.
/
/   A problem occurs when SaveFile1 == SaveFile2.  This can happen in a program
/	that contains two EXIT(-4) statements.  The first EXIT leaves us running
/	on the save file's resource fork.
/
/	We can reach back to spitRefNum if the file being compiled is an Untitled window
/	that hasn't been written to disk yet.  If it doesn't include any other files,
/	there won't be any resources to copy.
/
*/
int resaout(fn)
char *fn;
{
	int refNum, result;

//	if (usingTempFN)
//		fn = tempFileName;
//	else
//		closeSave();

	saveRefNum = openres(fn,1);
	if (saveRefNum == -1)
		return -1;


	do {
		CloseResFile(saveRefNum);
		refNum = CurResFile();				/* penultimate resource file */
		saveRefNum = openres(fn,2);
		if (refNum == spitRefNum)			/* in case no source reference file */
			break;
		result = copyresx(refNum, saveRefNum, refNum == srcRefNum ? 0 : 1);
		CloseResFile(refNum);
		} while (refNum != srcRefNum && !result);

	CloseResFile(saveRefNum);
	srcRefNum = saveRefNum = openres(fn,0);	/* leave open for execution */
	return result;
}
#endif					/* APPLE & SAVEFILE */
#endif					/* SAVEFILE | EXECFILE */
