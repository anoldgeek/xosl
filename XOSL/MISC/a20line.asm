;
; Extended Operating System Loader (XOSL)
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;
;
; EnableA20/DisableA20 code is take from Ranish Partition Manager 2.44
; EXT32A20.INC
;
;_TEXT16       segment  ;ML
;
; This file provides routines for Enabling or Disabling A20 line.
; It uses two metods:
;
;      - INT 15h AH=24h or Port 0092h for PS/2 or
;      - Keyboard Controller (KBC) A20 support for older systems
;
; Even though computer manufecturers tried to be compartible with KBC way
; of controlling A20 on the newer models (especially laptops) KBC A20 support
; is dropped or incomplete.
;
; Open Watcom Migration
; Copyright (c) 2010 by Mario Looijkens:
; - Adapt to Open Watcom (version 1.8) WASM syntax
; - Rename labels to get rid of Error! E600: 
;   '@@error' is already defined
;      '@@clear' is already defined
;   '@@again' is already defined
; - Add A20 Enable/Disable diagnostics 

                .model  compact
                .386p
		.code
		
		public	_EnableA20
		public	_DisableA20
		

is_a20_on	proc	near
		;
		; This function will test if A20 line is enabled.
		; It will return ZF=1 if disabled (jz=true) 
		;	      or ZF=0 if enabled (jnz=true)
		;
		push	ds
		push	es
		push	ax
		push	bx
		xor	ax, ax
		mov	bx, 472h
		mov	ds, ax
		dec	ax
		mov	es, ax
		push	word ptr [bx]
		xor	ax, es:[bx+10h]
		mov	word ptr [bx], ax
		cmp	ax, es:[bx+10h]
		pop	word ptr [bx]
		pop	bx
		pop	ax
		pop	es
		pop	ds
		ret

is_a20_on	endp


;dx used for diagnostics
;10h: A20 already enabled
;20h: A20 enabled using int15, ah=2401h
;30h: A20 enabled using port 92h
;40h: A20 enabled using KBC
;50h: KBC error
_EnableA20 proc far

		call	is_a20_on
		mov dx, 10h
		jnz	@@enabled	; line is already enabled
		
		mov	ax, 2401h	; Enable A20 gate
		int	15h
		call	is_a20_on
		mov dx, 20h
		jnz	@@enabled	; line is already enabled
		
		cli
		in	al, 92h		; PS/2 system control port A
		or	al, 2
		out	92h, al
		sti

		call	is_a20_on
		mov dx, 30h
		jnz	@@enabled	; line is already enabled
	
		call	kbc_wait_write
		mov dx, 50h
		;jc	@@error     ;Wasm Error! E600
		jc	@@error_enA20

		cli			; Only need for reading value from KBC
		mov	al, 0D0h
		out	64h, al
		call	kbc_wait_read
		jc	@@enable_int

		in	al, 60h		; Get current status
		mov	ah, al
		call	kbc_wait_write
@@enable_int:	sti
		mov dx, 50h
		;jc @@error     ;Wasm Error! E600
		jc @@error_enA20

		mov	al, 0D1h	; Tell the KBC we want to write to the
		out	64h, al		; Out Put Port next
		call	kbc_wait_write
		mov dx, 50h
		;jc	@@error     ;Wasm Error! E600
		jc	@@error_enA20

		mov	al, ah
		or	al, 2			; Enable line A20
		out	60h, al
		call	kbc_wait_write
		mov dx, 50h
		;jc	@@error     ;Wasm Error! E600
		jc	@@error_enA20

		mov	al, 0FFh		; NOP
		out	64h, al
		call	kbc_wait_write

		call	is_a20_on
		mov dx, 40h
		jnz	@@enabled	; line is already enabled
		mov dx, 50h

;@@error:	stc     ;Wasm Error! E600
@@error_enA20:	stc
		ret


@@enabled:	clc
		ret
		
_EnableA20	endp

;dx used for diagnostics
;01h: A20 already disabled
;02h: A20 disabled using int15, ah=2400h
;03h: A20 disabled using port 92h
;04h: A20 disabled using KBC
;05h: KBC error
_DisableA20 proc far
    ;push dx
		xor dx, dx
		
		call	is_a20_on
		mov dx,01h
		jz	@@disabled	; line is already disabled
		
		mov	ax, 2403h	; QUERY A20 GATE SUPPORT
		int	15h
		call	is_a20_on
		jz	@@disabled	; line is already disabled

		;mov	ax, 2402h	; Disable A20 gate
		mov	ax, 2402h	; GET A20 GATE STATUS
		int	15h		
		call	is_a20_on
		jz	@@disabled	; line is already disabled

		mov	ax, 2400h	; DISABLE A20 GATE
		int	15h
		call	is_a20_on
		mov dx,02h
		jz	@@disabled	; line is already disabled

		mov	ax, 2402h	; GET A20 GATE STATUS
		int	15h
		call	is_a20_on
		jz	@@disabled	; line is already disabled
		
		cli
		in	al, 92h		; PS/2 system control port A
		and	al, 0FDh	; ~2 - clear bit 1
		out	92h, al
		in	al, 92h		; PS/2 system control port A  ;ML out 92h has no effect?
		sti

		call	is_a20_on
		mov dx,03h
		jz	@@disabled	; line is already disabled

		call	kbc_wait_write
		jc	@@error

		cli			; Only need for reading value from KBC
		
		mov     al,0ADh  ; disable keyboard
		out     064h,al
		call	kbc_wait_write

		mov	al, 0D0h
		out	64h, al
		call	kbc_wait_read
		jc	@@disable_int

		in	al, 60h
		mov	ah, al
		call	kbc_wait_write
@@disable_int:	sti
		mov dx,05h
		jc	@@error

		mov	al, 0D1h
		out	64h, al
		call	kbc_wait_write
		mov dx,05h
		jc	@@error

		mov	al, ah
		and	al, 0FDh		; Disable line A20
		out	60h, al
		call	kbc_wait_write
		mov dx,05h
		jc	@@error

		mov	al, 0FFh
		out	64h, al
		call	kbc_wait_write
		
		mov     al,0AEh  ; enable keyboard
		out     064h,al
		call	kbc_wait_write		

		call	is_a20_on
		mov dx, 04h
		jz	@@disabled	; line is already disabled
		mov dx,05h

@@error:	stc
		ret


@@disabled:	clc
		ret
		
		
_DisableA20	endp


kbc_wait_write	proc	near

		; Before we write to KBC we have to wait until KBC input
		; register is empty or a reasonable amount of time has gone.
		
		xor	cx, cx		; we will retry up to 64k times
;@@again:	     ;Wasm Error! E600
@@again_wait_write:	
		in	al, 64h		; get KBC read status
		test	al, 02		; if bit 1 is clear the buffer is empty
		;jz	@@clear     ;Wasm Error! E600
		jz	@@clear_wait_write
		inc	cx
		;jnc	@@again     ;Wasm Error! E600
		jnc	@@again_wait_write
		ret
;@@clear:     ;Wasm Error! E600
@@clear_wait_write:
 		clc	; CF is clear - we may write to KBC ioput register
 		ret

kbc_wait_write	endp



kbc_wait_read	proc	near

		; Before we read from KBC we have to wait until KBC output
		; register is full or a reasonable amount of time has gone.

		xor	cx, cx		; we will retry up to 64k times
@@again:	
		in	al, 64h		; get KBC read status
		test	al, 01		; if bit 1 is set the buffer
		jnz	@@clear		; has some data for us to read
		inc	cx
		jnc	@@again
		ret
@@clear:
 		clc	; CF is clear - we may read from KBC output register
 		ret

kbc_wait_read	endp


COMMENT %
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
enableA20:
        mov gate,2                      ; 2 -> Enable A20
        call gateA20
        jc short @@notenabledA20
        ret
@@notenabledA20:
        mov dx,offset errmsg3
        mov ds:v86r_ax,0ffh
        jmp exit16err
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
disableA20:
        mov gate,0                      ; 0 -> Disable A20
        call gateA20
        jc short @@notdisabledA20
        ret
@@notdisabledA20:
        mov dx,offset errmsg4
        mov ds:v86r_ax,0ffh
        jmp exit16err
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
gateA20:                                ; Hardware gate A20
        xor ax,ax                       ; Set segments to 0 and ffffh for
        mov fs,ax                       ;  testA20
        dec ax
        mov gs,ax

        call testA20                    ; Is A20 enabled or disabled?
        call isA20ok                    ; Is that right?
        jnc @@gateA20done               ; If yes, done

        in al,92h                       ; PS/2 A20 gate
        and al,0fdh                     ; Clear bit 1 (=gate A20)
        or al,gate                      ; Set A20 bit or leave it 0
        out 92h,al                      ; 2 -> Enable A20; 0 -> Disable A20

        call testA20                    ; Is A20 enabled or disabled?
        call isA20ok                    ; Is that right?
        jnc short @@gateA20done         ; If yes, done

                                        ; A20 gate via 8042 (=keyb controller)
        call empty_8042                 ; Ensure 8042 input buf is empty prior
        jnz short @@gateA20fail         ;  to writing to the input registers
        mov al,0adh                     ; adh -> Disable keyboard interface
        out 64h,al                      ; 64h = 8042 command register

        call empty_8042
        jnz short @@gateA20fail
        mov al,0d0h                     ; d0h -> Read 8042 output port; value
        out 64h,al                      ;  in output port is placed in output
                                        ;  register
        xor cx,cx
@@wait8042:
        in al,64h                       ; Read 8042 status register
        test al,1                       ; Output reg (60h) has data for sys?
        loopz @@wait8042                ; If not (yet) -> Loop
        jz short @@gateA20fail

        in al,60h                       ; Data in 8042 output reg -> Read it
        push ax                         ; Save contents of 8042 output port

        mov al,0d1h                     ; d1h -> Write to output port of 8042
        out 64h,al                      ;  (through port 60h)
                                  
        call empty_8042
        jnz short @@gateA20fail
        pop ax
        and al,0fdh                     ; Clear bit 1 (=gate A20)
        or al,gate                      ; Set A20 bit or leave it 0
        out 60h,al                      ; 60h = 8042 output port (now)

        call empty_8042
        jnz short @@gateA20fail
        mov al,0aeh                     ; aeh -> Enable keyboard interface
        out 64h,al

                                        ; Wait for 8042 to gate A20
        mov al,36h                      ; Select counter 0; write LSB, MSB;
        out 43h,al                      ;  cntr mode 3; 16-bit binary counter
        mov al,0                        ; Set counter value to 65536; no check
        out 40h,al                      ;  is necessary if wrap takes place ->
        out 40h,al                      ;  16-bit register wrap does the job
                                        ; 65536 is default counter value too
        mov cx,30                       ; Wait maximum 30 ticks:
@@anothertick:                          ;  30 * 1/(1193181 ticks/s) χ 25 ζs
        mov al,0d2h                     ; Read back command; bit 5=0 -> Latch
        out 43h,al                      ;  counter; bit 4!=0 -> Don't latch
        in al,40h                       ;  status; bit 1=1 -> Select cntr 0;
        mov dl,al                       ;  bit 0 reserved. Writing 0 instead
        in al,40h                       ;  of read back command also works.
        mov dh,al
@@nexttick:                             ; Wait at least 1 PIT clock tick
        mov al,0d2h
        out 43h,al
        in al,40h                       ; Get LSB of current tick counter
        mov ah,al
        in al,40h                       ; Get MSB of current tick counter
        xchg ah,al                      ; If ax is greater than dx (instead
        cmp ax,dx                       ;  of the opposite), dx will still
        je short @@nexttick             ;  contain the right value because of
        sub dx,ax                       ;  16-bit reg wrap (i.e. 1-ffff=2)
        sub cx,dx                       ; Substract the number of passed
        jnc short @@anothertick         ;  clock ticks between 2 port reads

        call testA20                    ; Is A20 enabled or disabled?
        call isA20ok                    ; Is that right?
        jnc short @@gateA20done

@@gateA20fail:
        stc                             ; Error -> Carry Flag set
@@gateA20done:
        ret
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
testA20:
        mov al,fs:[0]                   ; Get byte from 0:0
        mov ah,al                       ; Save byte
        not al                          ; Modify byte
        xchg al,gs:[10h]                ; If A20 is off -> ffffh:10h = 0:0
        cmp ah,fs:[0]                   ; 0:0 has changed only if A20 is off
        mov gs:[10h],al                 ; Restore byte at ffffh:10h
        ret                             ; Zero Flag is set if A20 is enabled
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
isA20ok:                                ; Carry Flag set = A20 is not ok
        pushf                           ; Save result, zero -> A20 is on
        test gate,2                     ; Must A20 be enabled or disabled?
        jz short @@disableA20           ; If gate != 2 -> Must be disabled
        popf
        clc                             ; Ensure CF set only when error
        jz short @@ok                   ; Must be on, it is on -> A20 is ok
        jmp short @@notok
@@disableA20:
        popf
        clc
        jnz short @@ok                  ; Must be off, it is off -> A20 is ok
        jmp short @@notok
@@notok:
        stc                             ; Set Carry Flag
@@ok:
        ret                             ; Leave Carry Flag cleared
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
empty_8042:                             ; Wait for safe to write to 8042
        xor cx,cx                       ;  (= keyboard controller)
@@try_8042:
        in al,64h                       ; Read 8042 status register
        test al,2                       ; Input reg (60/64) has data for 8042?
        loopnz @@try_8042               ; If yes -> Loop
        ret
;μμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμμ
%
COMMENT %
;															;
;	Author: Zaro
;	Date:	10/28/1999
;
;	Description:
;		This program handles A20 line.
;		It uses PS/2 procedure (port 92h) and after that
;		tries standard AT procedures (Keyboard Controller).
;
;	Usage:
;		a20  - alternate mode
;		a20 /0 or a20 -0 - OFF mode
;		a20 /1 or a20 -1 - ON mode
;
;	Acknowledgments:
;		Algorithm for KBC handling is borrowed from
;		Pascal Van Leeuwen and Galkowski Adam
;

; produce code for COM or EXE file
;EXE EQU 1

.MODEL SMALL
.DATA
.386

ASSUME CS:_DATA, DS:_DATA
PUBLIC	A20

IFDEF EXE
STACK	100H				;for EXE file
ELSE
	ORG	100H				;for COM file
ENDIF


A20		proc	far	
		push	cs
		push	ds
		pop		es				
		pop		ds
		mov		di,80h			;command line param
		mov		al,es:[di]
		or		al,al
		jz		@@alt			;if not - Toggle mode
		xor		ah,ah			;clear AH
		add		di,ax
		mov		al,es:[di]		;command line parameter
		lea		dx,msgMOFF
		lea		di,A20OFF
		sub		al,30h			;check for '0"
		je		@@go			;turn A20 off
		lea		dx,msgMON
		lea		di,A20ON
		sub		al,1
		je		@@go			;turn A20 on
		lea		dx,msgErr
		jmp	short @@disp

@@alt:	lea		dx,msgMAlt
		call	ISA20ON
		lea		di,A20OFF		;A20 is 1, turn to 0
		jnz		@@go
		lea		di,A20ON		;A20 is 0, turn to 1
@@go:	mov		ah,09h
		int		21h
		call	di

;display A20 status and exit
		call	ISA20ON
		lea		dx,msgON
		jnz		@@disp
		lea		dx,msgOFF
@@disp:	mov		ah,09h
		int		21h

		mov		ax,4c00h	
		int		21h
A20		endp

;set A20 on,return A20 status in AX
	
A20ON	proc	near
		call	ISA20ON
		jnz		@@ex
;try PS/2 way
		cli
		in      al,92h
		or      al,2
		out     92h,al
		sti
		call	DLY50				;50 ms Delay
		call	ISA20ON
		jnz		@@ex
; try with KBC way
		cli 
		mov		al,0D0h
		out		64H,al
@@LP1:	in		al,64h
		ror		al,1
		jnc		@@LP1
		in		al,60h
		mov		ah,al
		mov		al,0D1h
		out		64h,al
@@LP2:	IN		AL,64H
		ROR		AL,1
		ROR		AL,1
		JC		@@LP2
		OR		AH,2
		MOV		AL,AH
		OUT		60H,AL
		STI 
;check again		
		call	DLY50

@@ex:	ret
A20ON	endp

;set A20 on,return A20 status in AX
A20OFF	proc	near
		call	ISA20ON		
		JZ		@@ex1
;try PS/2 way
		CLI
		IN      AL,92H
		AND     AL,0FDh
		OUT     92H,AL
		STI
		call	DLY50
		call	ISA20ON
		JZ		@@ex1
; try with KBC way
		CLI 
		MOV		AL,0D0H
		OUT		64H,AL
@@LP11:	IN		AL,64H
		ROR		AL,1
		JNC		@@LP11
		IN		AL,60H
		MOV		AH,AL
		MOV		AL,0D1H
		OUT		64H,AL
@@LP12:	IN		AL,64H
		ROR		AL,1
		ROR		AL,1
		JC		@@LP12
		AND		AH,0FDH
		MOV		AL,AH
		OUT		60H,AL
		STI 
		call	DLY50
@@ex1:	ret
A20OFF	endp

; return 1 if A20 is ON,
ISA20ON proc	near
		PUSH	DS
		XOR		AX,AX
		MOV		DS,AX
		DEC		AX
		MOV		ES,AX
		CLI
		MOV		AL,[DS:80H]
		MOV		BL,[ES:90H]
		MOV		AH,AL			;backup
		XOR		AL,55H
		MOV		[ES:90H],AL
		CMP		[DS:80H],AL		;if ==, A20 is Off
		MOV		[DS:80H],AH		;restore
		MOV		[ES:90H],BL
		STI
		POP		DS
		MOV		AX,0
		JZ		@@1
		INC		AX
		OR		AX,AX		;set flags
@@1:	ret
ISA20ON	endp

;delay 50 miliseconds
DLY50	proc	near
		MOV		AX,50		;50 ms
		MOV		CX,AX
		SHL		CH,2		;time*4
		MOV		AL,250		
		MUL		CL			;H=time*250*4
		MOV		DX,AX		
		XOR		CX,CX	
		MOV		AH,86H		;wait
		INT		15H
		ret
DLY50	endp

msgON	db		'A20 is ON',0dh,0ah,'$'
msgOFF	db		'A20 is OFF',0dh,0ah,'$'
msgMAlt	db		'Mode Alternate',0dh,0ah,'$'
msgMOn	db		'Mode ON',0dh,0ah,'$'
msgMOff	db		'Mode Off',0dh,0ah,'$'
msgErr	db		'Parameter error! Usage:',0dh,0ah
	db		' A20    -  alternate ON/OFF',0dh,0ah
	db		' A20 -0  - turn A20 off',0dh,0ah
	db		' A20 -1  - turn A20 on',0dh,0ah,'$'

END		A20
%

;_TEXT16	ends  ;ML
      ;end
                end
