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

                public  `W?gotoxy$F(II)V`
                public  `W?wherexy$F(PFIPFI)V`
                public  `W?puts$F(PFXA)V`
                public  `W?putch$F(I)V`

;void gotoxy(int x, int y)
;`W?gotoxy$F(II)V` proc c,
; Watcom calling convention.
;	ax  dx	bx,cx
;                @@x: word, @@y: word
`W?gotoxy$F(II)V` proc 
;                mov     dl,byte ptr @@x
;                mov     dh,byte ptr @@y
		mov	dh,dl
		mov	dl,al
                mov     ah,2
                xor     bh,bh
                int     10h
                ret
`W?gotoxy$F(II)V` endp

;void wherexy(int *x, int *y)
;`W?wherexy$F(PFIPFI)V` proc    c,
; Watcom calling convention.
;	ax  dx	bx,cx
;                @@x:  dword, @@y: dword
`W?wherexy$F(PFIPFI)V` proc  
		push	es
		push	bx
		push	cx
		push	ax
		push	dx
                mov     ah,3
                xor     bh,bh
                int     10h
;                les     bx,@@x
		pop	es	; pushed dx
		pop	bx	; pushed ax

                movzx   ax,dl
                mov     es:[bx],ax
;                les     bx,@@y
		pop	es	; pushed cx
		pop	bx	; pushed bx
                mov     al,dh
                mov     es:[bx],ax
		pop	es
                ret
`W?wherexy$F(PFIPFI)V` endp

;void puts(const char *str);
;`W?puts$F(PFXA)V` proc    c,
; Watcom calling convention.
;	ax  dx	bx,cx
;                @@str: dword
`W?puts$F(PFXA)V` proc    

                push    si
;                les     si,@@str
		mov	es,dx
		mov	si,ax
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
`W?puts$F(PFXA)V` endp

;void putch(int ch);
;`W?putch$F(I)V` proc    c
; Watcom calling convention.
;	ax  dx	bx,cx
;                @@ch: word
`W?putch$F(I)V` proc    

                mov     ah,0eh
                ;mov     al,byte ptr @@ch ; already loaded
                mov     bx,7
                int     10h
                ret
`W?putch$F(I)V` endp

                end
