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
                public  `W?ComPort$fi`
`W?ComPort$fi`  dw      ?
ByteNo          dw      ?
MouseData       db      ?,?,?,?
MouseHandler    dd      ?
                .data
                public  `W?MouseInt$fi`
`W?MouseInt$fi` dw      0
                .code

                public  `W?ComDetect$f(i)i`
                public  `W?ComInit$f(ipf(sccs)v)v`
                public  `W?ComIRQMask$f(i)v`

;void ComDetect(int Port);
`W?ComDetect$f(i)i` proc    c,
                @@Port: word

                mov     dx,@@Port
                or      dx,dx
                jz      _52ed
                add     dx,2
                in      al,dx
                call    Timeout
                and     al,0f8h
                je      _52de
                and     al,0c0h
                je      _52ed
                xor     al,al
                out     dx,al
                call    Timeout
                in      al,dx
                call    Timeout
                and     al,0f8h
                jne     _52ed
                xor     al,al

_52de:          sub     dx,2
                call    TestAndSet
                mov     al,1
                jnb     _52ea
                xor     al,al
_52ea:          jmp     _52f4
_52ed:          mov     al,0ffh
_52f4:          xor     ah,ah
                movsx   ax,al
                ret
`W?ComDetect$f(i)i` endp

;void ComInit(int Port, void (*MouseHandler)());
;void __cdecl ComInit(int port, void far __cdecl (*MouseHandler)(short P0, signed char dY, signed char dX, short Status));  //ML
`W?ComInit$f(ipf(sccs)v)v` proc c,
                @@Port: word, @@MouseHandler: dword

                mov     eax,@@MouseHandler
                mov     MouseHandler,eax

                mov     dx,@@Port
                add     dx,3
                mov     al,80h
                out     dx,al
                jmp     SI1
SI1:            sub     dx,2
                mov     al,0    ;brd
                out     dx,al
                jmp     SI2
SI2:            dec     dx
                mov     al,60h  ;brd
                out     dx,al
                jmp     SI3
SI3:            add     dx,3
                mov     al,2
                out     dx,al
                jmp     SI4
SI4:            mov     dx,[bp + 6]
                add     dx,4
                mov     al,0bh
                out     dx,al
                jmp     SI5
SI5:            mov     dx,[bp + 6]
                add     dx,1
                mov     al,1
                out     dx,al
                jmp     SI6
SI6:            mov     dx,[bp + 6]
                add     dx,5
                in      al,dx
                jmp     SI7
SI7:            mov     dx,[bp + 6]
                in      al,dx
                ret
`W?ComInit$f(ipf(sccs)v)v` endp

TestAndSet      proc    near
                push    cx
                push    dx
                push    si
                push    ds
                mov     bx,dx
                add     dx,3
                mov     al,80h
                out     dx,al
                jmp     _5853
_5853:          sub     dx,2
                mov     al,0
                out     dx,al
                jmp     _585b
_585b:          dec     dx
                mov     al,60h
                out     dx,al
                jmp     _5861
_5861:          add     dx,3
                mov     al,6
                out     dx,al
                jmp     _5869
_5869:          sub     dx,2
                xor     al,al
                out     dx,al
                add     dx,3
                mov     al,0
                out     dx,al
                jmp     _5877
_5877:          mov     ax,40h
                mov     ds,ax
                cli
                mov     cl,ds:[006ch]
                add     cl,9
                sti
_5885:          cmp     cl,ds:[006ch]
                jne     _5885

                mov     al,1
                out     dx,al
                jmp     _5890
_5890:          mov     dx,bx
                cli
                mov     cl,ds:[006ch]
                add     cl,5
                sti
_589b:          in      al,dx
                cmp     cl,ds:[006ch]
                jne     _589b
                add     dx,4
                mov     al,3
                out     dx,al
                jmp     _58aa
_58aa:          mov     dx,bx
                mov     ah,ds:[006ch]
                add     ah,5
_58b3:          add     dx,5
                in      al,dx
                test    al,1
                je      _58db
                mov     dx,bx
                in      al,dx
                and     al,7fh
                cmp     al,4dh
                jne     _58c9
                mov     ah,2
                jmp     _58e4
_58c9:          cmp     al,6dh
                jne     _58d2
                mov     ah,3
                jmp     _58e4

_58d2:          cmp     al,48h
                jne     _58db
                mov     ah,3
                jmp     _58e4
_58db:          mov     dx,bx
                cmp     ah,ds:[006ch]
                jne     _58b3
                stc
_58e4:          pop     ds
                pop     si
                pop     dx
                pop     cx
                ret
TestAndSet      endp

Timeout         proc    near
                jmp     TO1
TO1:            jmp     TO2
TO2:            ret
Timeout         endp

;void com_irqmask(int enable);
`W?ComIRQMask$f(i)v` proc    c,
                @@Enable: word

                in      al,21h
                cmp     byte ptr @@Enable,0
                je      CIMBlock
                and     al,0e7h
                jmp     CIMSetMask
CIMBlock:       or      al,18h
CIMSetMask:     out     21h,al
                ret
`W?ComIRQMask$f(i)v` endp

;void interupt com_handler(void);
                public  `W?ComHandler$f()v`
`W?ComHandler$f()v` proc
                cli
                pushad
                push    ds
		push	es
                mov     ax,DGROUP
                mov     ds,ax
                mov     dx,`W?ComPort$fi`

                in      al,dx
                test    al,40h
                jz      CHBorder
                mov     ByteNo,0

CHBorder:       mov     bx,ByteNo
                and     bx,3
                mov     [bx + MouseData],al
                inc     ByteNo
                cmp     bx,2
                jne     CHDone
                call    COMNewData                

CHDone:         
                sti
                mov     al,20h
                out     20h,al
                pop     es
		pop	ds
                popad
                iret
`W?ComHandler$f()v` endp

COMNewData      proc    near
                xor     ax,ax

                mov     bl,MouseData[0]
                mov     al,bl
                and     al,20h
                shr     al,5
                and     bl,10h
                shr     bl,3
                or      al,bl
                push    ax

                mov     al,MouseData[1]
                mov     bl,MouseData[0]
                and     bl,3
                shl     bl,6
                or      al,bl
                push    ax

                mov     al,MouseData[2]
                mov     bl,MouseData[0]
                and     bl,0ch
                shl     bl,4
                or      al,bl
                neg     al
                push    ax

                push    0
                call    MouseHandler
                add     sp,8
                ret
COMNewData      endp

                end
