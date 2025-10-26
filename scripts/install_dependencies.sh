#!/bin/bash

# Install all dependencies for Remote Access System on Debian

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
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

echo "============================================"
echo "Installing Dependencies"
echo "Remote Access System for Debian Linux"
echo "============================================"
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root"
    print_info "Please run: sudo $0"
    exit 1
fi

# Detect Debian/Ubuntu version
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_NAME=$NAME
    OS_VERSION=$VERSION_ID
    print_info "Detected: $OS_NAME $OS_VERSION"
else
    print_error "Cannot detect OS version"
    exit 1
fi

# Update package list
print_info "Updating package list..."
apt-get update -qq

# Install build tools
print_info "Installing build tools..."
apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    wget \
    curl \
    autoconf \
    automake \
    libtool \
    make \
    g++ \
    gcc

print_success "Build tools installed"

# Install development libraries
print_info "Installing development libraries..."
apt-get install -y \
    libssl-dev \
    libsqlite3-dev \
    libpthread-stubs0-dev \
    uuid-dev \
    zlib1g-dev

print_success "Development libraries installed"

# Install ACE+TAO
print_info "Checking for ACE+TAO..."

if [ ! -d "/usr/include/ace" ] && [ ! -d "/usr/local/include/ace" ]; then
    print_info "ACE+TAO not found. Installing..."
    
    # Try to install from package manager first
    if apt-cache search libace-dev | grep -q libace-dev; then
        print_info "Installing ACE+TAO from package manager..."
        apt-get install -y libace-dev libace-6.* || apt-get install -y libace-dev
        print_success "ACE installed from package manager"
    else
        print_info "ACE+TAO not available in package manager"
        print_info "Installing from source..."
        
        # Download and compile ACE
        ACE_VERSION="7.0.6"
        cd /tmp
        
        if [ ! -f "ACE-${ACE_VERSION}.tar.gz" ]; then
            wget "https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-${ACE_VERSION//./_}/ACE-${ACE_VERSION}.tar.gz" || \
            wget "http://download.dre.vanderbilt.edu/previous_versions/ACE-${ACE_VERSION}.tar.gz"
        fi
        
        tar -xzf "ACE-${ACE_VERSION}.tar.gz"
        cd ACE_wrappers
        
        export ACE_ROOT=$(pwd)
        
        # Configure ACE
        cat > ace/config.h << EOF
#include "ace/config-linux.h"
EOF
        
        cat > include/makeinclude/platform_macros.GNU << EOF
include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU
EOF
        
        # Build ACE
        cd ace
        make -j$(nproc)
        make install
        
        cd /tmp
        rm -rf ACE_wrappers "ACE-${ACE_VERSION}.tar.gz"
        
        print_success "ACE+TAO compiled and installed from source"
    fi
else
    print_success "ACE+TAO already installed"
fi

# Install Qt6 for mobile app (optional)
print_info "Checking for Qt6..."
if ! dpkg -l | grep -q qt6-base-dev; then
    read -p "Install Qt6 for mobile development? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_info "Installing Qt6 for mobile development..."
        apt-get install -y \
            qt6-base-dev \
            qt6-declarative-dev \
            qt6-tools-dev \
            qt6-tools-dev-tools \
            qml6-module-qtquick \
            qml6-module-qtquick-controls \
            qml6-module-qtquick-layouts \
            qml6-module-qtquick-window \
            libqt6svg6-dev \
            libqt6network6 2>/dev/null || print_info "Qt6 installation skipped (not critical for servers)"
    fi
else
    print_success "Qt6 already installed"
fi

# Install SQLite
print_info "Installing SQLite..."
apt-get install -y sqlite3 libsqlite3-dev

print_success "SQLite installed"

# Install PostgreSQL (optional, for production)
if ! dpkg -l | grep -q postgresql; then
    read -p "Install PostgreSQL for production use? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_info "Installing PostgreSQL..."
        apt-get install -y postgresql postgresql-contrib libpq-dev
        print_success "PostgreSQL installed"
        
        print_info "PostgreSQL has been installed but not configured"
        print_info "To configure PostgreSQL:"
        echo "  1. sudo -u postgres psql"
        echo "  2. CREATE DATABASE remote_access;"
        echo "  3. CREATE USER remote_access WITH PASSWORD 'your_password';"
        echo "  4. GRANT ALL PRIVILEGES ON DATABASE remote_access TO remote_access;"
    fi
fi

# Install additional utilities
print_info "Installing additional utilities..."
apt-get install -y \
    dmidecode \
    pciutils \
    usbutils \
    net-tools \
    iproute2 \
    netcat-openbsd \
    telnet \
    htop \
    iotop \
    vim \
    less

print_success "Additional utilities installed"

# Install system monitoring tools
print_info "Installing monitoring tools..."
apt-get install -y \
    sysstat \
    lsof \
    strace

print_success "Monitoring tools installed"

# Install security tools
print_info "Installing security tools..."
apt-get install -y \
    openssl \
    ca-certificates \
    fail2ban

print_success "Security tools installed"

# Configure fail2ban for SSH protection
if systemctl is-enabled fail2ban &>/dev/null; then
    print_info "Configuring fail2ban..."
    systemctl enable fail2ban
    systemctl start fail2ban
    print_success "fail2ban configured"
fi

# Install documentation tools
print_info "Installing documentation tools..."
apt-get install -y \
    man-db \
    manpages-dev

print_success "Documentation tools installed"

# Clean up
print_info "Cleaning up..."
apt-get autoremove -y
apt-get clean
print_success "Cleanup complete"

# Verify installations
echo ""
print_info "Verifying installations..."

MISSING_DEPS=()

# Check for essential tools
for cmd in gcc g++ cmake make pkg-config sqlite3 openssl; do
    if ! command -v $cmd &> /dev/null; then
        MISSING_DEPS+=("$cmd")
    fi
done

# Check for ACE
if [ ! -d "/usr/include/ace" ] && [ ! -d "/usr/local/include/ace" ]; then
    MISSING_DEPS+=("ACE library")
fi

# Check for OpenSSL development files
if ! pkg-config --exists openssl; then
    MISSING_DEPS+=("OpenSSL development files")
fi

# Check for SQLite development files
if ! pkg-config --exists sqlite3; then
    MISSING_DEPS+=("SQLite3 development files")
fi

if [ ${#MISSING_DEPS[@]} -eq 0 ]; then
    print_success "All essential dependencies verified"
else
    print_error "Missing dependencies: ${MISSING_DEPS[*]}"
    print_info "Please install missing dependencies manually"
    exit 1
fi

# Display installation summary
echo ""
echo "============================================"
echo "Dependency Installation Complete!"
echo "============================================"
echo ""

print_info "Installed Components:"
echo "  ✓ Build tools (gcc, g++, cmake, make)"
echo "  ✓ Development libraries (OpenSSL, SQLite3)"
echo "  ✓ ACE+TAO framework"
echo "  ✓ System utilities"
echo "  ✓ Security tools"
echo "  ✓ Monitoring tools"

# Check Qt installation
if dpkg -l | grep -q qt6-base-dev; then
    echo "  ✓ Qt6 (for mobile app)"
fi

# Check PostgreSQL installation
if dpkg -l | grep -q postgresql; then
    echo "  ✓ PostgreSQL"
fi

echo ""
print_info "System Information:"
echo "  OS: $OS_NAME $OS_VERSION"
echo "  Kernel: $(uname -r)"
echo "  Architecture: $(uname -m)"
echo "  CPU Cores: $(nproc)"
echo "  Memory: $(free -h | awk '/^Mem:/{print $2}')"
echo ""

print_info "Next Steps:"
echo "  1. Build the project:"
echo "     ./build.sh"
echo ""
echo "  2. Install the system:"
echo "     sudo ./install.sh"
echo ""
echo "  3. Setup database:"
echo "     sudo ./scripts/setup_database.sh"
echo ""
echo "  4. Generate certificates:"
echo "     sudo ./scripts/generate_certificates.sh"
echo ""
echo "  5. Start servers:"
echo "     sudo ./scripts/start_account_server.sh"
echo "     sudo ./scripts/start_relay_server.sh"
echo ""

print_success "All dependencies installed successfully!"