#!/bin/bash

set -e

if [ "$(id -u)" != "0" ]; then
    echo "This script requires root privileges."
    echo "Please run with sudo."
    exit 1
fi

echo "Installing Remote Access System components..."

# Create installation directories
mkdir -p /opt/remote-access-system/{bin,config,resources,data}
mkdir -p /etc/systemd/system

# Install binaries
echo "Installing binaries..."
cp build/usb-client/usb-client /opt/remote-access-system/bin/
cp build/pc-client/pc-client /opt/remote-access-system/bin/
cp build/account-server/account-server /opt/remote-access-system/bin/
cp build/relay-server/relay-server /opt/remote-access-system/bin/

# Set permissions
chmod +x /opt/remote-access-system/bin/*

# Generate TLS certificates if they don't exist
if [ ! -f /opt/remote-access-system/resources/server.crt ]; then
    echo "Generating TLS certificates..."
    openssl req -x509 -newkey rsa:4096 \
        -keyout /opt/remote-access-system/resources/server.key \
        -out /opt/remote-access-system/resources/server.crt \
        -days 365 -nodes \
        -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost"
    chmod 600 /opt/remote-access-system/resources/server.key
fi

# Create systemd service for account server
cat > /etc/systemd/system/remote-access-account.service << 'SERVICE'
[Unit]
Description=Remote Access Account Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/remote-access-system
ExecStart=/opt/remote-access-system/bin/account-server
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
SERVICE

# Create systemd service for relay server
cat > /etc/systemd/system/remote-access-relay.service << 'SERVICE'
[Unit]
Description=Remote Access Relay Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/remote-access-system
ExecStart=/opt/remote-access-system/bin/relay-server
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
SERVICE

# Reload systemd
systemctl daemon-reload

echo ""
echo "============================================"
echo "Installation completed successfully!"
echo "============================================"
echo ""
echo "Binaries installed to: /opt/remote-access-system/bin/"
echo ""
echo "Available services:"
echo "  - remote-access-account.service (Account Server)"
echo "  - remote-access-relay.service (Relay Server)"
echo ""
echo "To start services:"
echo "  sudo systemctl start remote-access-account"
echo "  sudo systemctl start remote-access-relay"
echo ""
echo "To enable services at boot:"
echo "  sudo systemctl enable remote-access-account"
echo "  sudo systemctl enable remote-access-relay"
echo ""
echo "Client applications:"
echo "  - PC Client: /opt/remote-access-system/bin/pc-client"
echo "  - USB Client: /opt/remote-access-system/bin/usb-client"
echo ""
