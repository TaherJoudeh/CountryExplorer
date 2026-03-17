@echo off
echo ================================================
echo Country Explorer - Dependency Setup (Windows)
echo ================================================
echo.

cd /d "%~dp0"

REM Create libs directory
if not exist "libs" mkdir libs
cd libs

echo [1/4] Downloading cpp-httplib...
if not exist "httplib" (
    git clone --depth 1 https://github.com/yhirose/cpp-httplib.git httplib
    echo cpp-httplib downloaded successfully!
) else (
    echo cpp-httplib already exists, skipping...
)
echo.

echo [2/4] Downloading nlohmann/json...
if not exist "json" (
    git clone --depth 1 --branch v3.11.3 https://github.com/nlohmann/json.git json
    echo nlohmann/json downloaded successfully!
) else (
    echo nlohmann/json already exists, skipping...
)
echo.

echo [3/4] Downloading Dear ImGui...
if not exist "imgui" (
    git clone --depth 1 --branch v1.90.1 https://github.com/ocornut/imgui.git imgui
    echo Dear ImGui downloaded successfully!
) else (
    echo Dear ImGui already exists, skipping...
)
echo.

echo [4/4] Downloading GLFW...
if not exist "glfw" (
    git clone --depth 1 --branch 3.3.9 https://github.com/glfw/glfw.git glfw
    echo GLFW downloaded successfully!
) else (
    echo GLFW already exists, skipping...
)
echo.

cd ..

echo ================================================
echo Setup Complete!
echo ================================================
echo.
echo Next steps:
echo   1. Create build directory: mkdir build
echo   2. Navigate to build: cd build
echo   3. Run CMake: cmake ..
echo   4. Build project: cmake --build . --config Release
echo.
echo Or use Visual Studio:
echo   1. Open CMakeLists.txt in Visual Studio
echo   2. Build the project (F7)
echo.
pause
