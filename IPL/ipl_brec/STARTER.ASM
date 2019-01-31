;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;

;/*
; * File  : MAIN.ASM
; * Author: Geurt Vos
; * Date  : March 1999
; *
; * Main routine for either FAT IPL
; */
; Open Watcom Migration
; Copyright (c) 2010 by Mario Looijkens:
; - Rename function main to mymain
; - Need to comment out .startup to get rid of Warning W1019: 
;   segment relocation at 0000:0001
; -    Need to add label start:
; - Why no ORG 100h here ????
; - Add label to directive end. Need label (end start) to get rid of 
;   Warning! W1023: no starting address found, using 0000:0000.
;

                .model  tiny
                .386p
                .data
LOADERADDR      equ     60000000h	; xoslloader load address
LOADERSEG       equ     6000h

		.code
                extrn   _mymain: near

		org	100h

;                .startup
start:
                jmp     LoaderStart
                nop

FatBootRec      db      67 dup (?)

LoaderStart:    
;0000:7c46:
                db      0eah            ;jmp 07b0:014b
;		dd      07b0014bh
		dw	LoaderStart + 5
		dw	07b0h
;07b0:014b:
                mov     ax,cs
                mov     ds,ax
                mov     ss,ax
                mov     sp,0fffeh

                call    _mymain


RunLoader:
                mov     ax,LOADERSEG
                mov     ds,ax
                mov     ss,ax
                mov     sp,0fffeh

; Pass brec seg:off to xoslload in ax,dx
		mov	ax,start
		mov	dx,cs

                db      0eah            ;execute com file
                dd      LOADERADDR

                end	start
