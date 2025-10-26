#!/bin/bash

echo "Building Remote Access System..."

# Build relay server
echo "Building relay server..."
cd relay-server
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ../..

# Build PC client
echo "Building PC client..."
cd pc-client
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ../..

# Build mobile app
echo "Building mobile app..."
cd mobile-app
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ../..

echo "Build completed successfully!"
echo ""
echo "To run the system:"
echo "1. Start relay server: ./relay-server/build/relay_server"
echo "2. Start PC client: ./pc-client/build/pc_client"
echo "3. Start mobile app: ./mobile-app/build/RemoteAccessMobile"
