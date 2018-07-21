                .model  compact
                .386p
                .data
Digits          db      '0123456789abcdef'
                .code
                public  GetDigit_
;                @@Digit: word
; Watcom calling convention.
;	ax		dx		bx		cx
;	@@Digit
GetDigit_       proc    c
		push	bx
                mov     bx,offset Digits
;                add     bx,@@Digit
		add	bx,ax
                mov     al,[bx]
		pop	bx
                ret
GetDigit_       endp

                end
