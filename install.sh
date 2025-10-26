#!/bin/bash

# Remote Access System Installation Script
# Installs dependencies and builds the project for Linux/macOS
# Run as: ./install.sh

set -e

# Check for root privileges (needed for some dependencies)
if [ "$(id -u)" != "0" ]; then
    echo "This script requires root privileges for installing dependencies."
    echo "Please run with sudo or as root."
    exit 1
fi

# Install dependencies
echo "Installing dependencies..."
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Ubuntu/Debian
    apt-get update
    apt-get install -y build-essential cmake libssl-dev libsqlite3-dev libusb-1.0-0-dev
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS (using Homebrew)
    brew install ace tao openssl sqlite libusb
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
cmake ..

# Build the project
echo "Building the project..."
make -j$(nproc || sysctl -n hw.ncpu)

# Install binaries
echo "Installing binaries..."
make install

# Generate TLS certificates
echo "Generating TLS certificates..."
mkdir -p resources
openssl req -x509 -newkey rsa:4096 -keyout resources/server.key -out resources/server.crt -days 365 -nodes \
    -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost"

# Initialize SQLite database
echo "Initializing database..."
sqlite3 account.db < ../account-server/sql/schema.sql

echo "Installation complete. Binaries installed in /usr/local/bin."
echo "Database initialized at account.db."
echo "TLS certificates generated in resources/."