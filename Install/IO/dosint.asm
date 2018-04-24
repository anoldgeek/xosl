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

                .model  compact
                .386p
		.data
ExecParamBlock	dw	0
ExecProgArgsPtr	dd	0
		dd	0
		dd	0
		dd	0
		dd	0


                .code
                public  `W?Create$:CDosFile$n(pfxa)i`
                public  `W?Open$:CDosFile$n(pfxa$TFileAccess$:1$)i`
                public  `W?Close$:CDosFile$n(i)v`
                public  `W?Read$:CDosFile$n(ipfvus)us`
                public  `W?Write$:CDosFile$n(ipfxvus)us`
                public  `W?SetAttrib$:CDosFile$n(pfxai)i`
                public  `W?Unlink$:CDosFile$n(pfxa)i`
                public  `W?LSeek$:CDosFile$n(il$TWhence$:1$)l`

;CDosFile::Create(const char *FileName);
`W?Create$:CDosFile$n(pfxa)i`   proc c,
                @@FileName: dword

                push    ds
                mov     ah,3ch
                xor     cx,cx
                lds     dx,@@FileName
                int     21h
                jnc     CreatOk
                mov     ax,-1
CreatOk:        pop     ds
                ret
`W?Create$:CDosFile$n(pfxa)i`  endp

;int CDosFile::Open(const char *FileName, TFileAccess Access);
`W?Open$:CDosFile$n(pfxa$TFileAccess$:1$)i` proc c,
                @@FileName: dword, @@Access: word

                push    ds
                mov     ah,3dh
                mov     al,byte ptr @@Access
                lds     dx,@@FileName
                xor     cx,cx
                int     21h
                jnc     OpenOk
                mov     ax,-1
OpenOk:         pop     ds
                ret
`W?Open$:CDosFile$n(pfxa$TFileAccess$:1$)i` endp

;void CDosFile::Close(int Handle);
`W?Close$:CDosFile$n(i)v`      proc c,
                @@Handle: word

                mov     ah,3eh
                mov     bx,@@Handle
                int     21h
                ret
`W?Close$:CDosFile$n(i)v` endp

;unsigned short CDosFile::Read(int Handle, void *Buffer, unsigned short Length)
`W?Read$:CDosFile$n(ipfvus)us`   proc c,
                @@Handle: word, @@Buffer: dword, @@Length: word

                push    ds
                mov     ah,3fh
                mov     bx,@@Handle
                mov     cx,@@Length
                lds     dx,@@Buffer
                int     21h
                jnc     ReadOK
                xor     ax,ax
ReadOk:         pop     ds
                ret
`W?Read$:CDosFile$n(ipfvus)us`  endp

;unsigned short CDosFile::Write(int Handle, void *Buffer, unsigned short Len);
`W?Write$:CDosFile$n(ipfxvus)us` proc c,
                @@Handle: word, @@Buffer: dword, @@Length: word

		push    ds
                mov     ah,40h
                mov     bx,@@Handle
                mov     cx,@@Length
                lds     dx,@@Buffer
                int     21h
                jnc     WriteOK

;GetExtended:	push	ax bx cx dx
;		push	di si es ds
;		mov	bx,0
;		mov	ax,59h
;		int	21h
;		mov	_error_code,ax
;		mov	_error_class,bh
;		mov	_action,bl
;		mov	_locus,ch
;		pop	ds es si di
;		pop	dx cx bx ax
;		mov	_error_code,ax

		xor	ax,ax
		
WriteOk:        pop     ds
                ret
`W?Write$:CDosFile$n(ipfxvus)us` endp

;int CDosFile::SetAttrib(const char *FileName, int Attributes)
`W?SetAttrib$:CDosFile$n(pfxai)i` proc c,
                @@FileName: dword, @@Attributes: word

                push    ds
                mov     ax,4301h
                mov     cx,@@Attributes
                lds     dx,@@FileName
                int     21h
                sbb     ax,ax
                pop     ds
                ret
`W?SetAttrib$:CDosFile$n(pfxai)i` endp

;int CDosFile::Unlink(const char *FileName);
`W?Unlink$:CDosFile$n(pfxa)i`   proc c,
                @@FileName: dword

                push    ds
                mov     ah,41h
                lds     dx,@@FileName
                xor     cx,cx
                int     21h
                sbb     ax,ax
                pop     ds
                ret
`W?Unlink$:CDosFile$n(pfxa)i` endp

;long CDosFile::LSeek(int Handle, long Offset, TWhence Whence)
`W?LSeek$:CDosFile$n(il$TWhence$:1$)l` proc c,
                @@Handle: word, @@OffsetLow: word, 
                @@OffsetHigh: word, @@Whence: word 

                mov     ah,42h
                mov     al,byte ptr @@Whence
                mov     bx,@@Handle
                mov     cx,@@OffsetHigh
                mov     dx,@@OffsetLow
                int     21h
                jnc     LSeekDone
                mov     ax,-1
                mov     dx,ax

LSeekDone:      ret
`W?LSeek$:CDosFile$n(il$TWhence$:1$)l` endp
		end
		       
