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
; - To correct Error! E004: REP prefix is not allowed on this instruction
;   replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
;

                .model  large
                .386p
                .code
	        public	MemCopy
		public	MemSet
		public	MemCompare


;void memcpy (void far *dest, void far *src, U16B size);
                
MemCopy        proc    c,
                @@Dest: dword,
                @@Src: dword,
                @@Count: word

                push    si
                push    di
                push    ds
                les     di,@@Dest
                lds     si,@@Src
                mov     cx,@@Count
                shr     cx,1
                cld
                rep     movsw
                jnc     MemCpyDone
                movsb

MemCpyDone:     pop     ds
                pop     di
                pop     si
                ret
MemCopy        endp

;void memset(void far *dest, U8B value, U16B count);
MemSet         proc    c,
                @@Dest: dword,
                @@Value: word,
                @@Count: word
		
                push    di
                les     di,@@Dest
                mov     ax,@@Value
                mov     ah,al
                mov     cx,@@Count
                shr     cx,1
                cld
                rep     stosw
                jnc     MemSetDone
                stosb

MemSetDone:     pop     di
                ret
MemSet         endp

;int memcmp(const void far *s1, const void far *s2, U16B count); 
MemCompare     proc    c,
                @@Ptr1: dword,
                @@Ptr2: dword,
                @@Count: word

                push    si
                push    di
                push    ds

                xor     ax,ax
                les     di,@@Ptr1
                lds     si,@@Ptr2
                mov     cx,@@Count
                cld
                repz     cmpsb	;replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
                setne   al
                pop     ds
                pop     di
                pop     si
                ret
MemCompare		endp


                end
