# Open Watcom Common MAKEFILE
#
# Extended Operating System Loader (XOSL)
# Copyright (c) 1999 by Geurt Vos
# This code is distributed under GNU General Public License (GPL)
# The full text of the license can be found in the GPL.TXT file,
# or at http://www.gnu.org
#
# Open Watcom Migration
# Copyright (c) 2010 by Mario Looijkens:
#

#DOS_DEBUG=true

INCPATH=..\include;.;$(%watcom)\h;..\..\include;..\install

#CPP_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -od -3 -bt=dos -fo=.obj -mc -xd -xr -i$(INCPATH)
#C_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -od -3 -bt=dos -fo=.obj -mc -xd -xr -i$(INCPATH)
#ASM_OPTIONS=-mc -d1 -w4 -e25 -zq
CPP_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -od -3 -bt=dos -fo=.obj -ml -xd -xr -i$(INCPATH)
C_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -od -3 -bt=dos -fo=.obj -ml -xd -xr -i$(INCPATH)
ASM_OPTIONS=-ml -d1 -w4 -e25 -zq
LIB_OPTIONS=-b -c -m -n -q -p=512 -l=$(LIST_FILE)

!ifdef DOS_DEBUG
CPP_OPTIONS=-DDOS_DEBUG -d2 $(CPP_OPTIONS)
C_OPTIONS=-DDOS_DEBUG -d2 $(C_OPTIONS)
ASM_OPTIONS=-DDOS_DEBUG -d1 $(ASM_OPTIONS)
!else
CPP_OPTIONS=$(CPP_OPTIONS)
C_OPTIONS=$(C_OPTIONS)
ASM_OPTIONS=$(ASM_OPTIONS)
!endif

!ifdef DISABLE_LBA
CPP_OPTIONS=-DDISABLE_LBA $(CPP_OPTIONS)
ASM_OPTIONS=-DDISABLE_LBA $(ASM_OPTIONS)
!endif

CPP=wpp $(CPP_OPTIONS)
C=wcc $(C_OPTIONS)
ASM=wasm $(ASM_OPTIONS)
LIB=wlib

$(LIB_NAME): $(COMPILE_OBJ)
   $(LIB) $(LIB_OPTIONS) $(LIB_NAME) $(LIB_OBJ)

clean: .SYMBOLIC
   @if exist *.obj del *.obj
   @if exist *.bak del *.bak
   @if exist *.lib del *.lib
   @if exist *.lst del *.lst
   @if exist *.err del *.err

.cpp.obj:
   $(CPP) $<

.c.obj:
   $(C) $<

.asm.obj:
   $(ASM) $<
