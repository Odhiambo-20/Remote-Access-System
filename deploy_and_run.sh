#!/bin/bash

# Remote Access System - Deploy and Run Script
# This script deploys built binaries to a runtime directory and starts all components

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR"

# Define paths
RUNTIME_DIR="$HOME/.local/share/remote-access-system"
RELAY_SERVER="$PROJECT_ROOT/relay-server/build/relay_server"
PC_CLIENT="$PROJECT_ROOT/pc-client/build/pc_client"
MOBILE_APP="$PROJECT_ROOT/mobile-app/build/RemoteAccessMobile"

# PID file locations
RELAY_PID="$RUNTIME_DIR/relay_server.pid"
PC_CLIENT_PID="$RUNTIME_DIR/pc_client.pid"
MOBILE_APP_PID="$RUNTIME_DIR/mobile_app.pid"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Remote Access System Deployer${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to check if a process is running
is_running() {
    local pid_file=$1
    if [ -f "$pid_file" ]; then
        local pid=$(cat "$pid_file")
        if ps -p "$pid" > /dev/null 2>&1; then
            return 0
        fi
    fi
    return 1
}

# Function to stop all components
stop_all() {
    echo -e "${YELLOW}Stopping all components...${NC}"
    
    # Stop by PID files first
    if is_running "$RELAY_PID"; then
        echo "Stopping relay server (PID file)..."
        kill $(cat "$RELAY_PID") 2>/dev/null || true
        rm -f "$RELAY_PID"
    fi
    
    if is_running "$PC_CLIENT_PID"; then
        echo "Stopping PC client (PID file)..."
        kill $(cat "$PC_CLIENT_PID") 2>/dev/null || true
        rm -f "$PC_CLIENT_PID"
    fi
    
    if is_running "$MOBILE_APP_PID"; then
        echo "Stopping mobile app (PID file)..."
        kill $(cat "$MOBILE_APP_PID") 2>/dev/null || true
        rm -f "$MOBILE_APP_PID"
    fi
    
    # Kill by process name as fallback
    pkill -9 relay_server 2>/dev/null || true
    pkill -9 pc_client 2>/dev/null || true
    pkill -9 RemoteAccessMobile 2>/dev/null || true
    
    # Force kill anything using port 2810 (relay server port)
    local port_pids=$(lsof -ti:2810 2>/dev/null)
    if [ ! -z "$port_pids" ]; then
        echo "Killing processes on port 2810..."
        echo "$port_pids" | xargs kill -9 2>/dev/null || true
    fi
    
    # Wait a moment for processes to die
    sleep 1
    
    echo -e "${GREEN}All components stopped${NC}"
}

# Function to check build status
check_builds() {
    echo -e "${BLUE}Checking if components are built...${NC}"
    
    local all_built=true
    
    if [ ! -f "$RELAY_SERVER" ]; then
        echo -e "${RED}✗ Relay server not built${NC}"
        all_built=false
    else
        echo -e "${GREEN}✓ Relay server built${NC}"
    fi
    
    if [ ! -f "$PC_CLIENT" ]; then
        echo -e "${RED}✗ PC client not built${NC}"
        all_built=false
    else
        echo -e "${GREEN}✓ PC client built${NC}"
    fi
    
    if [ ! -f "$MOBILE_APP" ]; then
        echo -e "${RED}✗ Mobile app not built${NC}"
        all_built=false
    else
        echo -e "${GREEN}✓ Mobile app built${NC}"
    fi
    
    if [ "$all_built" = false ]; then
        echo ""
        echo -e "${YELLOW}Some components are not built. Run ./build_and_run.sh first.${NC}"
        return 1
    fi
    
    echo ""
    return 0
}

# Function to deploy binaries
deploy() {
    echo -e "${BLUE}Deploying binaries to runtime directory...${NC}"
    
    # Create runtime directory
    mkdir -p "$RUNTIME_DIR/bin"
    mkdir -p "$RUNTIME_DIR/logs"
    
    # Copy binaries
    echo "Copying relay server..."
    cp "$RELAY_SERVER" "$RUNTIME_DIR/bin/"
    
    echo "Copying PC client..."
    cp "$PC_CLIENT" "$RUNTIME_DIR/bin/"
    
    echo "Copying mobile app..."
    cp "$MOBILE_APP" "$RUNTIME_DIR/bin/"
    
    # Make them executable
    chmod +x "$RUNTIME_DIR/bin/relay_server"
    chmod +x "$RUNTIME_DIR/bin/pc_client"
    chmod +x "$RUNTIME_DIR/bin/RemoteAccessMobile"
    
    echo -e "${GREEN}✓ Deployment complete${NC}"
    echo ""
}

# Function to start components
start_all() {
    echo -e "${BLUE}Starting all components...${NC}"
    echo ""
    
    # Start relay server
    echo -e "${YELLOW}Starting relay server...${NC}"
    cd "$RUNTIME_DIR"
    nohup "$RUNTIME_DIR/bin/relay_server" > "$RUNTIME_DIR/logs/relay_server.log" 2>&1 &
    echo $! > "$RELAY_PID"
    sleep 2
    
    if is_running "$RELAY_PID"; then
        echo -e "${GREEN}✓ Relay server started (PID: $(cat $RELAY_PID))${NC}"
    else
        echo -e "${RED}✗ Failed to start relay server${NC}"
        cat "$RUNTIME_DIR/logs/relay_server.log"
        return 1
    fi
    echo ""
    
    # Start PC client
    echo -e "${YELLOW}Starting PC client...${NC}"
    nohup "$RUNTIME_DIR/bin/pc_client" > "$RUNTIME_DIR/logs/pc_client.log" 2>&1 &
    echo $! > "$PC_CLIENT_PID"
    sleep 2
    
    if is_running "$PC_CLIENT_PID"; then
        echo -e "${GREEN}✓ PC client started (PID: $(cat $PC_CLIENT_PID))${NC}"
    else
        echo -e "${RED}✗ Failed to start PC client${NC}"
        cat "$RUNTIME_DIR/logs/pc_client.log"
        return 1
    fi
    echo ""
    
    # Start mobile app
    echo -e "${YELLOW}Starting mobile app...${NC}"
    nohup "$RUNTIME_DIR/bin/RemoteAccessMobile" > "$RUNTIME_DIR/logs/mobile_app.log" 2>&1 &
    echo $! > "$MOBILE_APP_PID"
    sleep 2
    
    if is_running "$MOBILE_APP_PID"; then
        echo -e "${GREEN}✓ Mobile app started (PID: $(cat $MOBILE_APP_PID))${NC}"
    else
        echo -e "${RED}✗ Failed to start mobile app${NC}"
        cat "$RUNTIME_DIR/logs/mobile_app.log"
        return 1
    fi
    
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}  All components started successfully!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "Runtime directory: ${BLUE}$RUNTIME_DIR${NC}"
    echo -e "Logs directory:    ${BLUE}$RUNTIME_DIR/logs${NC}"
    echo ""
    echo -e "To view logs:"
    echo -e "  Relay server: tail -f $RUNTIME_DIR/logs/relay_server.log"
    echo -e "  PC client:    tail -f $RUNTIME_DIR/logs/pc_client.log"
    echo -e "  Mobile app:   tail -f $RUNTIME_DIR/logs/mobile_app.log"
    echo ""
    echo -e "To stop all:  $0 stop"
}

# Function to show status
status() {
    echo -e "${BLUE}Component Status:${NC}"
    echo ""
    
    if is_running "$RELAY_PID"; then
        echo -e "Relay server:  ${GREEN}RUNNING${NC} (PID: $(cat $RELAY_PID))"
    else
        echo -e "Relay server:  ${RED}STOPPED${NC}"
    fi
    
    if is_running "$PC_CLIENT_PID"; then
        echo -e "PC client:     ${GREEN}RUNNING${NC} (PID: $(cat $PC_CLIENT_PID))"
    else
        echo -e "PC client:     ${RED}STOPPED${NC}"
    fi
    
    if is_running "$MOBILE_APP_PID"; then
        echo -e "Mobile app:    ${GREEN}RUNNING${NC} (PID: $(cat $MOBILE_APP_PID))"
    else
        echo -e "Mobile app:    ${RED}STOPPED${NC}"
    fi
    echo ""
}

# Function to show logs
logs() {
    local component=$1
    case $component in
        relay)
            tail -f "$RUNTIME_DIR/logs/relay_server.log"
            ;;
        pc)
            tail -f "$RUNTIME_DIR/logs/pc_client.log"
            ;;
        mobile)
            tail -f "$RUNTIME_DIR/logs/mobile_app.log"
            ;;
        *)
            echo "Usage: $0 logs [relay|pc|mobile]"
            ;;
    esac
}

# Main script logic
case "${1:-start}" in
    start)
        check_builds || exit 1
        stop_all
        deploy
        start_all
        ;;
    stop)
        stop_all
        ;;
    restart)
        stop_all
        sleep 2
        check_builds || exit 1
        deploy
        start_all
        ;;
    status)
        status
        ;;
    logs)
        logs "$2"
        ;;
    deploy)
        check_builds || exit 1
        deploy
        ;;
    *)
        echo "Usage: $0 {start|stop|restart|status|logs|deploy}"
        echo ""
        echo "Commands:"
        echo "  start   - Deploy and start all components (default)"
        echo "  stop    - Stop all running components"
        echo "  restart - Stop and restart all components"
        echo "  status  - Show status of all components"
        echo "  logs    - View logs (usage: $0 logs [relay|pc|mobile])"
        echo "  deploy  - Deploy binaries without starting"
        exit 1
        ;;
esac


