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
; - Modify LockScreen to make sure LockedData is addressed correctly
; - Modify UnlockScreen to make sure LockedData is addressed correctly
;

                .model  large
                .386p
		.data?
LockedData	db	4000 dup (?)		
                .code
                public  `W?ClearScreen$f()v`
                public  `W?LockScreen$f()v`
                public  `W?UnlockScreen$f()v`

;void ClearScreen();
`W?ClearScreen$f()v` proc
		push	edi
		
		mov	ax,0b800h
		mov	es,ax
		xor	edi,edi
		mov	eax,1f201f20h
		mov	ecx,1000
		cli
		rep	stosd
		
		;set cursor pos to (0,0)
                mov     ah,2
		xor	bh,bh
                xor     dx,dx
                int     10h

		pop	edi
		ret
`W?ClearScreen$f()v`	endp


`W?LockScreen$f()v`  proc
		push	edi
		push	esi
		push	ds

		;mov	ax,ds			;ML ds != seg LockedData
		mov	ax,seg LockedData	;ML make sure es = seg LockedData
		mov	es,ax
		mov	ax,0b800h
		mov	ds,ax
		
		xor	esi,esi
		mov	edi,offset LockedData
		
		mov	ecx,1000
		cli
		rep	movsd
		

		pop	ds
		pop	esi
		pop	edi
		ret
`W?LockScreen$f()v` 	endp

`W?UnlockScreen$f()v` proc
		push	edi
		push	esi
		push	ds			;ML save ds
		
		mov	ax,0b800h
		mov	es,ax

		mov	ax,seg LockedData	;ML needed because ds != seg LockedData
		mov	ds,ax			;ML make sure ds = seg LockedData

		mov	esi,offset LockedData
		xor	edi,edi
		
		mov	ecx,1000
		cli
		rep	movsd
		
		pop	ds			;ML restore ds
		pop	esi
		pop	edi
		
		ret
`W?UnlockScreen$f()v`		endp


                end
