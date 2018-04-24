                .model  compact
                .386p
                .code
;                public  _Execute
		public  `W?Execute$f(ususususus)v`

;void Execute(unsigned short StartSeg, unsigned short ReloSS,
;             unsigned short ExeSP, unsigned short ReloCS,
;             unsigned short ExeIP);

;_Execute        proc    c
`W?Execute$f(ususususus)v` proc c,
                @@StartSeg: word, @@ReloSS: word,
                @@ExeSP: word, @@ReloCS: word,
                @@ExeIP: word

                mov     ax,@@StartSeg
                sub     ax,0010h
                mov     ds,ax
                mov     es,ax

                mov     ax,@@ReloSS
                add     ax,@@StartSeg
                mov     bx,@@ExeSP
                mov     cx,@@ReloCS
                add     cx,@@StartSeg
                mov     dx,@@ExeIP
                mov     ss,ax
                mov     sp,bx
                push    cx
                push    dx
                retf
`W?Execute$f(ususususus)v`  endp
                
                end
