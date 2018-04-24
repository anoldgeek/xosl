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

                .model  large
                .386p

                .code

                public  `W?WaitKeyStrokeDef$:CKeyboard$f()us`
                public  `W?WaitKeyStrokeEnh$:CKeyboard$f()us`
                public  `W?StoreKeyStroke$:CKeyboard$f(us)v`
                public  `W?KeyStrokeAvailDef$:CKeyboard$f()q`
                public  `W?KeyStrokeAvailEnh$:CKeyboard$f()q`




;unsigned short CKeyboard::WaitKeyStrokeDef()
`W?WaitKeyStrokeDef$:CKeyboard$f()us` proc
                xor     ah,ah
                int     16h
                ret
`W?WaitKeyStrokeDef$:CKeyboard$f()us` endp

`W?WaitKeyStrokeEnh$:CKeyboard$f()us` proc
		mov	ah,10h
		int	16h
		ret
`W?WaitKeyStrokeEnh$:CKeyboard$f()us` endp


;void CKeyboard::StoreKeyStroke(unsigned short KeyCode)
`W?StoreKeyStroke$:CKeyboard$f(us)v` proc c,
                @@KeyCode: word

                mov     ah,5
                mov     cx,@@KeyCode
                int     16h
                ret
`W?StoreKeyStroke$:CKeyboard$f(us)v` endp


;bool CKeyboard::KeyStrokeAvailDef();
`W?KeyStrokeAvailDef$:CKeyboard$f()q` proc
                mov     ah,1
                int     16h
                mov     ax,1
                jnz     KHDEnd
                dec     ax
KHDEnd:         ret
`W?KeyStrokeAvailDef$:CKeyboard$f()q` endp


;bool CKeyboard::KeyStrokeAvailEnh();
`W?KeyStrokeAvailEnh$:CKeyboard$f()q` proc
                mov     ah,11h
                int     16h
                mov     ax,1
                jnz     KHEEnd
                dec     ax
KHEEnd:         ret
`W?KeyStrokeAvailEnh$:CKeyboard$f()q` endp



                end
