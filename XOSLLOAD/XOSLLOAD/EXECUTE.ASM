                .model  compact
                .386p
                .code
                public  _Execute

;void Execute(unsigned short StartSeg, unsigned short ReloSS,
;             unsigned short ExeSP, unsigned short ReloCS,
;             unsigned short ExeIP);

;void Execute(unsigned short StartSeg, unsigned short ReloSS,
;             unsigned short ExeSP, unsigned short ReloCS,
;             unsigned short ExeIP,  
;             unsigned short FreeStartSeg, unsigned short EndSeg);

_Execute        proc    c
                arg     @@StartSeg: word, @@ReloSS: word
                arg     @@ExeSP: word, @@ReloCS: word
                arg     @@ExeIP: word
		arg	@@FreeStartSeg: word, @@EndSeg: word

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

		; push freestart * length
		mov	ax,@@FreeStartSeg
		push	ax
		mov	ax,@@EndSeg
		push	ax
		call _Excute2
		pop	dx
		pop	dx
		retf

_Excute2:
                push    cx
                push    dx
                retf
_Execute        endp
                
                end
