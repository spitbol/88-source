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


Code	segment	dword public 'CODE'
	assume	cs:Code, ds:nothing
	public	ZTimerOn, ZTimerOff, ZTimerReport

BASE_8253 	equ	40h
TIMER_0_8253	equ	BASE_8253 + 0
MODE_8253	equ	BASE_8253 + 3
OCW3		equ	20h
IRR		equ	20h

MPOPF	macro
	  local	p1, p2
	  jmp	short p2
p1:	  iret
p2:	  push	cs
	  call	p1
	endm

DELAY	macro
	  local	l1, l2, l3
	  jmp	l1
l1:	  jmp	l2
l2:	  jmp	l3
l3:
	endm

OriginalFlags	db	?
TimedCount	dw	?
ReferenceCount	dw	?
OverflowFlag	db	?

OutputStr	label	byte
		db	0dh, 0ah, 'Timed count: ', 5 dup(?)
ASCIICountEnd	label	byte
		db	' microseconds', 0dh, 0ah
		db	'$'

OverflowStr	label	byte
	db	0dh, 0ah
	db	'***************************************************'
	db	0dh, 0ah
	db	'* The timer overflowed, so the interval timed was *'
	db	0dh, 0ah
	db	'* too long for the precision timer to measure.    *'
	db	0dh, 0ah
	db	'* Please perform the timing tess again with the   *'
	db	0dh, 0ah
	db	'* long-period timer.                              *'
	db	0dh, 0ah
	db	'$'

ZTimerOn	proc	near
	push	ax
	pushf
	pop	ax
	mov	cs:[OriginalFlags],ah
	and	ah, 0fdh
	push	ax
	sti
	mov	al,00110100b
	out	MODE_8253,al
	DELAY
	sub	al,al
	out	TIMER_0_8253,al
	DELAY
	out	TIMER_0_8253,al
	DELAY
	DELAY
	DELAY
	DELAY
	cli
	mov	al,00110100b
	out	MODE_8253,al
	DELAY
	sub	al,al
	out	TIMER_0_8253,al
	DELAY
	out	TIMER_0_8253,al
	MPOPF
	pop	ax
	ret
ZTimeron	endp

ZTimerOff	proc	near
	push	ax
	push	cx
	pushf
	mov	al,00000000b
	out	MODE_8253,al
	mov	al,00001010b
	out	OCW3,al
	DELAY
	in	al,IRR
	and	al,1
	mov	cs:[OverflowFlag],al
	sti
	in	al,TIMER_0_8253
	DELAY
	mov	ah,al
	in	al,TIMER_0_8253
	xchg	ah,al
	neg	ax
	mov	cs:[TimedCount],ax
	mov	cs:[ReferenceCount],0
	mov	cx,16
	cli
RefLoop:
	call	ReferenceZTimerOn
	call	ReferenceZTimerOff
	loop	RefLoop
	sti
	add	cs:[ReferenceCount],8
	mov	cl,4
	shr	cs:[ReferenceCount],cl
	pop	ax
	mov	ch,cs:[OriginalFlags]
	and	ch,not 0fdh
	and	ah,0fdh
	or	ah,ch
	push	ax
	MPOPF
	pop	cx
	pop	ax
	ret
ZTimerOff	endp

ReferenceZTimerOn	proc	near
	push	ax
	pushf
	mov	al,00110100b
	out	MODE_8253,al
	DELAY
	sub	al,al
	out	TIMER_0_8253,al
	DELAY
	out	TIMER_0_8253,al
	MPOPF
	pop	ax
	ret
ReferenceZTimeron	endp

ReferenceZTimerOff	proc	near
	push	ax
	push	cx
	pushf
	mov	al,00000000b
	out	MODE_8253,al
	DELAY
	in	al,TIMER_0_8253
	DELAY
	mov	ah,al
	in	al,TIMER_0_8253
	xchg	ah,al
	neg	ax
	add	cs:[ReferenceCount],ax
	MPOPF
	pop	cx
	pop	ax
	ret
ReferenceZTimerOff	endp

ZTimerReport	proc	near
	pushf
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	ds
	push	cs
	pop	ds
	assume	ds:Code
	cmp	[OverflowFlag],0
	jz	PrintGoodCount
	mov	dx,offset OverflowStr
	mov	ah,9
	int	21h
	jmp	short EndZTimerReport
PrintGoodCount:
	mov	ax,[TimedCount]
	sub	ax,[ReferenceCount]
	mov	si,offset ASCIICountEnd-1
	mov	dx,8381
	mul	dx
	mov	bx,10000
	div	bx
	mov	bx,10
	mov	cx,5
CTSLoop:
	sub	dx,dx
	div	bx
	add	dl,'0'
	mov	[si],dl
	dec	si
	loop	CTSLoop
	mov	ah,9
	mov	dx,offset OutputStr
	int	21h
EndZTimerReport:
	pop	ds
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	MPOPF
	ret
ZTimerReport	endp
Code	ends
	end


	
