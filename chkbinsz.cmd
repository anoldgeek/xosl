@echo off
forfiles /p arch /s /m IPLS*.bin /c "cmd /c if @fsize neq 512 echo **** @file @fsize SIZE MUST BE 512 ****"
forfiles /p arch /s /m IPL0*.bin /c "cmd /c if @fsize neq 440 echo **** @file @fsize SIZE MUST BE 440 ****"
