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
                public  `W?memcpy$N(PFVPFXVUS)V`

;		@@dest: dword, @@str: dword, 
;		@@size: word
; Watcom calling convention.
;	ax,dx	bx,cx
;	@@dest  @@str
`W?memcpy$N(PFVPFXVUS)V`	proc	syscall,
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
`W?memcpy$N(PFVPFXVUS)V`	endp

;void memset(void *dest, int value, unsigned short count);
                public  `W?memset$N(PNVIUS)V`
;		@@dest: dword, @@value: word, @@count: word
; Watcom calling convention.
;	ax,dx	bx	 cx
;	@@dest  @@value @@count		
`W?memset$N(PNVIUS)V` proc
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
`W?memset$N(PNVIUS)V` endp

;int memcmp( void const far *, void const far*, short unsigned )
                public  `W?memcmp$N(PFXVPFXVUS)I`
;		@@s1: dword, @@s2: dword, 
;		@@count: word
; Watcom calling convention.
;	ax,dx	bx,cx
;	@@s1	@@s2
`W?memcmp$N(PFXVPFXVUS)I` proc syscall,
		@@count: word
                push    si
                push    di
                push    ds
		push	cx

;                les     di,@@s1
		mov	ds,dx
		mov	si,ax

;                lds     si,@@s2
		mov	es,cx
		mov	di,bx

                mov     cx,@@count
                xor     ax,ax
                cld
                repz     cmpsb ; ML replace rep cmpsb with repz cmpsb  (repz: repeat while CX!=0 and zero-flag=1)
		jz	memcmp2
		jns	memcmp1
		dec	ax
		jmp	memcmp2
memcmp1:	inc	ax
memcmp2:
		pop	cx
                pop     ds
                pop     di
                pop     si
                ret	2
`W?memcmp$N(PFXVPFXVUS)I` endp


                end
