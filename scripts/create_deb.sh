#!/bin/bash

# Exit on any error
set -e

# Define build directory
BUILD_DIR="build-deb"

# Create build directory if it doesn't exist
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi
mkdir "$BUILD_DIR"

# Configure the project
echo "Configuring project in $BUILD_DIR..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
cmake --build "$BUILD_DIR" --config Release

# Generate the Debian package
echo "Generating Debian package..."
cd "$BUILD_DIR"
cpack -G DEB

echo "Debian package generated successfully in $BUILD_DIR/"
ls -l *.deb
