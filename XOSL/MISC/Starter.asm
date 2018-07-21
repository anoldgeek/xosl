;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;
; Open Watcom Migration
; Copyright (c) 2010 by Mario Looijkens:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
; - ToDo: Rename function main to mymain
; - Different MMU start address for DOS debug version and release version
; - Use better A20 line switching implementation
; - Flush keyboard before handing over control to OS bootsector
;

                .model  compact
                .386p
		.stack  6000h
                .data
		.code

;                extrn   `W?AllocInit$f(ul)v`: far
                ;extrn   _mymain: far                     ;ML
                extrn   main_: far
                extrn   `W?ResetTimer$f()v`: far
                extrn   _EnableA20: far
                extrn   _DisableA20: far
                extrn   _PrintA20Status: far


;		.startup
		mov	dx,DGROUP
		mov	ds,dx
;		mov	dx,STACK
;		mov	ax,1000;
;		add	ax,dx
		mov	ss,dx
		mov	ax,0fffeh
		mov	sp,ax
		mov	bp,ax

		call 	_EnableA20
                push    dx
                call    _PrintA20Status
                pop     dx

		call    main_
                push    ax              ;boot drive 

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
