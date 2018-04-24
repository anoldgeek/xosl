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
                .data?

                public  `W?APMAvailable$:CPowerManagement$fi`
                public  _PowerManagement

`W?APMAvailable$:CPowerManagement$fi` dw ?
_PowerManagement        dd      ?

                .code

                public  `W?APMInstalled$:CPowerManagement$f()i`
                public  `W?Connect$:CPowerManagement$f()i`
                public  `W?SetPowerState$:CPowerManagement$f(ii)i`

;static int CPowerManagement::APMInstalled();
`W?APMInstalled$:CPowerManagement$f()i` proc
                mov     ax,5300h
                xor     bx,bx
                int     15h
                sbb     ax,ax
                inc     ax
                ret
`W?APMInstalled$:CPowerManagement$f()i` endp

;static int CPowerManagement::Connect();
`W?Connect$:CPowerManagement$f()i` proc
                mov     ax,5301h
                xor     bx,bx
                int     15h
                sbb     ax,ax
                ret
`W?Connect$:CPowerManagement$f()i` endp

;static int CPowerManagement::SetPowerState(int DevId, int State);
`W?SetPowerState$:CPowerManagement$f(ii)i` proc c,
                @@DevId: word, @@State: word

                cmp     `W?APMAvailable$:CPowerManagement$fi`,0
                jne     SPSProceed
                mov     ax,-1
                jmp     SPSExit

SPSProceed:     mov     ax,5307h
                mov     bx,@@DevId
                mov     cx,@@State
                int     15h
                sbb     ax,ax

SPSExit:        ret
`W?SetPowerState$:CPowerManagement$f(ii)i` endp

                end
