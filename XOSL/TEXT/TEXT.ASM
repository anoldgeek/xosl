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

                public  `W?gotoxy$f(ii)v`
                public  `W?wherexy$f(pfipfi)v`
                public  `W?puts$f(pfxa)v`
                public  `W?putch$f(i)v`

;void gotoxy(int x, int y)
`W?gotoxy$f(ii)v` proc    c,
                @@x: word, @@y: word

                mov     dl,byte ptr @@x
                mov     dh,byte ptr @@y
                mov     ah,2
                xor     bh,bh
                int     10h
                ret
`W?gotoxy$f(ii)v` endp

;void wherexy(int *x, int *y)
`W?wherexy$f(pfipfi)v` proc    c,
                @@x:  dword, @@y: dword

                mov     ah,3
                xor     bh,bh
                int     10h
                les     bx,@@x
                movzx   ax,dl
                mov     es:[bx],ax
                les     bx,@@y
                mov     al,dh
                mov     es:[bx],ax
                ret
`W?wherexy$f(pfipfi)v` endp

;void puts(const char *str);
`W?puts$f(pfxa)v` proc    c,
                @@str: dword

                push    si
                les     si,@@str
                cld
                jmp     PSLods

Teletype:       mov     ah,0eh
                mov     bx,7
                int     10h
PSLods:         lods    es:byte ptr [si]
                or      al,al
                jnz     Teletype
                pop     si
                ret
`W?puts$f(pfxa)v` endp

;void putch(int ch);
`W?putch$f(i)v` proc    c,
                @@ch: word

                mov     ah,0eh
                mov     al,byte ptr @@ch
                mov     bx,7
                int     10h
                ret
`W?putch$f(i)v` endp

                end
