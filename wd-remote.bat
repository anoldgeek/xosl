cd \xosldev\xosl\arch
rem wd /tr=ser /in=xosl.dbg c:\xosldev\xosl\arch\xosl.exe

wdw /tr=ser c:\xosldev\xosl\arch\xosl.exe

rem wdw /tr=ser c:\xosldev\xosl120\arch\xosl.exe

rm mo /word 0:7c00 0x81,0x6,0x3800,0xffff

rm mo /word 0:7c00 0x80,0x6,0x1000,0xffff,0x0,0x0

rm mo /word 0:7c00 0x83,0x6,0x3800,0xffff,0,0
