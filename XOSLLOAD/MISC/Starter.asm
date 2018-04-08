;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;

                .model  tiny
                .386p
		.code
;                extrn   _AllocInit: near
                extrn   _CPPMain: near

		.startup

; Moved to CPPMain
;                push    word ptr 5000h
;		push	word ptr 9000h
;                call    _AllocInit
;                pop     eax

                call    _CPPMain
		 end
