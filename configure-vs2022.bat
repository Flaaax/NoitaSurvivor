@echo off
setlocal

if "%VCPKG_ROOT%"=="" set "VCPKG_ROOT=C:\dev\vcpkg"

set "SOURCE_DIR=%~dp0."
set "BUILD_DIR=%~dp0build"
set "CACHE_FILE=%BUILD_DIR%\CMakeCache.txt"
set "VCPKG_INSTALLED_DIR=%BUILD_DIR%\vcpkg_installed"

if exist "%CACHE_FILE%" (
  findstr /C:"VCPKG_MANIFEST_MODE:BOOL=OFF" "%CACHE_FILE%" >nul
  if not errorlevel 1 (
    echo Existing CMake cache was configured with VCPKG_MANIFEST_MODE=OFF.
    echo Delete "%BUILD_DIR%" or configure into a fresh build directory, then run this script again.
    pause
    exit /b 1
  )
)

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 ^
  -U "Boost_DIR" ^
  -U "boost_*_DIR" ^
  -U "box2d_DIR" ^
  -U "fmt_DIR" ^
  -U "GLEW_DIR" ^
  -U "ImGui-SFML_DIR" ^
  -U "imgui_DIR" ^
  -U "nlohmann_json_DIR" ^
  -U "SFML_DIR" ^
  -U "spdlog_DIR" ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_MANIFEST_MODE=ON ^
  -DVCPKG_MANIFEST_DIR="%SOURCE_DIR%" ^
  -DVCPKG_INSTALLED_DIR="%VCPKG_INSTALLED_DIR%"

if errorlevel 1 (
  echo.
  echo CMake configure failed.
  pause
  exit /b 1
)

endlocal
