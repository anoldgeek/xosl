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
;     push    si di ds
;   into   
;     push    si
;     push    di
;     push    ds
;

                .model  large
                .386p
                .data
TextScreenPtr   equ     0b8000000h

TextScreenSeg   dw      0b800h
                .data?

CursorPos       dw      ?
CursorSize      dw      ?
ScreenBuffer    db      160 * 25 dup (?)
                .code

                extrn   MemCopy_: far

                public  `W?$ct:CTextScreen$f(i)_`
                public  `W?$dt:CTextScreen$f()_`

                public  `W?PutStr$:CTextScreen$f(iipfxai)v`
                public  `W?PutChar$:CTextScreen$f(iiii)v`
                public  `W?FillX$:CTextScreen$f(iiiii)v`
                public  `W?FillY$:CTextScreen$f(iiiii)v`
                public  `W?PutImage$:CTextScreen$f(iiiipfus)v`
                public  `W?GetImage$:CTextScreen$f(iiiipfus)v`

;CTextScreen(int Attr)
`W?$ct:CTextScreen$f(i)_`   proc    c,
                @@this: dword, @@Attr: word

                push    di

                ;get cursor pos en size
                mov     ah,3
                xor     bh,bh
                int     10h
                mov     CursorPos,dx
                mov     CursorSize,cx

                ;hide cursor
                mov     ax,0103h
                mov     cx,2000h
                int     10h

                ;backup text screen
                push    160 * 25
                push    TextScreenPtr
                push    ds
                push    offset ScreenBuffer
                call    MemCopy_
                add     sp,10

                ;clear screen
                mov     es,TextScreenSeg
                xor     di,di
                mov     cx,80 * 25
                mov     ax,@@Attr
                cld
                rep     stosw

                pop     di
                ret
`W?$ct:CTextScreen$f(i)_` endp                

;~ClearScreen()
`W?$dt:CTextScreen$f()_`   proc    c

                ;restore text screen
                push    160 * 25
                push    ds
                push    offset ScreenBuffer
                push    TextScreenPtr
                call    MemCopy_
                add     sp,10

                ;restore cursor pos
                mov     ah,2
		xor	bh,bh
                mov     dx,CursorPos
                int     10h

                ;restore cursor size
                mov     ah,1
                mov     cx,CursorSize
                int     10h

                ret
`W?$dt:CTextScreen$f()_`  endp

;void CTextScreen::PutStr(int X, int Y, const char *Str, int Attr);
`W?PutStr$:CTextScreen$f(iipfxai)v` proc    c,
                @@this: dword, @@X: word, @@Y: word, @@Str: dword, @@Attr: word

                push    si
                push    di
                push    ds

                mov     es,TextScreenSeg
                mov     di,@@Y
                imul    di,80
                add     di,@@X
                shl     di,1
                lds     si,@@Str
                mov     ax,@@Attr
                jmp     PSXYGetChar

PSXYPutChar:    stosw
PSXYGetChar:    lodsb
                or      al,al
                jnz     PSXYPutChar

                pop     ds
                pop     di
                pop     si
                ret
`W?PutStr$:CTextScreen$f(iipfxai)v` endp

;void CTextScreen::PutChar(int X, int Y, int Ch, int Attr);
`W?PutChar$:CTextScreen$f(iiii)v` proc c,
                @@this: dword, @@X: word, @@Y: word,
                @@Ch: word, @@Attr: word

                mov     es,TextScreenSeg
                mov     bx,@@Y
                imul    bx,80
                add     bx,@@X
                shl     bx,1
                mov     ax,@@Ch
                or      ax,@@Attr
                mov     es:[bx],ax
                ret
`W?PutChar$:CTextScreen$f(iiii)v` endp

;void CTextScreen::FillX(int X, int Y, int Ch, int Attr, int Count);
`W?FillX$:CTextScreen$f(iiiii)v` proc  c,
                @@this: dword, @@X: word, @@Y: word,
                @@Ch: word, @@Attr: word, @@Count: word

                push    di

                mov     es,TextScreenSeg
                mov     di,@@Y
                imul    di,80
                add     di,@@X
                shl     di,1

                mov     ax,@@Ch
                or      ax,@@Attr
                mov     cx,@@Count
                cld
                rep     stosw

                pop     di
                ret
`W?FillX$:CTextScreen$f(iiiii)v` endp

;void FillY(int X, int Y, int Ch, int Attr, int Count);
`W?FillY$:CTextScreen$f(iiiii)v` proc  c,
                @@this: dword, @@X: word, @@Y: word,
                @@Ch: word, @@Attr: word, @@Count: word

                mov     es,TextScreenSeg
                mov     bx,@@Y
                imul    bx,80
                add     bx,@@X
                shl     bx,1

                mov     ax,@@Ch
                or      ax,@@Attr
                mov     cx,@@Count
                inc     cx
                jmp     FYTestEnd

FYDrawChar:     mov     es:[bx],ax
                add     bx,160

FYTestEnd:      loop    FYDrawChar
                ret
`W?FillY$:CTextScreen$f(iiiii)v` endp

;void PutImage(int X, int Y, int Width, int Height, unsigned short *Image);
`W?PutImage$:CTextScreen$f(iiiipfus)v` proc c,
                @@this: dword, @@X: word, @@Y: word,
                @@Width: word, @@Height: word, @@Image: dword

                push    si
                push    di
                push    ds

                mov     es,TextScreenSeg
                mov     di,@@Y
                imul    di,80
                add     di,@@X
                shl     di,1
                lds     si,@@Image
                cld
                jmp     PITestEnd

PIDrawLine:     push    di
                mov     cx,@@Width
                rep     movsw
                pop     di
                add     di,160

PITestEnd:      dec     @@Height
                jns     PIDrawLine


                pop     ds
                pop     di
                pop     si
                ret
`W?PutImage$:CTextScreen$f(iiiipfus)v` endp


;void GetImage(int X, int Y, int Width, int Height, unsigned short *Image);
`W?GetImage$:CTextScreen$f(iiiipfus)v` proc c,
                @@this: dword, @@X: word, @@Y: word,
                @@Width: word, @@Height: word, @@Image: dword

                push    si
                push    di
                push    ds

                mov     ds,TextScreenSeg
                mov     si,@@Y
                imul    si,80
                add     si,@@X
                shl     si,1

                les     di,@@Image
                cld
                jmp     GITestEnd

GIDrawLine:     push    si
                mov     cx,@@Width
                rep     movsw
                pop     si
                add     si,160

GITestEnd:      dec     @@Height
                jns     GIDrawLine


                pop     ds
                pop     di
                pop     si
                ret
`W?GetImage$:CTextScreen$f(iiiipfus)v` endp
                end
