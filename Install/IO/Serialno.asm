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
; - Correct Error! E523: Segment name is missing by commenting out ends
;   Is the use of ends before .code a mistake? 
;   This file is the only *.asm file in the xosl project where it is used.
; - When using 
;     DosDriveInfo    equ     this byte
;   the instruction
;     mov     dx,offset DosDriveInfo
;   results in Error! E066: Operand is expected
;   When using
;     DosDriveInfo    label   byte
;   compilation is without errors
;

                .model  large
                .386p
                .data?
;DosDriveInfo    equ     this byte
DosDriveInfo    label   byte
Reserved        dw      ?       ;0000
SerialNumLo     dw      ?
SerialNumHi     dw      ?
VolumeLabel     db      11 dup (?)
FSType          db      8 dup (?)
;                ends

                .code

		extern	`W?HDOffset$FI`: far

                public  `W?GetDosSerialNo$:CDosDriveList$F(I)UL`

;unsigned long CDOSDriveList::GetDOSSerialNo(int DriveNum);
`W?GetDosSerialNo$:CDosDriveList$F(I)UL` proc syscall,
                @@this: dword, @@DriveNum: word

                mov     bx,@@DriveNum
		add	bx,word ptr ss:W?HDOffset$FI
                mov     ax,6900h
                add     bx,3
                mov     dx,offset DosDriveInfo
                int     21h
                jnc     GSNFound
                mov     ax,0ffffh
                mov     dx,ax
                jmp     GSNExit

GSNFound:       mov     ax,SerialNumLo
                mov     dx,SerialNumHi

GSNExit:        ret
`W?GetDosSerialNo$:CDosDriveList$F(I)UL` endp

                end
