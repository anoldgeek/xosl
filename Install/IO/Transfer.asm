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
		.data
;Scratchpad      dd      00008000h
Scratchpad      dd      90008000h

                .code

                public  `W?$ct:CDiskAccess$f()_`
                public  `W?$dt:CDiskAccess$f()_`
                public  `W?DriveCount$:CDiskAccess$f(i)i`
                public  `W?Transfer$:CDiskAccess$f(iususpfvi)i`
                public  `W?GetDriveInfo$:CDiskAccess$f(irfirfi)i`
                public  `W?CopyFromScratchpad$:CDiskAccess$f(pfvi)i`
                public  `W?CopyToScratchpad$:CDiskAccess$f(pfxvi)i`

; constructor and destructor are not used
`W?$ct:CDiskAccess$f()_`:
`W?$dt:CDiskAccess$f()_`:
                ret

;int CDiskAccess::DriveCount(int Drive)
`W?DriveCount$:CDiskAccess$f(i)i` proc c,
                @@this: dword, @@Drive: word

		mov     ah,8
                mov     dl,byte ptr @@Drive
		int     13h
		movzx   ax,dl
                ret
`W?DriveCount$:CDiskAccess$f(i)i` endp

;int CDiskAccess::Transfer(int Action, unsigned short SectCyl,
;                          unsigned short DrvHead, void *Buffer, int Count);
`W?Transfer$:CDiskAccess$f(iususpfvi)i` proc c,
                @@this: dword, @@Action: word, @@SectCyl: word,
                @@DrvHead: word, @@Buffer: dword, @@Count: word

                test    byte ptr @@DrvHead,80h
		jz      FloppyTransfer

                mov     ax,@@Action
                or      ax,@@Count
                les     bx,@@Buffer
                mov     cx,@@SectCyl
                mov     dx,@@DrvHead

		int     13h
                sbb     ax,ax
                jmp     TransDone

FloppyTransfer:
                push    di
                jmp     TransTestEnd

TransLoop:      mov     di,3

TransSector:    mov     ax,@@Action
                mov     al,1
                les     bx,@@Buffer
                mov     cx,@@SectCyl
                mov     dx,@@DrvHead
                int     13h
                jnc     TransNextSect
                xor     ah,ah                   ;reset drive
                mov     dl,byte ptr @@DrvHead            
                int     13h
                dec     di
                jns     TransSector

                mov     ax,-1
                jmp     FloppyDone

TransNextSect:  add     word ptr @@Buffer,512

TransTestEnd:   dec     @@Count
                jns     TransLoop
                xor     ax,ax
FloppyDone:     pop     di

TransDone:      ret
`W?Transfer$:CDiskAccess$f(iususpfvi)i` endp

;int CDiskAccess::GetDriveInfo(int Drive, int &Heads, int &Sectors);
`W?GetDriveInfo$:CDiskAccess$f(irfirfi)i` proc c,
                @@this: dword, @@Drive: word,
                @@Heads: dword, @@Sectors: dword

		push    di

		mov     ah,8
                mov     dl,byte ptr @@Drive
		int     13h
                sbb     ax,ax
                les     bx,@@Sectors
		and     cx,3fh
		mov     es:[bx],cx
                les     bx,@@Heads
		shr     dx,8
		inc     dx
		mov     es:[bx],dx

		pop     di
                ret
`W?GetDriveInfo$:CDiskAccess$f(irfirfi)i` endp

;void CDiskAccess::CopyFromScratchpad(void *Buffer, int Sectors)
`W?CopyFromScratchpad$:CDiskAccess$f(pfvi)i` proc c,
                @@this: dword, @@Buffer: dword, @@Sectors: word

                push    si
                push    di
                push    ds

		xor     esi,esi
		xor     edi,edi

		lds     si,Scratchpad
                les     di,@@Buffer

                movzx   ecx,@@Sectors
		shl     cx,7

		cld
		rep     movsd

                pop     ds
                pop     di
                pop     si
                ret
`W?CopyFromScratchpad$:CDiskAccess$f(pfvi)i` endp

;void CDiskAccess::CopyToScratchpad(void *Buffer, int Sectors)
`W?CopyToScratchpad$:CDiskAccess$f(pfxvi)i` proc c,
                @@this: dword, @@Buffer: dword, @@Sectors: word

                push    si
                push    di
                push    ds

		xor     esi,esi
		xor     edi,edi

		les     di,Scratchpad
                lds     si,@@Buffer

                movzx   ecx,@@Sectors
		shl     cx,7

		cld
		rep     movsd

                pop     ds
                pop     di
                pop     si
                ret
`W?CopyToScratchpad$:CDiskAccess$f(pfxvi)i` endp

		end
