@echo off

c:\windows\system32\forfiles.exe /p arch /s /m IPLS*.bin /c "cmd /c if @fsize neq 510 echo **** @file @fsize SIZE MUST BE 510 ****"
c:\windows\system32\forfiles.exe /p arch /s /m IPL0*.bin /c "cmd /c if @fsize neq 440 echo **** @file @fsize SIZE MUST BE 440 ****"

rem c:\windows\system32\forfiles.exe /p arch /s /m IPLS*.bin /c "cmd /c if @fsize neq 510 echo **** @file @fsize SIZE MUST BE 502 ****"
rem c:\windows\system32\forfiles.exe /p arch /s /m IPL0*.bin /c "cmd /c if @fsize neq 440 echo **** @file @fsize SIZE MUST BE 432 ****"
