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

INCPATH=..\include;.;..\..\include

CPP_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -zls -os -s -zdp -zu -3 -bt=dos -fo=.obj -ml -xd -v -i$(INCPATH)
C_OPTIONS=-w4 -e25 -ei -zp1 -zq -zpw -zls -os -s -zdp -zu -ntFARCODE -3 -bt=dos -fo=.obj -mc -i$(INCPATH)
ASM_OPTIONS=-mc -d1 -w4 -e25 -zq
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

.cpp.obj:
   $(CPP) $<

.c.obj:
   $(C) $<

.asm.obj:
   $(ASM) $<

#CPP Options
#-ei   This option can be used to force the compiler to allocate at least an "int" 
#      for all enumerated types.  The macro __SW_EI will be predefined if "ei" is 
#      selected.
#-od   No optimizations
#-os   Space is favoured over time when generating code (smaller code but possibly 
#      slower execution).  By default, the compiler selects a balance between "space" 
#      and "time".  The macro __SW_OS will be predefined if "os" is selected.
#-s    Stack overflow checking is omitted from the generated code.  By default, 
#      the compiler will emit code at the beginning of every function that checks 
#      for the "stack overflow" condition.  This option can be used to disable 
#      this feature.  The macro __SW_S will be predefined if "s" is selected.
#-xd   This option disables exception handling.  It is the default option if 
#      no exception handling option is specified.  When this option is 
#      specified (or defaulted):
#      - Destruction of objects is caused by direct calls to the appropriate 
#        destructors
#      - Destructor functions are implemented with direct calls to appropriate 
#        destructors to destruct base classes and class members.
#-zpw  The compiler will output a warning message whenever padding is added to 
#      a struct/class for alignment purposes.
#-zls  The "zls" option tells the compilers to remove automatically inserted 
#      symbols.  These symbols are usually used to force symbol references to 
#      be fixed up from the run-time libraries.  
#      An example would be the symbol __DLLstart_, that is inserted into any 
#      object file that has a DllMain() function defined within its source file.
#-zdp  The "zdf" option allows the code generator to use the DS register to 
#      point to other segments besides "DGROUP" This is the default in the 
#      16-bit compact, large, and huge memory models (except for 16-bit Windows
#      applications).
#      The "zdp" option informs the code generator that the DS register must 
#      always point to "DGROUP" This is the default in the 16-bit small and 
#      medium memory models, all of the 16-bit Windows memory models, and the 
#      32-bit small and flat memory models.  
#      The macro __SW_ZDF will be predefined if "zdf" is selected.  
#      The macro __SW_ZDP will be predefined if "zdp" is selected.
#-zp1  The "zp" option allows you to specify the alignment of members in a 
#      structure.  The default is "zp2" for the 16-bit compiler and "zp8" for 
#      32-bit compiler.  The alignment of structure members is described in the 
#      following table.  If the size of the member is 1, 2, 4, 8 or 16, the 
#      alignment is given for each of the "zp" options.  If the member of the 
#      structure is an array or structure, the alignment is described by the 
#      row "x".
#-zu   The "zu" option relaxes the restriction that the SS register contains 
#      the base address of the default data segment, "DGROUP".  Normally, all 
#      data items are placed into the group "DGROUP" and the SS register 
#      contains the base address of this group.  When the "zu" option is 
#      selected, the SS register is volatile (assumed to point to another 
#      segment) and any global data references require loading a segment 
#      register such as DS with the base address of "DGROUP".
#      (16-bit only) This option is useful when compiling routines that are 
#      to be placed in a Dynamic Link Library (DLL) since the SS register 
#      points to the stack segment of the calling application upon entry to 
#      the function.
#      The macro __SW_ZU will be predefined if "zu" is selected.
#-zc   Constants in code segment
#-zq   The "quiet mode" option causes the informational messages displayed by 
#      the compiler to be suppressed.  Normally, messages are displayed identifying 
#      the compiler and summarizing the number of lines compiled.  
#      As well, a dot is displayed every few seconds while the code generator is active, 
#      to indicate that the compiler is still working.  
#      These messages are all suppressed by the "quiet mode" option.  Error and warning
#      messages are not suppressed.
