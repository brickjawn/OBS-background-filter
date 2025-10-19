@echo off
REM Build script for OBS Background Filter Plugin (Windows)

echo === OBS Background Filter Build Script ===
echo.

REM Configuration
set BUILD_TYPE=Release
if "%CMAKE_BUILD_TYPE%"=="" set BUILD_TYPE=%CMAKE_BUILD_TYPE%

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake is required but not found in PATH
    exit /b 1
)

REM Check for Visual Studio
where msbuild >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: Visual Studio is required but MSBuild not found in PATH
    echo Please run this script from a Visual Studio Developer Command Prompt
    exit /b 1
)

echo Build Configuration:
echo   Build Type: %BUILD_TYPE%
if defined ONNXRUNTIME_ROOT_DIR (
    echo   ONNX Runtime: %ONNXRUNTIME_ROOT_DIR%
) else (
    echo   ONNX Runtime: Not specified
    echo.
    echo Warning: ONNXRUNTIME_ROOT_DIR not set. Plugin will compile without ML inference.
    set /p CONTINUE="Continue without ONNX Runtime? (y/N): "
    if /i not "%CONTINUE%"=="y" exit /b 1
)
echo.

REM Create build directory
echo Creating build directory...
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
set CMAKE_ARGS=-G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if defined ONNXRUNTIME_ROOT_DIR (
    set CMAKE_ARGS=%CMAKE_ARGS% -DONNXRUNTIME_ROOT_DIR="%ONNXRUNTIME_ROOT_DIR%"
)

if defined LibObs_DIR (
    set CMAKE_ARGS=%CMAKE_ARGS% -DLibObs_DIR="%LibObs_DIR%"
)

cmake .. %CMAKE_ARGS%
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build
echo Building...
cmake --build . --config %BUILD_TYPE% --parallel
if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo To install the plugin, run:
echo   cmake --install . --config %BUILD_TYPE%
echo.
echo Don't forget to:
echo   1. Place the ONNX model file in the data/models directory
echo   2. Copy required DLLs (ONNX Runtime, OpenCV) to the plugin directory
echo   3. Restart OBS Studio

