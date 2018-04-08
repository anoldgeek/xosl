;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;

                .model  compact
                .386p
		.stack  6000h
                .data
		.code
                extrn   @AllocInit$qul: far
                extrn   _main: far
                extrn   @ResetTimer$qv: far
                extrn   _EnableA20: far
                extrn   _DisableA20: far
                extrn   _PrintA20Status: far

		; Check if started by xoslloader
		mov	ax,cs
		cmp	ax,02000h 
		je	loadedby_xoslloader

		; loaded by dos or debugger
;		.startup
		MOV	 DX,@data
		MOV	 DS,DX
		MOV	 BX,SS
		SUB	 BX,DX
		SHL	 BX,4
		MOV	 SS,DX
		ADD	 SP,BX

		; should get FreeSegStart and End from command line
		push	07000h;
		push	08000h;
		jmp	starter2

loadedby_xoslloader:
		; Get FreeSegStart and End from Stack
		mov	bp,sp
		mov	ax,[bp+06h]
		push	ax
		mov	ax,[bp+08h]
		push	ax

starter2:
		call 	_EnableA20
                push    dx
                call    _PrintA20Status
                pop     dx

                call    _main

                push    ax              ;boot drive 
                ;call    @ResetTimer$qv
		call	_DisableA20
                push    dx
                call    _PrintA20Status
                pop     dx
		
                pop     dx              ;Solaris needs drive no. in dl

		;Flush keyboard
                mov     bx,0040h
                mov     es,bx         ;set segment to 0040h
                cli                   ;need head not to be changed
                mov     bx,es:[001Ah] ;read head
                mov     es:[001Ch],bx ;set tail
                sti                   ;now restore IRQs
                xor     bx, bx

                ifdef DOS_DEBUG
                db      0eah      ; jmp far 8000:7c00
                dd      80007c00h
                else
                db      0eah      ; jmp far 0000:7c00
                dd      00007c00h ; standard mbr load address
                endif

                db      0eah
                dd      00007c00h
                
;;;;;;;;;;

;include		A20LINE.ASM



		 end
