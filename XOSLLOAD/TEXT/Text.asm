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
; Copyright (c) 2018 by Norman Back:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
;
;
;PutS( char const far * )
                .model  compact
                .386p
                .code
                public  `W?PutS$n(pfxa)v`, `W?GetCh$n()i`, `W?PutCh$n(i)v`

;void PutS(char *Str);
;		@@str: dword
; Watcom calling convention.
;	ax,dx	bx,cx
;	@@str
`W?PutS$n(pfxa)v` proc c
                push    si
		push	es
		push	bx
 ;               les     si,@@str
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
		pop	bx
		pop	es
		pop	si
                ret
`W?PutS$n(pfxa)v` endp

;void PutCh(int Ch);
;                @@ch: word
; Watcom calling convention.
;	ax  dx	bx,cx
;	@@ch
`W?PutCh$n(i)v`   proc    c
                mov     ah,0eh
;                mov     al,byte ptr @@ch	; Already loaded
                mov     bx,7
                int     10h
                ret
`W?PutCh$n(i)v` endp



;int GetCh(void);
`W?GetCh$n()i` proc c
                xor     ah,ah
                int     16h
                ret
`W?GetCh$n()i` endp

                end
