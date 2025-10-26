#!/bin/bash

echo "=========================================="
echo "  Remote Access System Startup"
echo "=========================================="

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Check if relay server is running
if ! pgrep -x "relay_server" > /dev/null; then
    echo "Starting relay server..."
    sudo "$SCRIPT_DIR/build/relay-server/relay_server" &
    sleep 2
fi

# Check if account server is running
if ! pgrep -x "account_server" > /dev/null; then
    echo "Starting account server..."
    sudo "$SCRIPT_DIR/build/account-server/account-server" &
    sleep 2
fi

echo ""
echo "Servers started!"
echo ""
echo "To start PC client:"
echo "  cd $SCRIPT_DIR/pc-client/build && ./pc-client"
echo ""
echo "To start Mobile app:"
echo "  cd $SCRIPT_DIR/mobile-app/build && ./RemoteAccessMobile"
echo ""
