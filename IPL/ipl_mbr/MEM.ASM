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

;/*
; * File  : MEM.ASM
; * Author: Geurt Vos
; * Date  : March 1999
; *
; * Memory compare function
; */

                .model  tiny
                .386p
                .code
                public  MemCompare

;int MemCompare(const void far *s1, const void far *s2, U16B count);
;Return: ax == 0 on equal
MemCompare      proc    
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds

                xor     ax,ax
                les     di,[bp + 4]
                lds     si,[bp + 8]
                mov     cx,[bp + 12]
                cld
                repz     cmpsb		;ML replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
                setne   al
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret	10		; Adjust stack for caller
MemCompare      endp

                end
