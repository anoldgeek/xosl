;
; Extended Operating System Loader (XOSL)
; Copyright (c) 1999 by Geurt Vos
;
; This code is distributed under GNU General Public License (GPL)
;
; The full text of the license can be found in the GPL.TXT file,
; or at http://www.gnu.org
;

                .model  tiny
                .386p
                .data
                db      100 dup (0)
		db       30 dup (0)  ;Tune filesize iplslba.bin to 512 bytes

                end