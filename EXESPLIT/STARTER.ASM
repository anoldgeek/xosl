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
; - Rename function "main" into "mymain" to get rid of
;   linker Error! E2028: _main is an undefined reference
;

                .model  compact
                .stack  16384
                .386p
                .code
                ;extrn   _main: near
                extrn   _mymain: near

                .startup
                ;call    _main
                call    _mymain
                xor     ax,ax
                .exit

                end
