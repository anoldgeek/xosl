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
                extrn   `W?AllocInit$f(ul)v`: near
                extrn   `W?CPPMain$f()v`: near

		.startup
                push    dword ptr 50000000h
                call    `W?AllocInit$f(ul)v`
                pop     eax

                call    `W?CPPMain$f()v`
		 end
