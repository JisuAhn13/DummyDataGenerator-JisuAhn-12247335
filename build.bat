@echo off
chcp 65001 > nul

echo =========================================
echo   DummyDataGenerator - Build
echo =========================================

set CMAKE="C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set NINJA="C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
set VCVARS="C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvarsall.bat"
set SRC=%~dp0
set BLD=%~dp0build

call %VCVARS% x64

if not exist "%BLD%" mkdir "%BLD%"

%CMAKE% -G Ninja -DCMAKE_MAKE_PROGRAM=%NINJA% -DCMAKE_BUILD_TYPE=Release -S "%SRC%" -B "%BLD%"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake 구성 실패
    pause
    exit /b 1
)

%CMAKE% --build "%BLD%" --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] 빌드 실패
    pause
    exit /b 1
)

echo.
echo [OK] 빌드 완료: build\DummyDataGenerator.exe
pause
