@echo off
echo PSX Horror Engine Build Script
echo ================================

cd /d "%~dp0"

if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

echo Cleaning previous build...
if exist CMakeCache.txt del CMakeCache.txt

echo Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build successful!

if "%1"=="run" (
    echo Running game...
    cd Release
    PSXHorrorEngine.exe
) else (
    echo Use "build.bat run" to build and run automatically
)

pause