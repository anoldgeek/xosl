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
; Copyright (c) 2018 by Norman Back:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
;
; Use Watcom calling conventions

                .model  tiny
                .386p
		.code

                extrn   `W?CPPMain$N(PFV)V`: near
		
		org	0h

;		.startup
start:
; brec segment passed in ax,dx
		mov	bx,seg _DATA
                mov     ds,bx
                mov     ss,bx
                mov     sp,0fffeh

;		push	ax
;		push	dx

;;                push    dword ptr 50000000h
;		mov	dx,5000h
;		xor	ax,ax
;                call    `W?AllocInit$n(ul)v`
;;               pop     ax
;;		pop	ax

;		; Watcom calling convention
;		;	ax,dx
;		pop	dx
;		pop	ax

                call    `W?CPPMain$N(PFV)V`
		 end
