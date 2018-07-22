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
                .model  compact
		.386p
                .code

                public  `W?LBARead$N(IRFX$__5fds00TLBAPacket$$)V`


;void LBARead(int Drive, const TLBAPacket &LBAPacket)
`W?LBARead$N(IRFX$__5fds00TLBAPacket$$)V` proc syscall,
                @@Drive: word, @@LBAPacket: dword

                push    si
                push    ds

                mov     dx,@@Drive
                lds     si,@@LBAPacket
                mov     ah,42h
                int     13h

                pop     ds
                pop     si
                ret
`W?LBARead$N(IRFX$__5fds00TLBAPacket$$)V` endp

		end
