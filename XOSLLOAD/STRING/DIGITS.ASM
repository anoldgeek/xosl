                .model  compact
                .386p
                .data
Digits          db      '0123456789abcdef'
                .code
                public  `W?GetDigit$N(I)A`
;                @@Digit: word
; Watcom calling convention.
;	ax		dx		bx		cx
;	@@Digit
`W?GetDigit$N(I)A`      proc    
		push	bx
                mov     bx,offset Digits
;                add     bx,@@Digit
		add	bx,ax
                mov     al,[bx]
		pop	bx
                ret
`W?GetDigit$N(I)A` endp

                end
