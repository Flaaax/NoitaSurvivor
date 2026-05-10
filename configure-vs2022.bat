@echo off
setlocal

if "%VCPKG_ROOT%"=="" set "VCPKG_ROOT=C:\dev\vcpkg"

set "BUILD_DIR=%~dp0build"

cmake -S "%~dp0" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"

endlocal
