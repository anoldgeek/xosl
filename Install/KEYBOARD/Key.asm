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
                .data?

                .code

                public  `W?$CT:CKeyboard$F()_`
                public  `W?$DT:CKeyboard$F()_`
                public  `W?GetCh$:CKeyboard$F()I`
                public  `W?KbHit$:CKeyboard$F()I`
                public  `W?Flush$:CKeyboard$F()V`
                public  `W?StoreCh$:CKeyboard$F(I)V`

;constructure and destructor are not needed
`W?$CT:CKeyboard$f()_`:
`W?$DT:CKeyboard$f()_`: ret


;int CKeyboard::Getch()
`W?GetCh$:CKeyboard$F()I`     proc
                xor     ah,ah
                int     16h
                ret
`W?GetCh$:CKeyboard$f()i` endp

;int CKeyboard::KbHit()
`W?KbHit$:CKeyboard$F()I`     proc
                mov     ah,1
                int     16h
                jz      KBHNoKey
                mov     ax,0ffffh
                ret
KBHNoKey:       xor     ax,ax
                ret
`W?KbHit$:CKeyboard$F()I` endp


;void CKeyboard::Flush()
`W?Flush$:CKeyboard$F()V`     proc
                jmp     FCheckBuffer
FGetKey:        xor     ah,ah
                int     16h
FCheckBuffer:   mov     ah,1
                int     16h
                jnz     FGetKey
                ret
`W?Flush$:CKeyboard$F()V`     endp

;void CKeyboard::StoreCh()
`W?StoreCh$:CKeyboard$F(I)V`     proc    syscall,
                @@this: dword, @@Key: word

                mov     ah,5
                mov     cx,@@Key
                int     16h
                ret
`W?StoreCh$:CKeyboard$F(I)V`     endp
		end
