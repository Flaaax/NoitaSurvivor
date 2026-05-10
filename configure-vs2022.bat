@echo off
setlocal

if "%VCPKG_ROOT%"=="" set "VCPKG_ROOT=C:\dev\vcpkg"

set "SOURCE_DIR=%~dp0."
set "BUILD_DIR=%~dp0build"

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_MANIFEST_MODE=OFF

if errorlevel 1 (
  echo.
  echo CMake configure failed.
  pause
  exit /b 1
)

endlocal
