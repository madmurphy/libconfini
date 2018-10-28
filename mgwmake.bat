@ECHO OFF
SETLOCAL

REM Don't forget to add C:\MinGW\bin to your user %Path% environment variable,
REM otherwise this script won't work.

SET RCCOMPILER=windres.exe
SET CCOMPILER=gcc.exe
SET CFLAGS=-std=c99 -g -O3 -Wall -shared -static-libgcc -Wl,--no-undefined -Wl,--subsystem,windows
SET DLLOUTPUT=libconfini.dll
SET RCFILE=src\winres.rc
REM The following variable may contain a space-delimited list of files
SET SRCFILES=src\confini.c

WHERE %CCOMPILER% >nul 2>nul

IF %ERRORLEVEL% NEQ 0 (
	@ECHO Program %CCOMPILER% was not found.
	EXIT /B 1
)

WHERE %RCCOMPILER% >nul 2>nul
SET /A __WINDRESERR__=%ERRORLEVEL%
SET __TMP_RSO__=tmpresource.o

IF %__WINDRESERR__% NEQ 0 (
	@ECHO Program %RCCOMPILER% was not found. Proceeding without resource file.
	GOTO COMPILE_DLL
)

@ECHO Loading resource file...
%RCCOMPILER% --codepage=65001 -i %RCFILE% -o %__TMP_RSO__%
SET /A __WINDRESERR__=%ERRORLEVEL%

IF %__WINDRESERR__% NEQ 0 (
	@ECHO An error occured. Proceeding without resource file.
)

:COMPILE_DLL

@ECHO Compiling %DLLOUTPUT%...

IF %__WINDRESERR__% EQU 0 (
	%CCOMPILER% %CFLAGS% -o %DLLOUTPUT% %SRCFILES% %__TMP_RSO__%
) ELSE (
	%CCOMPILER% %CFLAGS% -o %DLLOUTPUT% %SRCFILES%
)

SET /A __GCCERR__=%ERRORLEVEL%

IF %__WINDRESERR__% EQU 0 DEL %__TMP_RSO__%

IF %__GCCERR__% EQU 0 @ECHO Done.

EXIT /B %__GCCERR__%

ENDLOCAL
