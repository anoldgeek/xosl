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

                .model  compact
                .386p
                .code

;void memcpy (void far *dest, void far *str, U16B size);
;                public  _memcpy
		public  `W?memcpy$f(pfvpfxvus)v`
`W?memcpy$f(pfvpfxvus)v` proc    
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                les     di,[bp + 4]
                lds     si,[bp + 8]
                mov     cx,[bp + 12]
                shr     cx,1
                cld
                rep     movsw
                jnc     MemCpyDone
                movsb

MemCpyDone:     pop     ds
                pop     di
                pop     si
                pop     bp
                ret
`W?memcpy$f(pfvpfxvus)v` endp

;void memset(void far *dest, U8B value, U16B count);
                public  _memset
_memset         proc    
                push    bp
                mov     bp,sp
                push    di
                les     di,[bp + 4]
                mov     ax,[bp + 8]
                mov     ah,al
                mov     cx,[bp + 10]
                shr     cx,1
                cld
                rep     stosw
                jnc     MemSetDone
                stosb

MemSetDone:     pop     di
                pop     bp
                ret
_memset         endp

;int memcmp(const void far *s1, const void far *s2, U16B count); 
;int far memcmp( void const far *, void const far *, short unsigned )
;                public  _memcmp
		public `W?memcmp$f(pfxvpfxvus)i`
`W?memcmp$f(pfxvpfxvus)i` proc c,
		@@s1: dword, @@s2: dword, @@count: word
                push    si
                push    di
                push    ds

                xor     ax,ax
                les     di,@@s1
                lds     si,@@s2
                mov     cx,@@count
                cld
                repz     cmpsb ; ML replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
                setne   al
                pop     ds
                pop     di
                pop     si
                ret
`W?memcmp$f(pfxvpfxvus)i` endp


                end
