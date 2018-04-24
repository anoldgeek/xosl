;
; Open Watcom Migration
; Copyright (c) 2010 by Mario Looijkens:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling
;

		.model  large
		.386p
                .code

public          `W?CallBootExtension$:CCdRom$f(uliusus)v`
public          `W?GetExtDriveParams$:CCdRom$f(irf$CExtDriveParams$$)i`


;void CallBootExtension(unsigned long Address, int Func, unsigned short BX, unsigned short CX);
`W?CallBootExtension$:CCdRom$f(uliusus)v` proc c,
                @@this: dword,
                @@Address: dword, @@Func: word,
                @@BX: word, @@CX: word

		mov	ah,byte ptr @@Func
		mov	bx,@@BX
		mov	cx,@@CX
		call	@@Address
		ret
`W?CallBootExtension$:CCdRom$f(uliusus)v` endp                
		

;int CCdRom::GetExtDriveParams(int Drive, CExtDriveParams &Params)
`W?GetExtDriveParams$:CCdRom$f(irf$CExtDriveParams$$)i` proc c,
                @@this: dword,
                @@Drive: word,
                @@ParamBuffer: dword
	
		push	ds
		push	si
		
		mov	ah,48h
		mov	dl,byte ptr @@Drive
		lds	si,@@ParamBuffer
		int	13h
		mov	ax,-1
		jc	GetExtExit
		inc	ax
		
GetExtExit:	pop	si
		pop	ds
		ret
`W?GetExtDriveParams$:CCdRom$f(irf$CExtDriveParams$$)i` endp                
	
		end

