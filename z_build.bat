@echo off
REM ///////////////////////////////////////////////////////////////////////////
REM // Set Paths
REM ///////////////////////////////////////////////////////////////////////////
set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build"

REM ###############################################
REM # DO NOT MODIFY ANY LINES BELOW THIS LINE !!! #
REM ###############################################

REM ///////////////////////////////////////////////////////////////////////////
REM // Setup environment
REM ///////////////////////////////////////////////////////////////////////////
if exist "%QTVC_PATH%\bin\qtvars.bat" ( call "%QTVC_PATH%\bin\qtvars.bat" )
if exist "%QTVC_PATH%\bin\qtenv2.bat" ( call "%QTVC_PATH%\bin\qtenv2.bat" )
call "%MSVC_PATH%\vcvarsall.bat" x86

REM ///////////////////////////////////////////////////////////////////////////
REM // Check environment
REM ///////////////////////////////////////////////////////////////////////////
if "%VCINSTALLDIR%"=="" (
	echo %%VCINSTALLDIR%% not specified. Please check your MSVC_PATH var!
	goto BuildError
)
if "%QTDIR%"=="" (
	echo %%QTDIR%% not specified. Please check your MSVC_PATH var!
	goto BuildError
)
if not exist "%VCToolsInstallDir%\bin\HostX86\x86\cl.exe" (
	echo C++ compiler not found. Please check your MSVC_PATH var!
	goto BuildError
)
if not exist "%QTDIR%\bin\moc.exe" (
	echo Qt meta compiler not found. Please check your QTDIR var!
	goto BuildError
)
if not exist "%QTDIR%\include\QtCore\qglobal.h" (
	echo %%QTDIR%% header files not found. Please check your QTDIR var!
	goto BuildError
)

REM ///////////////////////////////////////////////////////////////////////////
REM // Get current date and time (in ISO format)
REM ///////////////////////////////////////////////////////////////////////////
set "ISO_DATE="
set "ISO_TIME="
if not exist "%~dp0\..\Prerequisites\MSYS\1.0\bin\date.exe" BuildError
for /F "tokens=1,2 delims=:" %%a in ('"%~dp0\..\Prerequisites\MSYS\1.0\bin\date.exe" +ISODATE:%%Y-%%m-%%d') do (
	if "%%a"=="ISODATE" set "ISO_DATE=%%b"
)
for /F "tokens=1,2,3,4 delims=:" %%a in ('"%~dp0\..\Prerequisites\MSYS\1.0\bin\date.exe" +ISOTIME:%%T') do (
	if "%%a"=="ISOTIME" set "ISO_TIME=%%b:%%c:%%d"
)
if "%ISO_DATE%"=="" goto BuildError
if "%ISO_TIME%"=="" goto BuildError

REM ///////////////////////////////////////////////////////////////////////////
REM // Build the binaries
REM ///////////////////////////////////////////////////////////////////////////
echo ---------------------------------------------------------------------
echo BEGIN BUILD
echo ---------------------------------------------------------------------
MSBuild.exe /property:Configuration=release_static /property:Platform=Win32 /target:clean   "%~dp0\MediaInfoXP.sln"
if not "%ERRORLEVEL%"=="0" goto BuildError
MSBuild.exe /property:Configuration=release_static /property:Platform=Win32 /target:rebuild "%~dp0\MediaInfoXP.sln"
if not "%ERRORLEVEL%"=="0" goto BuildError
MSBuild.exe /property:Configuration=release_static /property:Platform=Win32 /target:build   "%~dp0\MediaInfoXP.sln"
if not "%ERRORLEVEL%"=="0" goto BuildError

REM ///////////////////////////////////////////////////////////////////////////
REM // Copy base files
REM ///////////////////////////////////////////////////////////////////////////
echo ---------------------------------------------------------------------
echo BEGIN PACKAGING
echo ---------------------------------------------------------------------
set "PACK_PATH=%TMP%\~%RANDOM%%RANDOM%.tmp"
mkdir "%PACK_PATH%"
copy "%~dp0\bin\Win32\Release_Static\*.exe" "%PACK_PATH%"
copy "%~dp0\Copying.txt" "%PACK_PATH%"
copy "%~dp0\doc\*.txt"   "%PACK_PATH%"
copy "%~dp0\doc\*.html"  "%PACK_PATH%"
copy "%~dp0\doc\*.svg"   "%PACK_PATH%"

REM ///////////////////////////////////////////////////////////////////////////
REM // Compress
REM ///////////////////////////////////////////////////////////////////////////
::"%~dp0\..\Prerequisites\UPX\upx.exe" --best  "%PACK_PATH%\*.exe"

REM ///////////////////////////////////////////////////////////////////////////
REM // Attributes
REM ///////////////////////////////////////////////////////////////////////////
attrib +R "%PACK_PATH%\*.exe"
attrib +R "%PACK_PATH%\*.html"
attrib +R "%PACK_PATH%\*.txt"
attrib +R "%PACK_PATH%\*.svg"

REM ///////////////////////////////////////////////////////////////////////////
REM // Generate outfile name
REM ///////////////////////////////////////////////////////////////////////////
set "OUT_NAME=MediaInfo-GUI.%ISO_DATE%"
:CheckOutName
if exist "%~dp0\out\%OUT_NAME%.zip" (
	set "OUT_NAME=%OUT_NAME%.new"
	goto CheckOutName
)

REM ///////////////////////////////////////////////////////////////////////////
REM // Create version tag
REM ///////////////////////////////////////////////////////////////////////////
echo MediaInfoXP - Simple GUI for MediaInfo > "%~dp0\out\%OUT_NAME%.txt"
echo Built %ISO_DATE%, %TIME% >> "%~dp0\out\%OUT_NAME%.txt"
echo Copyright (C) 2004-2021 LoRd_MuldeR ^<MuldeR2@GMX.de^> >> "%~dp0\out\%OUT_NAME%.txt"
echo. >> "%~dp0\out\%OUT_NAME%.txt"
echo This program is free software; you can redistribute it and/or modify >> "%~dp0\out\%OUT_NAME%.txt"
echo it under the terms of the GNU General Public License as published by >> "%~dp0\out\%OUT_NAME%.txt"
echo the Free Software Foundation; either version 2 of the License, or >> "%~dp0\out\%OUT_NAME%.txt"
echo (at your option) any later version. >> "%~dp0\out\%OUT_NAME%.txt"
echo. >> "%~dp0\out\%OUT_NAME%.txt"
echo ------------------------------------------------------------------->> "%~dp0\out\%OUT_NAME%.txt"
echo. >> "%~dp0\out\%OUT_NAME%.txt"
echo MediaInfo(Lib) is Copyright (C) 2002-2021 MediaArea.net SARL. >> "%~dp0\out\%OUT_NAME%.txt"
echo. >> "%~dp0\out\%OUT_NAME%.txt"
echo Redistribution and use in source and binary forms, with or without >> "%~dp0\out\%OUT_NAME%.txt"
echo modification, are permitted according to the BSD-2-Clause license. >> "%~dp0\out\%OUT_NAME%.txt"
echo. >> "%~dp0\out\%OUT_NAME%.txt"

REM ///////////////////////////////////////////////////////////////////////////
REM // Build the package
REM ///////////////////////////////////////////////////////////////////////////
pushd "%PACK_PATH%
"%~dp0\..\Prerequisites\InfoZip\zip.exe" -9 -r -z "%~dp0\out\%OUT_NAME%.zip" "*.*" < "%~dp0\out\%OUT_NAME%.txt"
popd
rmdir /Q /S "%PACK_PATH%"
attrib +R "%~dp0\out\%OUT_NAME%.zip"
del "%~dp0\out\%OUT_NAME%.txt"

REM ///////////////////////////////////////////////////////////////////////////
REM // COMPLETE
REM ///////////////////////////////////////////////////////////////////////////
echo.
echo Build completed.
echo.
pause
goto:eof

REM ///////////////////////////////////////////////////////////////////////////
REM // FAILED
REM ///////////////////////////////////////////////////////////////////////////
:BuildError
echo.
echo Build has failed !!!
echo.
pause
