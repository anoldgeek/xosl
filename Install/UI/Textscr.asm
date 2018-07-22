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

                extrn   _MemCopy: far

                public  `W?$CT:CTextScreen$F(I)_`
                public  `W?$DT:CTextScreen$F()_`

                public  `W?PutStr$:CTextScreen$F(IIPFXAI)V`
                public  `W?PutChar$:CTextScreen$F(IIII)V`
                public  `W?FillX$:CTextScreen$F(IIIII)V`
                public  `W?FillY$:CTextScreen$F(IIIII)V`
                public  `W?PutImage$:CTextScreen$F(IIIIPFUS)V`
                public  `W?GetImage$:CTextScreen$F(IIIIPFUS)V`

;CTextScreen(int Attr)
`W?$ct:CTextScreen$F(I)_`   proc    syscall,
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
                call    _MemCopy
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
`W?$ct:CTextScreen$F(I)_` endp                

;~ClearScreen()
`W?$dt:CTextScreen$F()_`   proc    

                ;restore text screen
                push    160 * 25
                push    ds
                push    offset ScreenBuffer
                push    TextScreenPtr
                call    _MemCopy
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
`W?$dt:CTextScreen$F()_`  endp

;void CTextScreen::PutStr(int X, int Y, const char *Str, int Attr);
`W?PutStr$:CTextScreen$F(IIPFXAI)V` proc    syscall,
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
`W?PutStr$:CTextScreen$F(IIPFXAI)V` endp

;void CTextScreen::PutChar(int X, int Y, int Ch, int Attr);
`W?PutChar$:CTextScreen$F(IIII)V` proc syscall,
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
`W?PutChar$:CTextScreen$F(IIII)V` endp

;void CTextScreen::FillX(int X, int Y, int Ch, int Attr, int Count);
`W?FillX$:CTextScreen$F(IIIII)V` proc  syscall,
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
`W?FillX$:CTextScreen$F(IIIII)V` endp

;void FillY(int X, int Y, int Ch, int Attr, int Count);
`W?FillY$:CTextScreen$F(IIIII)V` proc  syscall,
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
`W?FillY$:CTextScreen$F(IIIII)V` endp

;void PutImage(int X, int Y, int Width, int Height, unsigned short *Image);
`W?PutImage$:CTextScreen$F(IIIIPFUS)V` proc syscall,
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
`W?PutImage$:CTextScreen$F(IIIIPFUS)V` endp


;void GetImage(int X, int Y, int Width, int Height, unsigned short *Image);
`W?GetImage$:CTextScreen$F(IIIIPFUS)V` proc syscall,
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
`W?GetImage$:CTextScreen$F(IIIIPFUS)V` endp
                end
