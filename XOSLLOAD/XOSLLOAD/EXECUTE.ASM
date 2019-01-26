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
; Copyright (c) 2018 by Norman Back:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Use Open Watcom Name Mangling

		.model  compact
                .386p
                .code
;                public  _Execute
		public  `W?Execute$N(USUSUSUSUSPFV)V`

;void Execute(unsigned short StartSeg, unsigned short ReloSS,
;             unsigned short ExeSP, unsigned short ReloCS,
;             unsigned short ExeIP, void *BrecAddr);

;_Execute        proc    c
;                @@StartSeg: word, @@ReloSS: word,
;                @@ExeSP: word, @@ReloCS: word,
;                @@ExeIP: word
; Watcom calling convention.
;	ax		dx		bx		cx
;	@@StartSeg	@@ReloSS	@@ExeSP		@@ReloCS
; No need to preserve registers as this does not return
`W?Execute$N(USUSUSUSUSPFV)V` proc syscall,
                @@ExeIP: word,
                @@BrecSeg: dword
;                mov     ax,@@StartSeg	; Already done
		push	ax	; @@StartSeg

		; point ds & es to PSP
		sub     ax,0010h
                mov     ds,ax
                mov     es,ax

;                mov     ax,@@ReloSS
		pop	ax	; @@StartSeg
		push	ax	; @@StartSeg

;                add     ax,@@StartSeg
		add	ax,dx

;                mov     bx,@@ExeSP	; Already loaded
;                mov     cx,@@ReloCS	; Already loaded
		pop	dx	; @@StartSeg

;                add     cx,@@StartSeg
		add	cx,dx

		mov	esi,@@BrecSeg	; save in esi before moving stack!
                mov     dx,@@ExeIP	; save in dx before moving stack!
		; move the stack!
                mov     ss,ax
                mov     sp,bx
                push    cx
                push    dx
		push	esi
		pop	ax
		pop	dx
                retf	2
`W?Execute$N(USUSUSUSUSPFV)V`  endp
                
                end
