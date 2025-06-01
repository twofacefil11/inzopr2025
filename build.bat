@echo off
REM setlocal enabledelayedexpansion

REM set ARCH=x86_amd64
REM set VS_PATH=

REM :: Try Enterprise first
REM if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
REM     set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
REM )

REM :: Fallback to Community
REM if "!VS_PATH!"=="" if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
REM     set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
REM )

REM :: Neither is found
REM if "!VS_PATH!"=="" (
REM     echo Oj... Chyba nie masz Visual Studio. 
REM     exit /b 1
REM )

REM :: Call vcvarsall.bat to set up environment
REM call "!VS_PATH!" %ARCH%
REM if errorlevel 1 (
REM     echo Failed to initialize Visual Studio environment.
REM     exit /b 1
REM )

REM :: Setup build dirs
REM set BUILD_DIR=build
REM set BACKUP_DIR=build\build_last

REM :: Backup previous build
REM if exist %BUILD_DIR% (
REM     echo Backing up previous build to %BACKUP_DIR%...
REM     if exist %BACKUP_DIR% rd /s /q %BACKUP_DIR%
REM     ren %BUILD_DIR% %BACKUP_DIR%
REM )

REM mkdir %BUILD_DIR%

REM :: Compile each .c file into .obj files
REM for %%f in (src\*.c) do (
REM     cl /Iinclude /c /Fo:%BUILD_DIR%\%%~nf.obj %%f
REM     if errorlevel 1 (
REM         echo Compilation failed for %%f.
REM         exit /b 1
REM     )
REM )

REM :: Link all the .obj files
REM set OBJ_FILES=
REM for %%f in (%BUILD_DIR%\*.obj) do (
REM     set OBJ_FILES=!OBJ_FILES! %%f
REM )

REM cl /Fe:%BUILD_DIR%\out.exe %OBJ_FILES% user32.lib gdi32.lib comdlg32.lib
REM if errorlevel 1 (
REM     echo Linking failed.
REM     exit /b 1
REM )

REM REM :: Compile
REM REM cl /Iinclude /Fo:%BUILD_DIR%\main.obj /Fe:%BUILD_DIR%\out.exe src\main.c user32.lib gdi32.lib

REM :: Run the executable
REM %BUILD_DIR%\out.exe
REM REM -------------------------------------------------------------

@echo off
setlocal enabledelayedexpansion

set ARCH=x86_amd64
set VS_PATH=

:: Try Enterprise first
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
)

:: Fallback to Community
if "!VS_PATH!"=="" if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

:: Neither is found
if "!VS_PATH!"=="" (
    echo Oj... Chyba nie masz Visual Studio. 
    exit /b 1
)

:: Call vcvarsall.bat to set up environment
call "!VS_PATH!" %ARCH%
if errorlevel 1 (
    echo Failed to initialize Visual Studio environment.
    exit /b 1
)

:: Setup build dirs
set BUILD_DIR=build
set BACKUP_DIR=build\build_last

:: Backup previous build
if exist %BUILD_DIR% (
    echo Backing up previous build to %BACKUP_DIR%...
    if exist %BACKUP_DIR% rd /s /q %BACKUP_DIR%
    ren %BUILD_DIR% %BACKUP_DIR%
)

mkdir %BUILD_DIR%

:: Compile each .c file into .obj files
for %%f in (src\*.c) do (
    cl /Iinclude /c /Fo:%BUILD_DIR%\%%~nf.obj %%f
    if errorlevel 1 (
        echo Compilation failed for %%f.
        exit /b 1
    )
)

:: ==== ADD THIS BLOCK TO COMPILE YOUR RESOURCE FILE (main.rc) ====

:: Compile resource script (main.rc) into a .res file to embed manifest
rc /fo %BUILD_DIR%\main.res src\main.rc 
if errorlevel 1 (
    echo Resource compilation failed. on 126
    exit /b 1
)

:: ==== END ADD RESOURCE COMPILATION ====

:: Link all the .obj files
set OBJ_FILES=
for %%f in (%BUILD_DIR%\*.obj) do (
    set OBJ_FILES=!OBJ_FILES! %%f
)

:: ==== MODIFY LINKING COMMAND TO INCLUDE THE RESOURCE FILE AND comctl32.lib ====

cl /Fe:%BUILD_DIR%\out.exe %OBJ_FILES% %BUILD_DIR%\main.res user32.lib gdi32.lib comctl32.lib comdlg32.lib shell32.lib
if errorlevel 1 (
    echo Linking failed.
    exit /b 1
)

:: ==== END MODIFIED LINK ====

REM :: Compile
REM cl /Iinclude /Fo:%BUILD_DIR%\main.obj /Fe:%BUILD_DIR%\out.exe src\main.c user32.lib gdi32.lib

:: Run the executable
%BUILD_DIR%\out.exe
