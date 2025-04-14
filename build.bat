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

:: Compile
cl /Iinclude /Fo:%BUILD_DIR%\main.obj /Fe:%BUILD_DIR%\out.exe src\main.c user32.lib gdi32.lib
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

:: Run the executable
%BUILD_DIR%\out.exe

