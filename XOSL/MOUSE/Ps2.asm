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

                public  `W?PS2Initialize$F(I)I`
                public  `W?PS2Call$F(USI)V`
                public  `W?PS2SetHandler$F(PF(SAAS)V)V`

;int PS2Initialize(int PackSize)
`W?PS2Initialize$F(I)I` proc syscall,
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
`W?PS2Initialize$F(I)I` endp

;void PS2Call(U16B Func, int Value)
`W?PS2Call$F(USI)V` proc    syscall,
                @@Func: word, @@Value: word

                mov     ax,@@Func
                mov     bh,byte ptr @@Value
                int     15h
                ret
`W?PS2Call$f(usi)v` endp

;void PS2SetHandler(TPS2Handler Handler)
`W?PS2SetHandler$F(PF(SAAS)V)V` proc syscall,
                @@Handler: dword

                mov     ax,0c207h
                les     bx,@@Handler
                int     15h
                ret
`W?PS2SetHandler$F(PF(SAAS)V)V` endp

                end

