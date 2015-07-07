
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
/	File:  DOSYS.C		Version:  01.04
/	---------------------------------------
/
/	Contents:	Function dosys
/
/	1.04	15-Oct-91	Intel bug in system() command.  Malloc must
/						allocate out of high memory to allow spawn to
/						work properly.  We set a global switch that
/						tells malloc to use sbrk to satisfy memory
/						request made by spawn() and system().
/
/   1.03	08-May-91	Revised for HighC version 2.3, which cannot use
/						the system function because it calls spawnle, which
/						in turn uses the _ENVIRON pointer to a local copy
/						of the environment block.  Because our custom
/						INIT function does not copy the environment blcok
/						to local storage, _ENVIRON cannot be used.
/
/	1.02	23-Jun-90	Add second argument for optional path specification.
/				Change first argument to C-string, not SCBLK.
/
/	1.01	04-Mar-88	Changes for Definicon
/
*/

/*
/   dosys( cmd, path )
/
/   dosys() does a "system" function call with the string contained in cmd.
/
/   Parameters:
/	cmd		C-string of command to execute
/	path	C-string of optional pathspec of program to execute.
/			May be null string.
/   Returns:
/	code returned by system
*/

#include "port.h"

#if OS2 | WINNT
#include <stdlib.h>
#endif					/* OS2 */

#if MSDOS && (HIGHC | INTEL | BCC16 | BCC32 | VCC)
#undef sbrk
#include <stdlib.h>
#include <errno.h>
#if HIGHC16
extern short int _mwc_exec(char *path, char *cmd);
#else					/* HIGHC16 */
#include <process.h>
#endif					/* HIGHC16 */
#endif					/* HIGHC | INTEL | BCC16 */

#if DSI
char	*getshell();
#endif					/* DSI */

int dosys( cmd, path )
char	*cmd;
char	*path;
{
#if DSI
/*
/	Append " /c " and command line to command processor path.
*/
	char	*shell_path, *limit, *p;

	p = shell_path = getshell();	/* get path to command.com	*/
	limit = shell_path + TSCBLK_LENGTH - 3;

	while ( *++p )			/* advance to null after path	*/
		;

	*p++ = ' ';			/* append " /c "		*/
	*p++ = '/';
	*p++ = 'c';
	*p++ = ' ';

	while( *cmd && p < limit)	/* append command line		*/
		*p++ = *cmd++;

	*p++ = '\r';			/* append return and null	*/
	*p++ = 0;
	cmd = shell_path;
#endif					/* DSI */

#if MSDOS && (HIGHC | INTEL | BCC16 | BCC32 | VCC)
	char *p;

	errno = 0;
	p = pTSCBLK->str;
	*p++ = 0;				/* character count */
	if (!*path) {
		if (*cmd) {
			*p++ = ' ';
			*p++ = '/';
			*p++ = 'c';
			*p++ = ' ';
			pTSCBLK->str[0] += 4;
			}
		path = getshell();
		}
	while ( *cmd && pTSCBLK->str[0] < 126 )	{	/* DOS command line length limit */
		*p++ = *cmd++;
		pTSCBLK->str[0]++;
		}
	*p++ = '\r';			/* append return, space and null */
	*p++ = ' ';				/* per docs in HighC's exec.pf */
	*p = 0;
    mallocSys = 1;
	return spawnl(P_WAIT, path, path, &(pTSCBLK->str[1]), NULL);
#else					/* INTEL | HIGHC | BCC16 */
	return system( cmd );
#endif					/* INTEL | HIGHC | BCC16 */
}

#if HIGHC16
int spawnl(int mode, char *cmdname, char *arg0, char *arg1, int marker)
{
	return _mwc_exec( cmdname, arg1 );
}
#endif					/* HIGHC16 */

#if HIGHC23
int system(const char *__string)
{
	char *path = getshell();

	return spawnl(P_WAIT, path, path, "/c", __string, NULL);
}
#endif					/* HIGHC23 */

#if 0
int system(const char *__string)
{
	extern char **environ;
	char *path, **p, **q, **envp;
	int result;

	path = getshell();
	if (!path)
		return -1;

	for(p = environ; *p++;)				/* get size of environ array */
		;
	q = envp = (char **)sbrkx((uword)((unsigned)p - (unsigned)environ));
	if (q == (char **)-1)
		return -1;

	for (p = environ; *p; ) {			/* allocate and copy strings */
		*q = sbrkx((uword)length(*p) + 1);
		if (*q == (char *)-1) {
			brk((void *)envp);			/* discard any copied stuff */
			return -1;
			}
		mystrcpy(*q++, *p++);
		}
	*q = (char *)0;

	result = spawnle(P_WAIT, path, path, "/c", __string, NULL, envp);

	brk(envp);	  						/* discard all the copied stuff */
	return result;
}
#endif					/* INTEL */

