# Borland C++ 3.1

!ifdef DOS_DEBUG
MAKE_OPTIONS=-DDOS_DEBUG
!endif

ENTER=cd
LEAVE=$(ENTER) ..
COPY=copy
DELETE=del

all: build arch

#------------------------- BUILD ------------------------- #

build: xosl xoslload install ipl


xosl:
    $(ENTER) xosl
    $(MAKEDIR)\make $(MAKE_OPTIONS)
    split
    $(LEAVE)

xoslload:
    $(ENTER) xoslload
    $(MAKEDIR)\make $(MAKE_OPTIONS)
    $(LEAVE)

install:
    $(ENTER) install
    $(MAKEDIR)\make $(MAKE_OPTIONS)
    $(LEAVE)

ipl:
    $(ENTER) ipl
    $(ENTER) ipl_brec
    $(MAKEDIR)\make $(MAKE_OPTIONS)
    $(LEAVE)
    $(ENTER) ipl_mbr
    $(MAKEDIR)\make $(MAKE_OPTIONS)
    $(LEAVE)
    $(LEAVE)

#------------------------- ARCHIVE ------------------------- #

arch:
    $(COPY) xosl\*.xxf arch
    $(COPY) xoslload\xoslload.xcf arch
    $(COPY) install\install.exe arch
    $(COPY) ipl\ipl_brec\*.bin arch
    $(COPY) ipl\ipl_mbr\*.bin arch
    $(COPY) resource\*.* arch

#------------------------- CLEAN ------------------------- #

clean: clean_xosl clean_xoslload clean_install clean_ipl clean_arch


clean_xosl:
    $(ENTER) xosl
    $(MAKEDIR)\make clean
    $(LEAVE)

clean_xoslload:
    $(ENTER) xoslload
    $(MAKEDIR)\make clean
    $(LEAVE)

clean_install:
    $(ENTER) install
    $(MAKEDIR)\make clean
    $(LEAVE)

clean_ipl:
    $(ENTER) ipl
    $(ENTER) ipl_brec
    $(MAKEDIR)\make clean
    $(LEAVE)
    $(ENTER) ipl_mbr
    $(MAKEDIR)\make clean
    $(LEAVE)
    $(LEAVE)

clean_arch:
    $(ENTER) arch
    $(DELETE) *.xxf
    $(DELETE) *.xcf
    $(DELETE) *.xff
    $(DELETE) *.xbf
    $(DELETE) *.exe
    $(DELETE) *.bin
    $(DELETE) *.mbr
    $(DELETE) *.txt
    $(LEAVE)