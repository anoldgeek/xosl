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
; - To correct Error! E004: REP prefix is not allowed on this instruction
;   replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
;

                .model  compact
                .386p
                .code

                
                public  `W?MemCompare$n(pfxvpfxvi)i`

;int MemCompare(const void *Mem1, const void *Mem2, int ByteCount)
`W?MemCompare$n(pfxvpfxvi)i`  proc c,
                @@Mem1: dword, @@Mem2: dword,
                @@ByteCount: word


                push    si 
                push    di 
                push    ds

                les     di,@@Mem1

                lds     si,@@Mem2

                mov     cx,@@ByteCount
                xor     ax,ax
                cld
		; replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
                ; rep     cmpsb
                repz     cmpsb
                setne   al
                pop     ds 
                pop     di
                pop     si
                ret
`W?MemCompare$n(pfxvpfxvi)i` endp


                end
