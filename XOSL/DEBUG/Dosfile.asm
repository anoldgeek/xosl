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
                public `W?DosCreate$F(PFXA)I`
                public `W?DosClose$F(I)V`
                public `W?DosWrite$F(IPFXVUS)V`

;int DosCreate(const char *FileName)
`W?DosCreate$F(PFXA)I` proc   syscall,
                @@FileName: dword

                push    ds
                mov     ah,3ch
                xor     cx,cx
                lds     dx,@@FileName
                int     21h
                pop     ds
                ret
`W?DosCreate$F(PFXA)I` endp                

;void DosClose(int FileHandle)
`W?DosClose$F(I)V` proc    syscall,
                @@FileHandle: word

                mov     ah,3eh
                mov     bx,@@FileHandle
                int     21h
                ret
`W?DosClose$F(I)V` endp                

;void far DosWrite(int FileHandle, const void *Data, unsigned short Size)
`W?DosWrite$F(IPFXVUS)V` proc  syscall,
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
`W?DosWrite$F(IPFXVUS)V` endp                

                end

;		.bss
;		public	_BSS_END
;LABEL _BSS_END		$
;		end bss
;
;		end
