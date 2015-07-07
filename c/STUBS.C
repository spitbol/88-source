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
/	File:  STUBS.C		Version:  01.02
/	---------------------------------------
/
/	Contents:	Function zysdc
/			Function zysdm
/			Function zystt
*/

/*
/	All functions are "dummy" functions not supported by this
/	implementation.
*/

#include "port.h"

#if VAX
zysdc()
{
	return NORMAL_RETURN;
}
#endif

zysdm()
{
	return NORMAL_RETURN;
}


zystt()
{
	return NORMAL_RETURN;
}

