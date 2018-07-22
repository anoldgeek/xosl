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
; - Implementation GetVect & SetVect not yet checked.
; - Still need to find out how to debug using COM Mouse instead of PS/2 Mouse
;

                .model  large
                .386p
                .code

                public  `W?GetVect$F(I)PF()V`       ;ML: is this correct???
                public  `W?SetVect$F(IPF()V)V`

;void interrupt (*GetVect(int Vect))(void);
`W?GetVect$f(i)pf()v`  proc    syscall,
                @@Vect: word

                xor     ax,ax
                mov     es,ax
                mov     bx,@@Vect
                shl     bx,2
                mov     ax,es:[bx]
                mov     dx,es:[bx + 2]
                ret
`W?GetVect$F(I)PF()V` endp

;void interrupt SetVect(int Vect, void interrupt (*Handler)(void));
`W?SetVect$F(IPF()V)V` proc    syscall,
                @@Vect: word, @@Handler: dword

                xor     ax,ax
                mov     es,ax
                mov     bx,@@Vect
                shl     bx,2
                mov     eax,@@Handler
                cli
                mov     es:[bx],eax
                sti
                ret
`W?SetVect$F(IPF()V)V` endp

                end
