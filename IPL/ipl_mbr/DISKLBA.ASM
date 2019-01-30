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
; - Comment out extrn DiskSectors: dword to correct 
;   Error! E2028: DiskSectors is an undefined reference
; - Comment out extrn DiskHeads: dword to correct 
;   Error! E2028: DiskHeads is an undefined reference 
;
;/*
; * File  : DISK.ASM
; * Author: Geurt Vos
; * Date  : March 1999
; *
; * Reads sectors from the 'mapped' partition
; */

                .model  tiny
                .386p
                .data

                extrn   DriveNumber: byte
                ;extrn   DiskSectors: dword
                ;extrn   DiskHeads: dword
                extrn   ABSSectorStart: qword

                extrn   LBAPacket: byte
                extrn   SectorCount: word
                extrn   TransferBuffer: dword
                extrn   Sector: qword

                .code
                public  ReadSectors

;int ReadSectors(long Sector, void *Buffer, int Count)
ReadSectors     proc
                push    bp
                mov     bp,sp
                push    si

;		mov     eax,[bp + 4]
;                add     eax,ABSSectorStart
;                mov     dword ptr Sector,eax
		; ABSSectorStart is now unsigned long long to allow booting from > 2TB HDs
		mov	eax, dword ptr ABSSectorStart
		mov	edx, dword ptr ABSSectorStart + 4
		add	eax,[bp + 4]
		adc	edx,0
		mov	dword ptr Sector,eax
		mov	dword ptr Sector+4,edx

                mov     eax,[bp + 8]
                mov     TransferBuffer,eax
                mov     ax,[bp + 12]
                mov     SectorCount,ax
                mov     ah,42h
                mov     dl,DriveNumber
                mov     si,offset LBAPacket
                int     13h
                sbb     ax,ax                  ;ax = cf ? -1 : 0

                pop     si
                pop     bp
                ret	10		; Adjust stack for caller
ReadSectors     endp

                end
