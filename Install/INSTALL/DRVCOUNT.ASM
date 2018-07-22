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

                .model  large
                .386p

                .code

                public  `W?GetDriveCount$:CData$F()I`
                public  `W?GetDosVersion$:CData$F()US`
                
;int CData::GetDriveCount();
`W?GetDriveCount$:CData$F()I` proc
                mov     ah,52h
                int     21h
                movzx   ax,es:byte ptr [bx + 20h]
                ret
`W?GetDriveCount$:CData$F()I`  endp

;unsigned short CData::GetDosVersion();
`W?GetDosVersion$:CData$F()US` proc
                mov     ah,30h
                int     21h
                ret
`W?GetDosVersion$:CData$F()US` endp

                end
                
