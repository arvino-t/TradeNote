#!/bin/bash

# Exit on any error
set -e

# Define build directory
BUILD_DIR="build-win"

# This script is intended to be run in a Windows environment with CMake, Qt, and NSIS installed.
# It can also be run in a cross-compilation environment if configured correctly.

# Create build directory if it doesn't exist
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi
mkdir "$BUILD_DIR"

# Configure the project
echo "Configuring project in $BUILD_DIR..."
# Note: For real Windows build, use -G "Visual Studio ..." or -G "MinGW Makefiles"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
cmake --build "$BUILD_DIR" --config Release

# Note: On Windows, you would typically run windeployqt here:
# windeployqt --release "$BUILD_DIR/TradeNote.exe"

# Generate the NSIS installer
echo "Generating Windows installer..."
cd "$BUILD_DIR"
# This will likely fail on Linux unless NSIS is installed and cross-compilation is set up
if command -v makensis &> /dev/null; then
    cpack -G NSIS
    echo "Windows installer generated successfully in $BUILD_DIR/"
    ls -l *.exe
else
    echo "makensis (NSIS) not found. Skipping installer generation."
    echo "You can still generate a ZIP package:"
    cpack -G ZIP
    echo "ZIP package generated successfully in $BUILD_DIR/"
    ls -l *.zip
fi
