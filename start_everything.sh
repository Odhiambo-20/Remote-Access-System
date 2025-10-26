#!/bin/bash

# MASTER STARTUP SCRIPT
# Starts EVERYTHING: Account Server + Relay Server + PC Client + Mobile App Build

echo "🚀🚀🚀 STARTING COMPLETE REMOTE ACCESS SYSTEM 🚀🚀🚀"
echo "========================================================"

# ============================================
# STEP 1: Start Account Server
# ============================================
echo ""
echo "📡 [1/5] Starting Account Server..."
sudo systemctl start remote-access-account-server
sleep 1

if systemctl is-active --quiet remote-access-account-server; then
    echo "✅ Account Server: RUNNING on port 2809"
else
    echo "❌ Account Server: FAILED"
    sudo systemctl status remote-access-account-server --no-pager
    exit 1
fi

# ============================================
# STEP 2: Start Relay Server
# ============================================
echo ""
echo "🔄 [2/5] Starting Relay Server..."
sudo systemctl start remote-access-relay-server
sleep 1

if systemctl is-active --quiet remote-access-relay-server; then
    echo "✅ Relay Server: RUNNING on port 2810"
else
    echo "❌ Relay Server: FAILED"
    sudo systemctl status remote-access-relay-server --no-pager
    exit 1
fi

# ============================================
# STEP 3: Check USB Drive
# ============================================
echo ""
echo "💾 [3/5] Checking USB Drive..."

USB_MOUNT="/media/victor/KALI LINUX"
USB_CONFIG="$USB_MOUNT/.remote_access"

if [ -d "$USB_MOUNT" ]; then
    echo "✅ USB Drive: MOUNTED at $USB_MOUNT"
    
    if [ -f "$USB_CONFIG/usb_id.txt" ]; then
        USB_ID=$(cat "$USB_CONFIG/usb_id.txt")
        echo "✅ USB ID: $USB_ID"
    else
        echo "❌ USB configuration not found!"
        exit 1
    fi
else
    echo "❌ USB Drive: NOT MOUNTED"
    echo "   Please insert your USB drive!"
    exit 1
fi

# ============================================
# STEP 4: Start PC Client
# ============================================
echo ""
echo "💻 [4/5] Starting PC Client..."
echo "   (This will run in foreground - Press Ctrl+C to stop)"
echo ""

# Option A: Run in foreground
# /usr/local/bin/pc_client --usb-id="$USB_ID" --config-dir="$USB_CONFIG"

# Option B: Run in background (uncomment to use)
nohup /usr/local/bin/pc_client --usb-id="$USB_ID" --config-dir="$USB_CONFIG" > ~/pc_client.log 2>&1 &
PC_CLIENT_PID=$!
sleep 2

if ps -p $PC_CLIENT_PID > /dev/null; then
    echo "✅ PC Client: RUNNING (PID: $PC_CLIENT_PID)"
    echo "   Logs: ~/pc_client.log"
else
    echo "❌ PC Client: FAILED"
    cat ~/pc_client.log
    exit 1
fi

# ============================================
# STEP 5: Mobile App Info
# ============================================
echo ""
echo "📱 [5/5] Mobile App Status..."
echo ""

MOBILE_APP_DIR="$HOME/Documents/Desktop/remote-access-system/mobile-app"

if [ -d "$MOBILE_APP_DIR" ]; then
    if [ -f "$MOBILE_APP_DIR/build/remote_access_mobile" ]; then
        echo "✅ Mobile App: BUILT"
        echo "   Location: $MOBILE_APP_DIR/build/"
        echo "   To deploy to Android: Use Qt Creator or androiddeployqt"
        echo "   To deploy to iOS: Use Qt Creator with Xcode"
    else
        echo "⚠️  Mobile App: NOT BUILT"
        echo "   Build it with:"
        echo "   cd $MOBILE_APP_DIR"
        echo "   mkdir -p build && cd build"
        echo "   cmake .. -DCMAKE_BUILD_TYPE=Release"
        echo "   make -j\$(nproc)"
    fi
else
    echo "⚠️  Mobile App: Source not found at $MOBILE_APP_DIR"
fi

# ============================================
# SYSTEM STATUS SUMMARY
# ============================================
echo ""
echo "========================================================"
echo "✅✅✅ REMOTE ACCESS SYSTEM FULLY OPERATIONAL ✅✅✅"
echo "========================================================"
echo ""
echo "📊 SYSTEM STATUS:"
echo "  ✅ Account Server: Running on localhost:2809"
echo "  ✅ Relay Server:   Running on localhost:2810"
echo "  ✅ PC Client:      Running (PID: $PC_CLIENT_PID)"
echo "  �� USB Drive:      $USB_MOUNT"
echo "  🆔 PC ID:          $(cat "$USB_CONFIG/pc_id.txt" 2>/dev/null || echo 'N/A')"
echo ""
echo "🎮 QUICK COMMANDS:"
echo "  • View PC Client logs:  tail -f ~/pc_client.log"
echo "  • Stop PC Client:       kill $PC_CLIENT_PID"
echo "  • Stop servers:         sudo systemctl stop remote-access-*"
echo "  • Check status:         sudo systemctl status remote-access-*"
echo ""
echo "📱 MOBILE APP CONNECTION:"
echo "  1. Open mobile app"
echo "  2. Configure server: localhost or $(hostname -I | awk '{print $1}')"
echo "  3. Login with your username/password"
echo "  4. Connect to PC ID: $(cat "$USB_CONFIG/pc_id.txt" 2>/dev/null || echo 'N/A')"
echo ""
echo "🔥 System is ready! Keep this terminal open or run PC Client in background."
echo "========================================================"


