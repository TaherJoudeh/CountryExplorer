#!/bin/bash

echo "================================================"
echo "Country Explorer - Dependency Setup (Linux/Mac)"
echo "================================================"
echo ""

cd "$(dirname "$0")"

# Create libs directory
mkdir -p libs
cd libs

echo "[1/4] Downloading cpp-httplib..."
if [ ! -d "httplib" ]; then
    git clone --depth 1 https://github.com/yhirose/cpp-httplib.git httplib
    echo "cpp-httplib downloaded successfully!"
else
    echo "cpp-httplib already exists, skipping..."
fi
echo ""

echo "[2/4] Downloading nlohmann/json..."
if [ ! -d "json" ]; then
    git clone --depth 1 --branch v3.11.3 https://github.com/nlohmann/json.git json
    echo "nlohmann/json downloaded successfully!"
else
    echo "nlohmann/json already exists, skipping..."
fi
echo ""

echo "[3/4] Downloading Dear ImGui..."
if [ ! -d "imgui" ]; then
    git clone --depth 1 --branch v1.90.1 https://github.com/ocornut/imgui.git imgui
    echo "Dear ImGui downloaded successfully!"
else
    echo "Dear ImGui already exists, skipping..."
fi
echo ""

echo "[4/4] Downloading GLFW..."
if [ ! -d "glfw" ]; then
    git clone --depth 1 --branch 3.3.9 https://github.com/glfw/glfw.git glfw
    echo "GLFW downloaded successfully!"
else
    echo "GLFW already exists, skipping..."
fi
echo ""

cd ..

echo "================================================"
echo "Setup Complete!"
echo "================================================"
echo ""
echo "Next steps:"
echo "  1. Install system dependencies (if needed):"
echo "     Ubuntu/Debian: sudo apt-get install build-essential cmake libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev"
echo "     Fedora: sudo dnf install cmake gcc-c++ mesa-libGL-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel"
echo "     macOS: brew install cmake (Xcode Command Line Tools required)"
echo ""
echo "  2. Build the project:"
echo "     mkdir build && cd build"
echo "     cmake .."
echo "     make -j4"
echo ""
echo "  3. Run:"
echo "     ./bin/CountryExplorer"
echo ""
