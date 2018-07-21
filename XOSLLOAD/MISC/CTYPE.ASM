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

                .model  compact
                .386p
                .code

                public  `W?tolower$n(i)i`

;int tolower(int ch);
; Watcom calling convention.
;	ax	dx	bx,cx
;       @@ch: word
`W?tolower$n(i)i`     proc    c
                ;mov     ax,@@ch	; Already loaded
                cmp     ax,'A'
                jb      TLExit
                cmp     ax,'Z'
                ja      TLExit
                or      ax,20h
TLExit:         ret
`W?tolower$n(i)i` endp

                end
