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
                .data?

                .code

                public  `W?$ct:CKeyboard$n()_`
                public  `W?$dt:CKeyboard$n()_`
                public  `W?GetCh$:CKeyboard$n()i`
                public  `W?KbHit$:CKeyboard$n()i`
                public  `W?Flush$:CKeyboard$n()v`
                public  `W?StoreCh$:CKeyboard$n(i)v`

;constructure and destructor are not needed
`W?$ct:CKeyboard$n()_`:
`W?$dt:CKeyboard$n()_`: ret


;int CKeyboard::Getch()
`W?GetCh$:CKeyboard$n()i`     proc
                xor     ah,ah
                int     16h
                ret
`W?GetCh$:CKeyboard$n()i` endp

;int CKeyboard::KbHit()
`W?KbHit$:CKeyboard$n()i`     proc
                mov     ah,1
                int     16h
                jz      KBHNoKey
                mov     ax,0ffffh
                ret
KBHNoKey:       xor     ax,ax
                ret
`W?KbHit$:CKeyboard$n()i` endp


;void CKeyboard::Flush()
`W?Flush$:CKeyboard$n()v`     proc
                jmp     FCheckBuffer
FGetKey:        xor     ah,ah
                int     16h
FCheckBuffer:   mov     ah,1
                int     16h
                jnz     FGetKey
                ret
`W?Flush$:CKeyboard$n()v`     endp

;void CKeyboard::StoreCh()
`W?StoreCh$:CKeyboard$n(i)v`     proc    c,
                @@this: dword, @@Key: word

                mov     ah,5
                mov     cx,@@Key
                int     16h
                ret
`W?StoreCh$:CKeyboard$n(i)v`     endp
		end