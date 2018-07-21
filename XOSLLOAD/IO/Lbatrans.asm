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

                public  `W?LBAAccessAvail$:CDiskAccess$n(i)i`
                public  `W?LBATransfer$:CDiskAccess$n(iirfx$__3b5thaTLBAPacket$$)i`

;int CDiskAccess::LBAAccessAvail(int Drive)
;                @@this: dword, @@Drive: word
; Watcom calling convention.
;	ax,dx		bx	cx
;       @@this: dword, @@Drive: word

`W?LBAAccessAvail$:CDiskAccess$n(i)i` proc c
		push	bx
		push	cx
                mov     ah,41h
;                mov     dx,@@Drive
		mov	dx,bx
                mov     bx,55aah
                int     13h
                jc      NoLBA
                cmp     bx,0aa55h
                jne     NoLBA
                test    cx,1
                jz      NoLBA
                xor     ax,ax
                jmp     LBA_AAEnd

NoLBA:          mov     ax,-1
LBA_AAEnd:	pop	cx
		pop	bx
		ret
`W?LBAAccessAvail$:CDiskAccess$n(i)i` endp

;int CDiskAccess::LBATransfer(int Action, int Drive, const TLBAPacket &LBAPacket)
;                @@this: dword, @@Action: word,
;		@@Drive: word, @@LBAPacket: dword
; Watcom calling convention.
;	ax,dx		bx		cx
;	@@this		@@action	@@drive
`W?LBATransfer$:CDiskAccess$n(iirfx$__3b5thaTLBAPacket$$)i` proc c,
		@@LBAPacket: dword

                push    si
		push	ds

;                mov     ax,@@Action
		mov	ax,bx

                or      ax,4000h
;                mov     dx,@@Drive
		mov	dx,cx

                lds     si,@@LBAPacket
                int     13h
                sbb     ax,ax

                pop     ds
		pop	si
                ret 2
`W?LBATransfer$:CDiskAccess$n(iirfx$__3b5thaTLBAPacket$$)i` endp



		end
