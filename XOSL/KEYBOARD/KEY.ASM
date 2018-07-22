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

                public  `W?WaitKeyStrokeDef$:CKeyboard$F()US`
                public  `W?WaitKeyStrokeEnh$:CKeyboard$F()US`
                public  `W?StoreKeyStroke$:CKeyboard$F(US)V`
                public  `W?KeyStrokeAvailDef$:CKeyboard$F()Q`
                public  `W?KeyStrokeAvailEnh$:CKeyboard$F()Q`




;unsigned short CKeyboard::WaitKeyStrokeDef()
`W?WaitKeyStrokeDef$:CKeyboard$F()US` proc
                xor     ah,ah
                int     16h
                ret
`W?WaitKeyStrokeDef$:CKeyboard$F()US` endp

`W?WaitKeyStrokeEnh$:CKeyboard$F()US` proc
		mov	ah,10h
		int	16h
		ret
`W?WaitKeyStrokeEnh$:CKeyboard$F()US` endp


;void CKeyboard::StoreKeyStroke(unsigned short KeyCode)
; Watcom calling convention.
;	ax	dx	bx,cx
`W?StoreKeyStroke$:CKeyboard$F(US)V` proc 
;                @@KeyCode: word
;                mov     cx,@@KeyCode
                mov     cx,ax
                mov     ah,5
                int     16h
                ret
`W?StoreKeyStroke$:CKeyboard$F(US)V` endp


;bool CKeyboard::KeyStrokeAvailDef();
`W?KeyStrokeAvailDef$:CKeyboard$F()Q` proc
                mov     ah,1
                int     16h
                mov     ax,1
                jnz     KHDEnd
                dec     ax
KHDEnd:         ret
`W?KeyStrokeAvailDef$:CKeyboard$F()Q` endp


;bool CKeyboard::KeyStrokeAvailEnh();
`W?KeyStrokeAvailEnh$:CKeyboard$F()Q` proc
                mov     ah,11h
                int     16h
                mov     ax,1
                jnz     KHEEnd
                dec     ax
KHEEnd:         ret
`W?KeyStrokeAvailEnh$:CKeyboard$F()Q` endp



                end
