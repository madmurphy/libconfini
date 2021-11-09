@ECHO OFF
SETLOCAL

REM Please make sure that Microsoft Visual Studio compiler and linker are
REM reachable from your %Path% environment variable.

SET RCOMPILER=rc.exe
SET CCOMPILER=cl.exe
SET LINKER=link.exe
SET STRIPCMD=strip.exe
SET CFLAGS=/c /O2
SET DLLOUTPUT=libconfini.dll
SET DEFFILE=src\libconfini.def
SET RCFILE=src\winres.rc
REM The following variable may contain a space-delimited list of files
SET SRCFILES=src\confini.c

WHERE %CCOMPILER% >nul 2>nul

IF %ERRORLEVEL% NEQ 0 (
	@ECHO Program %CCOMPILER% has not been found. Abort.
	EXIT /B 1
)

WHERE %RCOMPILER% >nul 2>nul
SET /A __RCERR__=%ERRORLEVEL%
SET __TMP_RSO__=winres.res

IF %__RCERR__% NEQ 0 (
	@ECHO Program %RCOMPILER% has not been found. Proceeding without resource file.
	GOTO COMPILE_DLL
)

@ECHO Loading resource file...
%RCOMPILER% /fo %__TMP_RSO__% %RCFILE%
SET /A __RCERR__=%ERRORLEVEL%

IF %__RCERR__% NEQ 0 (
	@ECHO An error occured. Proceeding without resource file.
)

:COMPILE_DLL
SET __TMP_OBJ__=confini.obj
@ECHO Compiling %SRCFILES%...

%CCOMPILER% /Fo%__TMP_OBJ__% %CFLAGS% %SRCFILES%

SET /A __CCERR__=%ERRORLEVEL%

IF %__CCERR__% NEQ 0 (
	@ECHO An error occured while compiling the library. Abort.
	EXIT /B %__CCERR__%
)

@ECHO Creating %DLLOUTPUT%...

IF %__RCERR__% EQU 0 (
    %LINKER% /DLL /DEF:%DEFFILE% /OUT:%DLLOUTPUT% %__TMP_RSO__% %__TMP_OBJ__%
) ELSE (
    %LINKER% /DLL /DEF:%DEFFILE% /OUT:%DLLOUTPUT% %__TMP_OBJ__%
)

SET /A __LINKERR__=%ERRORLEVEL%

IF %__LINKERR__% EQU 0 (
	IF %__RCERR__% EQU 0 DEL %__TMP_RSO__%
	DEL %__TMP_OBJ__%
	@ECHO Done.
) ELSE (
	@ECHO Could not create the DLL.
)

EXIT /B %__CCERR__%

ENDLOCAL
