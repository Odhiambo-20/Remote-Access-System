#!/bin/bash

# Start Account Server Script
# This script starts the Remote Access Account Server

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

print_status() {
    echo -e "${BLUE}[STATUS]${NC} $1"
}

echo "============================================"
echo "Remote Access Account Server"
echo "============================================"
echo ""

# Configuration
BINARY="/usr/local/bin/account_server"
CONFIG_FILE="/etc/remote-access/account_server.conf"
PID_FILE="/var/run/remote-access/account_server.pid"
LOG_FILE="/var/log/remote-access/account_server.log"
DB_FILE="/var/lib/remote-access/accounts.db"

# Parse command line arguments
DAEMON_MODE=false
FOREGROUND=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--daemon)
            DAEMON_MODE=true
            shift
            ;;
        -f|--foreground)
            FOREGROUND=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -d, --daemon      Run as daemon (background)"
            echo "  -f, --foreground  Run in foreground"
            echo "  -v, --verbose     Enable verbose output"
            echo "  -h, --help        Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 -d             # Run as daemon"
            echo "  $0 -f -v          # Run in foreground with verbose output"
            echo ""
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root"
    print_info "Please run: sudo $0"
    exit 1
fi

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    print_error "Account server binary not found: $BINARY"
    print_info "Please run: sudo ./install.sh"
    exit 1
fi

# Check if database exists
if [ ! -f "$DB_FILE" ]; then
    print_error "Database not found: $DB_FILE"
    print_info "Please run: sudo ./scripts/setup_database.sh"
    exit 1
fi

# Check if already running
if [ -f "$PID_FILE" ]; then
    OLD_PID=$(cat "$PID_FILE")
    if ps -p "$OLD_PID" > /dev/null 2>&1; then
        print_error "Account server is already running (PID: $OLD_PID)"
        print_info "Stop it first with: sudo systemctl stop remote-access-account-server"
        print_info "Or kill it with: sudo kill $OLD_PID"
        exit 1
    else
        print_info "Removing stale PID file"
        rm -f "$PID_FILE"
    fi
fi

# Create run directory
mkdir -p "$(dirname $PID_FILE)"
mkdir -p "$(dirname $LOG_FILE)"

# Verify database integrity
print_info "Verifying database..."
if sqlite3 "$DB_FILE" "PRAGMA integrity_check;" | grep -q "ok"; then
    print_success "Database integrity check passed"
else
    print_error "Database integrity check failed"
    print_info "Try running: sudo ./scripts/setup_database.sh"
    exit 1
fi

# Check database permissions
if [ ! -r "$DB_FILE" ] || [ ! -w "$DB_FILE" ]; then
    print_error "Insufficient database permissions"
    print_info "Fixing permissions..."
    chmod 640 "$DB_FILE"
    chown root:root "$DB_FILE"
    print_success "Permissions fixed"
fi

# Display configuration
print_info "Server Configuration:"
echo "  Binary: $BINARY"
echo "  Config: $CONFIG_FILE"
echo "  Database: $DB_FILE"
echo "  Log File: $LOG_FILE"
echo "  PID File: $PID_FILE"
echo ""

# Check port availability
CONFIG_PORT=$(grep "^port=" "$CONFIG_FILE" 2>/dev/null | cut -d'=' -f2 || echo "2809")
print_info "Checking port $CONFIG_PORT..."

if netstat -tuln 2>/dev/null | grep -q ":$CONFIG_PORT "; then
    print_error "Port $CONFIG_PORT is already in use"
    print_info "Check with: sudo netstat -tulpn | grep $CONFIG_PORT"
    exit 1
else
    print_success "Port $CONFIG_PORT is available"
fi

# Start the server
echo ""
print_status "Starting Account Server..."

if [ "$FOREGROUND" = true ]; then
    # Run in foreground
    print_info "Running in foreground mode (Ctrl+C to stop)"
    echo ""
    
    if [ "$VERBOSE" = true ]; then
        exec "$BINARY" --config="$CONFIG_FILE" --verbose
    else
        exec "$BINARY" --config="$CONFIG_FILE"
    fi
    
elif [ "$DAEMON_MODE" = true ]; then
    # Run as daemon
    print_info "Starting as daemon..."
    
    if [ "$VERBOSE" = true ]; then
        nohup "$BINARY" --config="$CONFIG_FILE" --verbose >> "$LOG_FILE" 2>&1 &
    else
        nohup "$BINARY" --config="$CONFIG_FILE" >> "$LOG_FILE" 2>&1 &
    fi
    
    SERVER_PID=$!
    echo "$SERVER_PID" > "$PID_FILE"
    
    # Wait a moment and check if it started successfully
    sleep 2
    
    if ps -p "$SERVER_PID" > /dev/null 2>&1; then
        print_success "Account server started successfully (PID: $SERVER_PID)"
        echo ""
        print_info "Server Status:"
        echo "  PID: $SERVER_PID"
        echo "  Port: $CONFIG_PORT"
        echo "  Log: $LOG_FILE"
        echo ""
        print_info "View logs: sudo tail -f $LOG_FILE"
        print_info "Check status: sudo systemctl status remote-access-account-server"
        print_info "Stop server: sudo systemctl stop remote-access-account-server"
    else
        print_error "Failed to start account server"
        rm -f "$PID_FILE"
        print_info "Check logs: sudo tail -n 50 $LOG_FILE"
        exit 1
    fi
    
else
    # Use systemd service
    print_info "Starting via systemd..."
    systemctl start remote-access-account-server
    
    # Wait for service to start
    sleep 2
    
    if systemctl is-active --quiet remote-access-account-server; then
        print_success "Account server started successfully"
        echo ""
        systemctl status remote-access-account-server --no-pager
        echo ""
        print_info "View logs: sudo journalctl -u remote-access-account-server -f"
        print_info "Stop server: sudo systemctl stop remote-access-account-server"
    else
        print_error "Failed to start account server"
        print_info "Check logs: sudo journalctl -u remote-access-account-server -n 50"
        exit 1
    fi
fi

echo ""
print_success "Account server is running!"