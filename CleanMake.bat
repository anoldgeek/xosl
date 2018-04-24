@echo off
rem   Extended Operating System Loader (XOSL)
rem   Copyright (c) 1999 by Geurt Vos
rem   This code is distributed under GNU General Public License (GPL)
rem   The full text of the license can be found in the GPL.TXT file,
rem   or at http://www.gnu.org
rem
rem   Open Watcom Migration
rem   Copyright (c) 2010 by Mario Looijkens:
rem   Build Host: WindowXP
rem   Build with Open Watcom version 1.8
rem   http://www.openwatcom.org/index.php/Download
rem   Open Watcom Installer: open-watcom-c-win32-1.8.exe
rem
echo Clean XOSL
echo ==========
echo.
call WatcomEnv.bat
set WORKING_DIRECTORY=%cd%
set OW18BIN_PATH=%WATCOM%\BINNT
set PATH=%OW18BIN_PATH%
echo Path to Open Watcom binaries ...
path
echo.
cd %WORKING_DIRECTORY%
echo Working Directory ...
cd
echo.
echo To abort press Ctrl-C. To clean XOSL
pause
wmake clean
echo.
echo Done! To exit this batch file
pause
