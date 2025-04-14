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

:: Link all the .obj files
set OBJ_FILES=
for %%f in (%BUILD_DIR%\*.obj) do (
    set OBJ_FILES=!OBJ_FILES! %%f
)

cl /Fe:%BUILD_DIR%\out.exe %OBJ_FILES% user32.lib gdi32.lib
if errorlevel 1 (
    echo Linking failed.
    exit /b 1
)

REM :: Compile
REM cl /Iinclude /Fo:%BUILD_DIR%\main.obj /Fe:%BUILD_DIR%\out.exe src\main.c user32.lib gdi32.lib

:: Run the executable
%BUILD_DIR%\out.exe

