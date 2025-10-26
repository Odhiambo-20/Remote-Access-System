#!/bin/bash

# Start Relay Server Script
# This script starts the Remote Access Relay Server

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
echo "Remote Access Relay Server"
echo "============================================"
echo ""

# Configuration
BINARY="/usr/local/bin/relay_server"
CONFIG_FILE="/etc/remote-access/relay_server.conf"
PID_FILE="/var/run/remote-access/relay_server.pid"
LOG_FILE="/var/log/remote-access/relay_server.log"

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
    print_error "Relay server binary not found: $BINARY"
    print_info "Please run: sudo ./install.sh"
    exit 1
fi

# Check if already running
if [ -f "$PID_FILE" ]; then
    OLD_PID=$(cat "$PID_FILE")
    if ps -p "$OLD_PID" > /dev/null 2>&1; then
        print_error "Relay server is already running (PID: $OLD_PID)"
        print_info "Stop it first with: sudo systemctl stop remote-access-relay-server"
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

# Display configuration
print_info "Server Configuration:"
echo "  Binary: $BINARY"
echo "  Config: $CONFIG_FILE"
echo "  Log File: $LOG_FILE"
echo "  PID File: $PID_FILE"
echo ""

# Check port availability
IPV4_PORT=$(grep "^ipv4_port=" "$CONFIG_FILE" 2>/dev/null | cut -d'=' -f2 || echo "3000")
IPV6_PORT=$(grep "^ipv6_port=" "$CONFIG_FILE" 2>/dev/null | cut -d'=' -f2 || echo "3000")

print_info "Checking ports..."

if netstat -tuln 2>/dev/null | grep -q ":$IPV4_PORT "; then
    print_error "IPv4 port $IPV4_PORT is already in use"
    print_info "Check with: sudo netstat -tulpn | grep $IPV4_PORT"
    exit 1
else
    print_success "IPv4 port $IPV4_PORT is available"
fi

if [ "$IPV4_PORT" != "$IPV6_PORT" ]; then
    if netstat -tuln 2>/dev/null | grep -q ":$IPV6_PORT "; then
        print_error "IPv6 port $IPV6_PORT is already in use"
        print_info "Check with: sudo netstat -tulpn | grep $IPV6_PORT"
        exit 1
    else
        print_success "IPv6 port $IPV6_PORT is available"
    fi
fi

# Check network interfaces
print_info "Checking network interfaces..."
IPV4_AVAILABLE=false
IPV6_AVAILABLE=false

if ip addr show | grep -q "inet "; then
    IPV4_AVAILABLE=true
    print_success "IPv4 available"
fi

if ip addr show | grep -q "inet6 " | grep -v "::1/128"; then
    IPV6_AVAILABLE=true
    print_success "IPv6 available"
fi

if [ "$IPV4_AVAILABLE" = false ] && [ "$IPV6_AVAILABLE" = false ]; then
    print_error "No network interfaces available"
    exit 1
fi

# Check firewall
print_info "Checking firewall..."
if command -v ufw &> /dev/null; then
    if ufw status | grep -q "Status: active"; then
        if ! ufw status | grep -q "$IPV4_PORT"; then
            print_info "Firewall is active but port $IPV4_PORT is not allowed"
            read -p "Allow port $IPV4_PORT through firewall? (y/N): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                ufw allow "$IPV4_PORT/tcp"
                print_success "Port $IPV4_PORT allowed through firewall"
            fi
        fi
    fi
fi

# Start the server
echo ""
print_status "Starting Relay Server..."

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
        print_success "Relay server started successfully (PID: $SERVER_PID)"
        echo ""
        print_info "Server Status:"
        echo "  PID: $SERVER_PID"
        echo "  IPv4 Port: $IPV4_PORT"
        echo "  IPv6 Port: $IPV6_PORT"
        echo "  Log: $LOG_FILE"
        echo ""
        print_info "View logs: sudo tail -f $LOG_FILE"
        print_info "Check status: sudo systemctl status remote-access-relay-server"
        print_info "Stop server: sudo systemctl stop remote-access-relay-server"
        
        # Show listening ports
        echo ""
        print_info "Listening on:"
        netstat -tuln 2>/dev/null | grep ":$IPV4_PORT " | sed 's/^/  /'
        
    else
        print_error "Failed to start relay server"
        rm -f "$PID_FILE"
        print_info "Check logs: sudo tail -n 50 $LOG_FILE"
        exit 1
    fi
    
else
    # Use systemd service
    print_info "Starting via systemd..."
    systemctl start remote-access-relay-server
    
    # Wait for service to start
    sleep 2
    
    if systemctl is-active --quiet remote-access-relay-server; then
        print_success "Relay server started successfully"
        echo ""
        systemctl status remote-access-relay-server --no-pager
        echo ""
        print_info "View logs: sudo journalctl -u remote-access-relay-server -f"
        print_info "Stop server: sudo systemctl stop remote-access-relay-server"
        
        # Show listening ports
        echo ""
        print_info "Listening on:"
        netstat -tuln 2>/dev/null | grep ":$IPV4_PORT " | sed 's/^/  /'
        
    else
        print_error "Failed to start relay server"
        print_info "Check logs: sudo journalctl -u remote-access-relay-server -n 50"
        exit 1
    fi
fi

# Display connection information
echo ""
print_info "Connection Information:"
echo ""
echo "  For PC clients:"
echo "    relay_server=<your-server-ip>:$IPV4_PORT"
echo ""
echo "  For mobile apps:"
echo "    Relay Server: <your-server-ip>"
echo "    Port: $IPV4_PORT"
echo ""

# Show server IP addresses
print_info "Server IP Addresses:"
ip addr show | grep "inet " | grep -v "127.0.0.1" | awk '{print "  IPv4: " $2}' | cut -d'/' -f1
ip addr show | grep "inet6 " | grep -v "::1/128" | grep -v "fe80:" | awk '{print "  IPv6: " $2}' | cut -d'/' -f1

echo ""
print_success "Relay server is running!"

# Optional: Test connection
echo ""
read -p "Test connection to relay server? (y/N): " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_info "Testing connection..."
    
    if timeout 5 bash -c "cat < /dev/null > /dev/tcp/127.0.0.1/$IPV4_PORT" 2>/dev/null; then
        print_success "Connection test successful!"
        echo "  Server is accepting connections on port $IPV4_PORT"
    else
        print_error "Connection test failed"
        echo "  Server may not be listening or firewall is blocking"
        print_info "Check with: sudo netstat -tulpn | grep $IPV4_PORT"
    fi
fi

echo ""