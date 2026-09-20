@echo off
rem ============================================================
rem  Command-line build helper for hello-triangle
rem  Usage:  build-cmdline.cmd [Debug or Release]
rem
rem  Finds Visual Studio (vswhere first, common paths as
rem  fallback), sets up the MSVC environment, then runs
rem  MSBuild on the .sln.  File content is ASCII-only on
rem  purpose: cmd parses .cmd files as ANSI, so the Chinese
rem  folder path is only handled at runtime, via %~dp0.
rem ============================================================
setlocal
set "CFG=%~1"
if "%CFG%"=="" set "CFG=Debug"

set "VS_PATH="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%i in (`"%VSWHERE%" -products * -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath 2^>nul`) do set "VS_PATH=%%i"
)
rem vswhere reports nothing while an update is running; fall back to known paths
if not defined VS_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\18\Community"
if not defined VS_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvars64.bat" set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2026\Community"
if not defined VS_PATH if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" set "VS_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community"
if not defined VS_PATH (
    echo [ERROR] Visual Studio with C++ workload not found.
    echo         If VS is updating right now, wait for it to finish and retry.
    exit /b 1
)

call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d "%~dp0"
echo Building configuration %CFG%, platform x64 ...
msbuild hello-triangle.sln /t:Build /p:Configuration="%CFG%" /p:Platform=x64 /m /v:m /nologo
if errorlevel 1 (echo [ERROR] build failed. & exit /b 1)
echo.
echo [OK] Output: build\%CFG%\hello-triangle.exe
