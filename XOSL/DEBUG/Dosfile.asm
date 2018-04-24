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
                public `W?DosCreate$f(pfxa)i`
                public `W?DosClose$f(i)v`
                public `W?DosWrite$f(ipfxvus)v`

;int DosCreate(const char *FileName)
`W?DosCreate$f(pfxa)i` proc   c,
                @@FileName: dword

                push    ds
                mov     ah,3ch
                xor     cx,cx
                lds     dx,@@FileName
                int     21h
                pop     ds
                ret
`W?DosCreate$f(pfxa)i` endp                

;void DosClose(int FileHandle)
`W?DosClose$f(i)v` proc    c,
                @@FileHandle: word

                mov     ah,3eh
                mov     bx,@@FileHandle
                int     21h
                ret
`W?DosClose$f(i)v` endp                

;void far DosWrite(int FileHandle, const void *Data, unsigned short Size)
`W?DosWrite$f(ipfxvus)v` proc  c,
                @@FileHandle: word, @@Data: dword,
                @@Size: word

                push    ds
                mov     ah,40h
                mov     bx,@@FileHandle
                mov     cx,@@Size
                lds     dx,@@Data
                int     21h
                pop     ds
                ret
`W?DosWrite$f(ipfxvus)v` endp                

                end

		.bss
		public	_BSS_END
LABEL _BSS_END		$
		end bss
