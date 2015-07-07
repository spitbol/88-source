
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
/	File:  GETHOST.C	Version:  01.05
/	---------------------------------------
/
/	Contents:	Function gethost
*/

/*
/   gethost( scptr, maxlen )
/
/   gethost() reads the first line from the host file into the passed SCBLK.
/
/   Parameters:
/	scptr	pointer to SCBLK to receive host string
/	maxlen	max length of string area in SCBLK
/   Returns:
/	Nothing.
/   Side Effects:
/	Modifies contents of passed SCBLK (scptr).
*/

#include "port.h"
#if MSDOS && (BCC16 | BCC32)
#include <dos.h>
#endif

#if APOLLO
char htype[] = "Apollo";
char osver[] = ":Berkeley Unix";
#endif
#if APPLE
#include "pedit.h"
#include <gestaltequ.h>
extern void GetSerialStr(StringPtr sbuf);
char htype[] = "Apple Macintosh ";
char osver[] = ":System ";
#endif
#if ATT73
char htype[] = "Unix PC";
char osver[] = ":System V Unix";
#endif
#if BCC16
#if IA32
char htype[] = "8088/32";
#else
char htype[] = "8088/16";
#endif
char osver[] = ":MS-DOS ";
#endif
#if BCC32
char htype[] = "80386";
char osverNT[] = ":WinNT";
char osver95[] = ":Win95";
char osver98[] = ":Win98";
char osverDOS[] = ":MS-DOS(Borland Extended)";
char *osver = osverNT;
#endif                  /* BCC32 */
#if VCC
char htype[] = "80386";
char osverNT[] = ":WinNT";
char osver95[] = ":Win95";
char osver98[] = ":Win98";
char osverDOS[] = ":Windows";
char *osver = osverNT;
#endif                  /* VCC */
#if DSI
char htype[] = "Definicon";
char osver[] = ":MS-DOS ";
#endif
#if HPBSD
char htype[] = "Hewlett-Packard";
char osver[] = ":Berkeley Unix";
#endif
#if HPUX
char htype[] = "Hewlett-Packard";
char osver[] = ":System V Unix";
#endif
#if HIGHC
char htype[] = "80386";
char osver[] = ":MS-DOS(PharLap Extended) ";
#endif
#if INTEL
char htype[] = "80386";
char osver[] = ":MS-DOS(Intel Extended) ";
#endif
#if LINUX
char htype[] = "80386";
char osver[] = ":Linux ";
#endif
#if MIPSSGI
char htype[] = "MIPS";
char osver[] = ":IRIX";
#endif
#if NCR
char htype[] = "NCR";
char osver[] = ":System V Unix";
#endif
#if OS2
char htype[] = "80386";
char osver[] = ":OS/2 ";
#endif
#if RS6
char htype[] = "RS/6000";
#if AIX3
char osver[] = ":AIX V3";
#endif
#if AIX4
char osver[] = ":AIX V4";
#endif
#endif
#if SUN2
char htype[] = "Sun 2";
char osver[] = ":Sun OS";
#endif
#if SUN3
char htype[] = "Sun 3";
char osver[] = ":Sun OS";
#endif
#if SUN4
char htype[] = "SPARC";
#if SOLARIS
char osver[] = ":Solaris";
#else
char osver[] = ":Sun OS";
#endif
#endif
#if UNISYS
char htype[] = "UniSys";
char osver[] = ":System V Unix";
#endif
#if VAX
char htype[] = "VAX";
char osver[] = ":System V Unix";
#endif
#if WICAT
char htype[] = "WICAT";
char osver[] = ":MCS O/S";
#endif


#if BSD42 | SUNOS
#include <sys/file.h>
#endif					/* BSD42 */

#if SYSTEM5 | MSDOS | OS2 | IRIX | AIX | SOLARIS | LINUX
#include <fcntl.h>
#endif					/* SYSTEM5 | MSDOS | OS2 | IRIX */

#if APPLE
/* No longer need to update machines[] array.  For operating systems >= 6.0.4,
 * we get the machine name from the System file's STR# resource via Gestalt.
 */
char *machines[] = {"", "", "", "512KE", "Plus", "SE", "II", "IIx", "IIcx", "SE/30",
					"Portable", "IIci"};
#endif					/* APPLE */

void gethost( scptr, maxlen )
struct	scblk	*scptr;
word	maxlen;

{
	struct scblk *pHEADV = GET_DATA_OFFSET(HEADV,struct scblk *);
#if APPLE
	register char *cp, *scp;
	int vminor;
	char serStr[10];

	gettype( scptr, maxlen );
	scp = scptr->str;
	strcat(scp,":");
	strcat(scp,htype);
	cp = scp + strlen(scp);
	cp += stci_d(cp, systemVersion >> 8, 4);
	*cp++ = '.';
	*cp++ = ((systemVersion >> 4) & 0xF) + '0';
	if (vminor = (systemVersion & 0xF))
	{
		*cp++ = '.';
		*cp++ = vminor + '0';
	}
	*cp++ = '\0';
	strcat(scp,":Macro SPITBOL ");
	strncat(scp, pHEADV->str, pHEADV->len );
	strncat(scp, pID1->str, (int)pID1->len);
	strcat(scp," #");
	GetSerialStr((unsigned char *)serStr);
	PtoCstr(serStr);
	strcat(scp, serStr);
	scptr->len = strlen(scp);
#else					/* APPLE */
	int	cnt = 0;
	word fd;

	if ( (fd = spit_open( HOST_FILE, O_RDONLY, IO_PRIVATE | IO_DENY_WRITE,
	 IO_OPEN_IF_EXISTS )) >= 0 )
	{
		cnt	= read( fd, scptr->str, maxlen );
		close( fd );
	}

#if EOL2
        if ( cnt > 0  &&  scptr->str[cnt-2] == EOL1 )
	{
		cnt--;
		scptr->str[--cnt] = 0;
	}
#else					/* EOL2 */
        if ( cnt > 0  &&  scptr->str[cnt-1] == EOL1 )
	{
		scptr->str[--cnt] = 0;
	}
#endif					/* EOL2 */

	if ( cnt == 0 )
	{
		/* Could not read spithost file.  Construct string instead */
		register char *scp;

		gettype( scptr, maxlen );
		scp = scptr->str + scptr->len;
		scp = mystrcpy(scp,osver);

#if MSDOS | DSI | OS2
		{
			register char *cp;
			scp += stcu_d(scp, dosvers() & 0xFF, 4);
			cp = scp;
			scp += stcu_d(scp, 100+((dosvers() >> 8) & 0x7F), 5);
			*cp = '.';
		}
#endif					/* HIGHC | INTEL | BCC16 | DSI | OS2 */

		scp = mystrcpy(scp,":Macro SPITBOL ");
		scp += mystrncpy(scp, pHEADV->str, pHEADV->len );
		scp += mystrncpy(scp, pID1->str, (int)pID1->len);
		*scp++ = ' ';
		*scp++ = '#';
		scp = mystrcpy(scp, SERIAL);
		cnt = scp - scptr->str;
	}

	scptr->len = cnt;
#endif					/* APPLE */
}



/*
 * Get type of host computer
 */
void gettype( scptr, maxlen )

struct	scblk	*scptr;
word	maxlen;
{
	cpys2sc( htype, scptr, maxlen );	/* Computer type */
#if APPLE
	{
		Str255 nameBuf;
		GetIndString(nameBuf, kMachineNameStrID, machineType);
		if (nameBuf[0]) {
			strncpy(&scptr->str[6], (char *)&nameBuf[1], nameBuf[0]);
			scptr->len = nameBuf[0] + 6;
			}
		else if (machineType < (sizeof(machines)/sizeof(char *))) {
			strcat(scptr->str, machines[machineType]);
			scptr->len = strlen(scptr->str);
			}
	}
#endif					/* APPLE */
}
