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
; Copyright (c) 2018 by Norman Back:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
;

                .model  compact
                .386p
                .code

;void memcpy (void far *dest, void far *str, U16B size);
                public  `W?memcpy$n(pfvpfxvus)v`

;		@@dest: dword, @@str: dword, 
;		@@size: word
; Watcom calling convention.
;	ax,dx	bx,cx
;	@@dest  @@str
`W?memcpy$n(pfvpfxvus)v`	proc	c,
		@@size: word
                push    si
                push    di
                push    ds
		push	cx
		push	es
;                les     di,@@dest
		mov	es,dx
		mov	di,ax

;                lds     si,@@str
		mov	ds,cx
		mov	si,bx

                mov     cx,@@size
                shr     cx,1
                cld
                rep     movsw
                jnc     MemCpyDone
                movsb

MemCpyDone:     pop	es
		pop	cx
		pop     ds
                pop     di
                pop     si
                ret	2
`W?memcpy$n(pfvpfxvus)v`	endp

;void memset(void *dest, int value, unsigned short count);
                public  `W?memset$n(pnvius)v`
;		@@dest: dword, @@value: word, @@count: word
; Watcom calling convention.
;	ax,dx	bx	 cx
;	@@dest  @@value @@count		
`W?memset$n(pnvius)v` proc c
                push    di
;                les     di,@@dest
		mov	es,dx
		mov	di,ax
		
;                mov     ax,@@value
		mov	ax,bx
                mov     ah,al
;                mov     cx,@@count	; already loaded
                shr     cx,1
                cld
                rep     stosw
                jnc     MemSetDone
                stosb

MemSetDone:     pop     di
                ret
`W?memset$n(pnvius)v` endp

;int memcmp( void const far *, void const far*, short unsigned )
                public  `W?memcmp$n(pfxvpfxvus)i`
;		@@s1: dword, @@s2: dword, 
;		@@count: word
; Watcom calling convention.
;	ax,dx	bx,cx
;	@@s1	@@s2
`W?memcmp$n(pfxvpfxvus)i` proc c,
		@@count: word
                push    si
                push    di
                push    ds
		push	cx

;                les     di,@@s1
		mov	es,dx
		mov	di,ax

;                lds     si,@@s2
		mov	ds,cx
		mov	si,bx

                mov     cx,@@count
                xor     ax,ax
                cld
                repz     cmpsb ; ML replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
                setne   al
		pop	cx
                pop     ds
                pop     di
                pop     si
                ret	2
`W?memcmp$n(pfxvpfxvus)i` endp


                end
