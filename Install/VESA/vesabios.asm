;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;
;
; Open Watcom Migration
; Copyright (c) 2010 by Mario Looijkens:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
;

                .model  large
                .386p
                .code


                public  `W?GetSvgaInfo$:CVesa$f(rf$CSvgaInfo$:1$)i`
                public  `W?GetModeInfo$:CVesa$f(irf$CModeInfo$:1$)i`

;static int CVesa::GetSvgaInfo(CVesa::CSvgaInfo &SvgaInfo);
`W?GetSvgaInfo$:CVesa$f(rf$CSvgaInfo$:1$)i` proc c,
                @@SvgaInfo: dword

                push    di

                mov     ax,4f00h
                les     di,@@SvgaInfo
                int     10h
                cmp     ax,004fh
                je      GSISuccess
                mov     ax,-1
                jmp     GSIExit

GSISuccess:     xor     ax,ax
GSIExit:        pop     di
                ret
`W?GetSvgaInfo$:CVesa$f(rf$CSvgaInfo$:1$)i` endp

;int CVesa::GetModeInfo(int VesaMode, CModeInfo &ModeInfo)
`W?GetModeInfo$:CVesa$f(irf$CModeInfo$:1$)i` proc c,
                @@VesaMode: word, @@ModeInfo: dword

                push    di
                mov     ax,4f01h
                mov     cx,@@VesaMode
                les     di,@@ModeInfo
                int     10h
                xor     ax,004fh
                jz      MIExit
                mov     ax,-1

MIExit:         pop     di
                ret
`W?GetModeInfo$:CVesa$f(irf$CModeInfo$:1$)i` endp



                end
