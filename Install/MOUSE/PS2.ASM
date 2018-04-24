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
;

                .model  compact
                .386p
                .code
                public  `W?Ps2Initialize$:CMouse$n(i)i`

;int CMouse::PS2Initialize(int PackSize);
`W?Ps2Initialize$:CMouse$n(i)i` proc c,
                @@PackSize: word

                mov     ax,0c205h
                mov     bh,byte ptr @@PackSize
                int     15h
                or      ah,ah
                jnz     PS2IErr
                mov     ax,0c201h
                int     15h
                or      ah,ah
                jz      PS2IEnd

PS2IErr:        mov     ah,1
PS2IEnd:        shr     ax,8
                ret
`W?Ps2Initialize$:CMouse$n(i)i` endp

                end

