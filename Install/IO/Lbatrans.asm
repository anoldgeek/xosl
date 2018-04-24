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
; - Watcom Assembler does not support pop/push with multiple parameters
;   For example change
;     push    si ds
;   into   
;     push    si
;     push    ds
;

                .model  large
		.386p
                .code

                public  `W?LBAAccessAvail$:CDiskAccess$f(i)i`
                public  `W?LBATransfer$:CDiskAccess$f(iirfx$__3b5thaTLBAPacket$$)i`
                public  `W?SetLockStatus$:CDiskAccess$f(ii)v`

;int CDiskAccess::LBAAccessAvail(int Drive)
`W?LBAAccessAvail$:CDiskAccess$f(i)i` proc c,
                @@this: dword, @@Drive: word

                mov     ah,41h
                mov     bx,55aah
                mov     dx,@@Drive
                int     13h
                jc      NoLBA
                cmp     bx,0aa55h
                jne     NoLBA
                test    cx,1
                jz      NoLBA
                xor     ax,ax
                jmp     LBA_AAEnd

NoLBA:          mov     ax,-1
LBA_AAEnd:      ret
`W?LBAAccessAvail$:CDiskAccess$f(i)i` endp

;int CDiskAccess::LBATransfer(int Action, int Drive, const TLBAPacket &LBAPacket)
`W?LBATransfer$:CDiskAccess$f(iirfx$__3b5thaTLBAPacket$$)i` proc c,
                @@this: dword, @@Action: word,
                @@Drive: word, @@LBAPacket: dword

                push    si
                push    ds

                mov     ax,@@Action
                or      ax,4000h
                mov     dx,@@Drive
                lds     si,@@LBAPacket
                int     13h
                sbb     ax,ax

                push    si
                push    ds
                ret
`W?LBATransfer$:CDiskAccess$f(iirfx$__3b5thaTLBAPacket$$)i` endp

;void CDiskAccess::SetLockStatus(int Drive, int Status)
`W?SetLockStatus$:CDiskAccess$f(ii)v` proc c,
                @@this: dword,
                @@Drive: word, @@Status: word

                mov     ah,45h
                mov     al,byte ptr @@Status
                mov     dl,byte ptr @@Drive
                int     13h
                ret
`W?SetLockStatus$:CDiskAccess$f(ii)v` endp

		end
