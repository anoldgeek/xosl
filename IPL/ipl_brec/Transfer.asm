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
                .data?

;                public  Drive
;                public  StartSector

                public  `W?Drive$NI`
                public  `W?StartSector$NUL`


DrvHeadCount    dd      ?
DrvSectorCount  dd      ?
`W?Drive$NI`          dw      ?
;`W?StartSector$NUL`    label	dword	; relies on little endian
`W?StartSector$NUL`    dd      ?


;'local' vars for Sector2CHS
;(don't know how to control 'enter' with 'proc c')
;Cylinder        dw      ?
Sector          dw      ?
;Head            dw      ?


                .code

	       public `W?ConvRead$N(USUSPFVI)V`
	       public `W?GetDriveInfo$N(I)V`
	       public `W?Sector2CHS$N(ULRNUSRNUS)V`

;void ConvRead(unsigned short SectCyl,unsigned short DrvHead,
;             void *Buffer, int Count);
`W?ConvRead$N(USUSPFVI)V`      proc syscall,
                @@SectCyl: word, @@DrvHead: word, @@Buffer: dword,
		@@Count: word

                les     bx,@@Buffer
                mov     cx,@@SectCyl
                mov     dx,@@DrvHead
                mov     ah,02h
                mov     al,byte ptr @@Count
		int     13h
                ret
`W?ConvRead$N(USUSPFVI)V` endp

;void GetDriveInfo(int Drive);
`W?GetDriveInfo$N(I)V`        proc syscall,
                @@Drive: word

		push    di

                mov     dl,byte ptr @@Drive
		mov     ah,8
		int     13h

                and     cl,3fh
                movzx   ecx,cl
                mov     DrvSectorCount,ecx

                inc     dh
                movzx   edx,dh
                mov     DrvHeadCount,edx

		pop     di
                ret
`W?GetDriveInfo$N(I)V` endp

;void Sector2CHS(unsigned long RSector, unsigned short &SectCyl, unsigned short &DrvHead)
`W?Sector2CHS$N(ULRNUSRNUS)V`   proc syscall,
                @@RSector: dword, 
		@@SectCyl: dword, @@DrvHead: dword

                ;RSector += StartSector
                mov     eax,@@RSector
                add     eax,`W?StartSector$NUL`

                ;Sector = RSector % DrvSectorCount + 1
                ;RSector /= DrvSectorCount
                xor     edx,edx
                div     DrvSectorCount
                inc     dx
                mov     Sector,dx

                ;Head = RSector % DrvHeadCount
                ;Cylinder = RSector / DrvHeadCount
                xor     dx,dx
                div     dword ptr DrvHeadCount

                ;DrvHead = Drive | (Head << 8)
                mov     dh,dl
                mov     dl,byte ptr `W?Drive$ni`
                les     bx,@@DrvHead
                mov     es:[bx],dx

                ;((Cylinder & 0xff) << 8)
                mov     dh,al

                ; ((Cylinder >> 8) << 6)
                mov     dl,ah
                shl     dl,6

                ; SectCyl = Sector | ((Cylinder & 0xff) << 8) | ....
                or      dl,byte ptr Sector
                les     bx,@@SectCyl
                mov     es:[bx],dx


                ret
`W?Sector2CHS$N(ULRNUSRNUS)V` endp

		end
