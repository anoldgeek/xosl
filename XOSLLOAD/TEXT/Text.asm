;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;
;PutS( char const far * )
                .model  compact
                .386p
                .code
                public  `W?PutS$f(pfxa)v`, `W?GetCh$f()i`, `W?PutCh$f(i)v`

;void PutS(char *Str);
`W?PutS$f(pfxa)v` proc    
                push    bp
                mov     bp,sp
                push    si
                les     si,[bp + 4]
                cld
                jmp     PSLods

Teletype:       mov     ah,0eh
                mov     bx,7
                int     10h
PSLods:         lods    es:byte ptr [si]
                or      al,al
                jnz     Teletype
                pop     si
                pop     bp
                ret
`W?PutS$f(pfxa)v` endp

;void PutCh(int Ch);
`W?PutCh$f(i)v`   proc    c,
                @@ch: word

                mov     ah,0eh
                mov     al,byte ptr @@ch
                mov     bx,7
                int     10h
                ret
`W?PutCh$f(i)v` endp



;int GetCh(void);
`W?GetCh$f()i` proc
                xor     ah,ah
                int     16h
                ret
`W?GetCh$f()i` endp

                end
