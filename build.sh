#!/bin/bash

echo "Starting build process..."

# Install dependencies
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libxtst-dev \
    libssl-dev \
    uuid-dev \
    qt6-base-dev \
    libqt6core6 \
    libqt6gui6 \
    libqt6network6

# Create build directory
mkdir -p build
cd build

# Configure and build
echo "Configuring with CMake..."
cmake ..

echo "Building..."
make -j$(nproc)

echo "Build complete! Binaries in ./build/bin/"
ls -la bin/
