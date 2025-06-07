@echo off
setlocal enabledelayedexpansion

:: Configuration
set ARCH=x86_amd64
set BUILD_DIR=build_release
set BACKUP_DIR=build_release\build_last

:: Compiler Flags (Release Mode)
set CFLAGS=/O2 /MT /GL /W3 /Iinclude /DNDEBUG
set LDFLAGS=/LTCG

:: Libraries to link against
set LIBS=user32.lib gdi32.lib comctl32.lib comdlg32.lib shell32.lib

:: Find Visual Studio
set VS_PATH=
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
)
if "!VS_PATH!"=="" if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
)

if "!VS_PATH!"=="" (
    echo Error: Visual Studio not found.
    exit /b 1
)

:: Initialize VS environment
call "!VS_PATH!" %ARCH%
if errorlevel 1 (
    echo Failed to initialize Visual Studio environment.
    exit /b 1
)

:: Backup previous build if exists
if exist %BUILD_DIR% (
    echo Backing up previous release build...
    if exist %BACKUP_DIR% rd /s /q %BACKUP_DIR%
    ren %BUILD_DIR% %BACKUP_DIR%
)

mkdir %BUILD_DIR% 2>nul

echo Compiling in Release mode...

:: Compile all C files
for %%f in (src\*.c) do (
    cl %CFLAGS% /c /Fo:%BUILD_DIR%\%%~nf.obj %%f
    if errorlevel 1 (
        echo Compilation failed for %%f
        exit /b 1
    )
)

:: Compile resources
rc /fo %BUILD_DIR%\main.res src\main.rc
if errorlevel 1 (
    echo Resource compilation failed for main.rc
    exit /b 1
)

rc /fo %BUILD_DIR%\icon.res src\icon.rc
if errorlevel 1 (
    echo Resource compilation failed for icon.rc
    exit /b 1
)

:: Collect all object files
set OBJ_FILES=
for %%f in (%BUILD_DIR%\*.obj) do (
    set OBJ_FILES=!OBJ_FILES! %%f
)

:: Link everything
link %LDFLAGS% /OUT:%BUILD_DIR%\out.exe %OBJ_FILES% %BUILD_DIR%\main.res %BUILD_DIR%\icon.res %LIBS%
if errorlevel 1 (
    echo Linking failed.
    exit /b 1
)

echo ----------------------------------------
echo Release build successful: %BUILD_DIR%\out.exe
echo ----------------------------------------

:: Optional: Run the executable
REM %BUILD_DIR%\out.exe
