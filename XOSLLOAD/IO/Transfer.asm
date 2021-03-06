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
		.data
Scratchpad      dd      90008000h
;Scratchpad      dd      00008000h
                .code

;                public  @CDiskAccess@$bctr$qv
;                public  @CDiskAccess@$bdtr$qv
;                public  @CDiskAccess@DriveCount$qi
;                public  @CDiskAccess@Transfer$qiususnvi
;                public  @CDiskAccess@GetDriveInfo$qimit2
;                public  @CDiskAccess@CopyFromScratchpad$qnvi
;                public  @CDiskAccess@CopyToScratchpad$qnxvi

                public  `W?$CT:CDiskAccess$N()_`
                public  `W?$DT:CDiskAccess$N()_`
                public  `W?DriveCount$:CDiskAccess$N(I)I`
                public  `W?Transfer$:CDiskAccess$N(IUSUSPFVI)I`
                public  `W?GetDriveInfo$:CDiskAccess$N(IRFIRFI)I`
                public  `W?CopyFromScratchpad$:CDiskAccess$N(PFVI)I`
                public  `W?CopyToScratchpad$:CDiskAccess$N(PFXVI)I`



;CDiskAccess::CDiskAccess()
`W?$CT:CDiskAccess$N()_` proc
                ret
`W?$CT:CDiskAccess$N()_` endp

;CDiskAccess::~CDiskAccess()
`W?$DT:CDiskAccess$N()_` proc
                ret
`W?$DT:CDiskAccess$N()_` endp

;int CDiskAccess::DriveCount(int Drive)
;                @@this: dword, @@Drive: word
; Watcom calling convention.
;	ax,dx		bx		cx
;	@@this		@@drive
`W?DriveCount$:CDiskAccess$N(I)I` proc
		mov     ah,8
;                mov     dl,byte ptr @@Drive
		mov	dl,bl
		int     13h
		movzx   ax,dl
                ret
`W?DriveCount$:CDiskAccess$N(I)I` endp

;int CDiskAccess::Transfer(int Action, unsigned short SectCyl,
;                          unsigned short DrvHead, void *Buffer, int Count);
;                @@this: dword, @@Action: word, @@SectCyl: word,
;                @@DrvHead: word, @@Buffer: dword, @@Count: word
; Watcom calling convention.
;	ax,dx		bx		cx
;	@@this		@@action	@@SectCyl
`W?Transfer$:CDiskAccess$N(IUSUSPFVI)I` proc syscall,
                @@DrvHead: word, @@Buffer: dword, @@Count: word
		push	di
		push	bx
		push	cx
                test    byte ptr @@DrvHead,80h
		jz      FloppyTransfer

;                mov     ax,@@Action
		mov	ax,bx
                or      ax,@@Count
;                mov     cx,@@SectCyl	; Already loaded
                les     bx,@@Buffer
                mov     dx,@@DrvHead

		int     13h
                sbb     ax,ax
                jmp     TransDone

FloppyTransfer:
                jmp     TransTestEnd

TransLoop:      mov     di,3

TransSector:    
;		mov     ax,@@Action
		pop	cx
		pop	ax
		push	ax
		push	cx
                mov     al,1
                les     bx,@@Buffer
;                mov     cx,@@SectCyl	; Already popped
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
FloppyDone:     
		pop	cx
		pop	bx
		pop     di

TransDone:      ret 8
`W?Transfer$:CDiskAccess$N(IUSUSPFVI)I` endp

;int CDiskAccess::GetDriveInfo(int Drive, int &Heads, int &Sectors);
;                @@this: dword, @@Drive: word,
;                @@Heads: dword, @@Sectors: dword
; Watcom calling convention.
;	ax,dx		bx		cx
;	@@this		@@Drive

`W?GetDriveInfo$:CDiskAccess$N(IRFIRFI)I` proc syscall,
                @@Heads: dword, @@Sectors: dword

		push    di
		push	bx
		push	cx

		mov     ah,8
;                mov     dl,byte ptr @@Drive
		mov	dl,bl
		int     13h
                sbb     ax,ax
                les     bx,@@Sectors
		and     cx,3fh
		mov     es:[bx],cx
                les     bx,@@Heads
		shr     dx,8
		inc     dx
		mov     es:[bx],dx

		pop	cx
		pop	bx
		pop     di
                ret	8
`W?GetDriveInfo$:CDiskAccess$N(IRFIRFI)I` endp

;void CDiskAccess::CopyFromScratchpad(void *Buffer, int Sectors)
;               @@this: dword, @@Buffer: dword, 
;		@@Sectors: word
; Watcom calling convention.
;	ax,dx		bx,cx
;	@@this		@@Buffer
`W?CopyFromScratchpad$:CDiskAccess$N(PFVI)I` proc syscall,
 		@@Sectors: word

                push    si
		push	di
		push	ds
		push	bx
		push	cx

		xor     esi,esi
		xor     edi,edi

		lds     si,Scratchpad
;                les     di,@@Buffer
		mov	es,cx
		mov	di,bx

                movzx   ecx,@@Sectors
		shl     cx,7

		cld
		rep     movsd

		pop	cx
		pop	bx
                pop     ds
		pop	di
		pop	si
                ret	2
`W?CopyFromScratchpad$:CDiskAccess$N(PFVI)I` endp

;void CDiskAccess::CopyToScratchpad(void *Buffer, int Sectors)
;                @@this: dword, @@Buffer: dword, 
;		@@Sectors: word
; Watcom calling convention.
;	ax,dx		bx,cx
;	@@this		@@Buffer
`W?CopyToScratchpad$:CDiskAccess$N(PFXVI)I` proc syscall,
 		@@Sectors: word

                push    si
		push	di
		push	ds
		push	bx
		push	cx

		xor     esi,esi
		xor     edi,edi

		les     di,Scratchpad
 ;               lds     si,@@Buffer
		mov	ds,cx
		mov	di,bx

                movzx   ecx,@@Sectors
		shl     cx,7

		cld
		rep     movsd

		pop	cx
		pop	bx
		pop     ds
		pop	di
		pop	si
                ret	2
`W?CopyToScratchpad$:CDiskAccess$N(PFXVI)I` endp

		end
