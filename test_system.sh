#!/bin/bash

echo "=========================================="
echo "  Testing Remote Access System"
echo "=========================================="

# Start servers
./start_system.sh

sleep 3

# Start PC client in background
echo ""
echo "Starting PC client..."
cd pc-client/build
./pc-client &
PC_PID=$!
cd ../..

sleep 3

# Start mobile app
echo ""
echo "Starting Mobile app..."
cd mobile-app/build
./RemoteAccessMobile

# Cleanup
echo ""
echo "Cleaning up..."
kill $PC_PID 2>/dev/null

echo "Test complete!"
