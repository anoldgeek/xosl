                .model  compact
                .386p
                .data
Digits          db      '0123456789abcdef'
                .code
                public  GetDigit_
GetDigit_       proc    c,
                @@Digit: word

                mov     bx,offset Digits
                add     bx,@@Digit
                mov     al,[bx]

                ret
GetDigit_       endp

                end
