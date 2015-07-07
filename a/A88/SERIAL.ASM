; Copyright (C)  1988-1998  Mark Emmer

; SPITBOL is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.


; First segment in program.  Contains serial number string.
; If external functions are included, a call to the external
; function will appear in this segment as well, placed here
; by the code in load.asm.
;
; Because the call occurs at a fixed address in the code,
; it is possible to place a breakpoint at location CS:0008 to
; intercept function calls.
;
	page		80,132
	name		SERIAL
	include		systype.ah
	include		rules.asi

	Header@

	Code_Seg@
	public		_serial
_serial	db		"50001",0
	Code_EndS@

	Data_Seg@
	public		_hasfpu
	public		_cprtmsg
	even
_hasfpu	dw		?		;-1 if 80x87 present, else 0
_cprtmsg db             "(c) Copyright 1987-2000 Robert B. K. Dewar and Catspaw, Inc.",0
	Data_EndS@
	end
