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
; - Tune filesize ipls.bin to 512 bytes
;

                .model  tiny
                .386p
                .data
;                db      26 dup (0)	;Tune filesize ipls.bin to 510 bytes (leave space for int magic number).
                db      4 dup (0)	;Tune filesize ipls.bin to 510 bytes (leave space for int magic number).

                end
;; **** "ipls.bin" 518 SIZE MUST BE 510 ****