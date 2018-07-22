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
                .code
                public  `W?GetTimerTicks$F()UL`

;unsigned long GetTimerTicks(void);
`W?GetTimerTicks$F()UL` proc
                xor     ax,ax
                mov     es,ax
                mov     ax,es:[046ch]
                mov     dx,es:[046eh]
                ret
`W?GetTimerTicks$F()UL` endp

                end

