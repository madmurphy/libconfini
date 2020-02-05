@ECHO OFF
SETLOCAL

REM Do not forget to add C:\MinGW\bin to your %Path% environment variable, or
REM otherwise this script will not work!

SET RCCOMPILER=windres.exe
SET CCOMPILER=gcc.exe
SET STRIPCMD=strip.exe
SET CFLAGS=-std=c99 -pedantic -g -O3 -Wall -shared -static-libgcc -Wl,--no-undefined -Wl,-out-implib,libconfini.lib
SET DLLOUTPUT=libconfini.dll
SET RCFILE=src\winres.rc
REM The following variable may contain a space-delimited list of files
SET SRCFILES=src\confini.c

WHERE %CCOMPILER% >nul 2>nul

IF %ERRORLEVEL% NEQ 0 (
	@ECHO Program %CCOMPILER% has not been found. Abort.
	EXIT /B 1
)

WHERE %RCCOMPILER% >nul 2>nul
SET /A __WINDRESERR__=%ERRORLEVEL%
SET __TMP_RSO__=winres.o

IF %__WINDRESERR__% NEQ 0 (
	@ECHO Program %RCCOMPILER% has not been found. Proceeding without resource file.
	GOTO COMPILE_DLL
)

@ECHO Loading resource file...
%RCCOMPILER% -i %RCFILE% -o %__TMP_RSO__%
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

IF %__GCCERR__% NEQ 0 (
	EXIT /B %__GCCERR__%
)

WHERE %STRIPCMD% >nul 2>nul
SET /A __STRIPCMDERR__=%ERRORLEVEL%

IF %__STRIPCMDERR__% EQU 0 (
	@ECHO Stripping %DLLOUTPUT%...
) ELSE (
	@ECHO Program %STRIPCMD% has not been found. The dll has not been stripped.
	EXIT /B 0
)

%STRIPCMD% %DLLOUTPUT%
SET /A __STRIPPROGERR__=%ERRORLEVEL%

IF %__STRIPPROGERR__% EQU 0 (
	@ECHO Done.
)

EXIT /B %__STRIPPROGERR__%

ENDLOCAL

