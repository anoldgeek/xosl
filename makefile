# Open Watcom MAKEFILE
#
# Extended Operating System Loader (XOSL)
# Copyright (c) 1999 by Geurt Vos
# This code is distributed under GNU General Public License (GPL)
# The full text of the license can be found in the GPL.TXT file,
# or at http://www.gnu.org
#
# Open Watcom Migration
# Copyright (c) 2010 by Mario Looijkens:
# Copyright (c) 2018 by Norman Back:

#DOS_DEBUG=1

!ifdef DOS_DEBUG
MAKE_OPTIONS=DOS_DEBUG=1
!endif

ENTER=cd
LEAVE=$(ENTER) ..
COPY=copy
DELETE=del

all: build arch

#------------------------- BUILD ------------------------- #

build: .SYMBOLIC xosl xoslload install ipl


xosl: .SYMBOLIC
    $(ENTER) xosl
    wmake $(MAKE_OPTIONS)
    split
    $(LEAVE)

xoslload: .SYMBOLIC
    $(ENTER) xoslload
    wmake $(MAKE_OPTIONS)
    $(LEAVE)

install: .SYMBOLIC
    $(ENTER) install
    wmake $(MAKE_OPTIONS)
    $(LEAVE)

ipl: .SYMBOLIC
    $(ENTER) ipl
    $(ENTER) ipl_brec
    wmake $(MAKE_OPTIONS)
    $(LEAVE)
    $(ENTER) ipl_mbr
    wmake $(MAKE_OPTIONS)
    $(LEAVE)
    $(LEAVE)

#------------------------- ARCHIVE ------------------------- #

arch: .SYMBOLIC
    $(COPY) xosl\*.xxf arch
    $(COPY) xosl\xosl.* arch
    $(COPY) xoslload\xoslload.xcf arch
    $(COPY) install\install.exe arch
    $(COPY) install\install.map arch
    $(COPY) install\install.sym arch
    $(COPY) ipl\ipl_brec\*.bin arch
    $(COPY) ipl\ipl_mbr\*.bin arch
    $(COPY) resource\*.* arch
    @cmd.exe /c chkbinsz.cmd
#    dir arch\*.bin

#------------------------- CLEAN ------------------------- #

clean: clean_xosl clean_xoslload clean_install clean_ipl clean_arch


clean_xosl: .SYMBOLIC
    $(ENTER) xosl
    wmake clean
    $(LEAVE)

clean_xoslload: .SYMBOLIC
    $(ENTER) xoslload
    wmake clean
    $(LEAVE)

clean_install: .SYMBOLIC
    $(ENTER) install
    wmake clean
    $(LEAVE)

clean_ipl: .SYMBOLIC
    $(ENTER) ipl
    $(ENTER) ipl_brec
    wmake clean
    $(LEAVE)
    $(ENTER) ipl_mbr
    wmake clean
    $(LEAVE)
    $(LEAVE)

clean_arch: .SYMBOLIC
    $(ENTER) arch
    $(DELETE) *.xxf
    $(DELETE) *.xcf
    $(DELETE) *.xff
    $(DELETE) *.xbf
    $(DELETE) *.exe
    $(DELETE) *.bin
    $(DELETE) *.mbr
    $(DELETE) *.txt
    $(DELETE) *.map
    $(DELETE) *.sym
    $(LEAVE)
