               .model  compact
                .386p
                .data
Digits          db      '0123456789abcdef'
                .code
                public  `W?GetDigit$F(I)A`
;`W?GetDigit$F(I)A`      proc    c,
; Watcom calling convention.
;	ax  dx	bx,cx
;                @@Digit: word
`W?GetDigit$F(I)A`      proc    
                mov     bx,offset Digits
;                add     bx,@@Digit
		add	bx,ax
                mov     al,[bx]

                ret
`W?GetDigit$F(I)A`       endp

                end
