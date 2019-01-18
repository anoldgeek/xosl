; Open Watcom Migration
; Copyright (c) 2090 by Norman Back:
; - Adapt to Open Watcom (version 2.0) WASM syntax
; - Use Open Watcom Name Mangling
; - Adapt to utilise GPT disks
;
; Avoid use of Open Watcom libs

                .model  large
                .386p
                .code

		public __U8RS
__U8RS		proc    far
		test	si,si
		je	L1
L2:		shr	ax,1
		rcr	bx,1
		rcr	cx,1
		rcr	dx,1
		dec	si
		jne	L2
L1:		ret

__U8RS		endp

		end
