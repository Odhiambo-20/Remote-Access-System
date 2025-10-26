# Remote Access System

A secure, hardware-authenticated remote computer access system for Debian Linux.

## Overview

This system allows you to securely access your computer remotely from a mobile device using hardware-based authentication (USB drive) and encrypted communication channels.

### Features

- **Hardware-based authentication** using USB drive unique ID
- **PC hardware binding** for additional security
- **Multi-device support** - bind multiple PCs to one mobile device
- **File management** - browse, upload, download files
- **File sharing** - generate temporary download links
- **IPv4/IPv6 support** for modern networks
- **Encrypted communication** using TLS/AES-256-GCM
- **Cross-platform mobile app** using Qt/QML

## Architecture

```
[Mobile Device] <---> [Relay Server] <---> [Account Server]
                            ^
                            |
                     [PC with USB Drive]
```

### Components

1. **USB Client** - Identifies USB drive and launches PC client
2. **PC Client** - Runs on your computer, handles file operations
3. **Account Server** - Manages user accounts and PC bindings
4. **Relay Server** - Routes messages between mobile and PC
5. **Mobile App** - Qt/QML application for smartphones

## System Requirements

### Hardware

- USB flash drive (any size)
- Computer running Debian Linux (or derivative)
- Mobile device (Android/iOS with Qt support)

### Software

- Debian 10 (Buster) or later
- GCC 7.0 or later
- CMake 3.16 or later
- OpenSSL 1.1.1 or later
- ACE+TAO 6.5 or later
- SQLite3 3.22 or later
- Qt6 6.2 or later (for mobile app)

## Installation

### 1. Install Dependencies

```bash
sudo ./scripts/install_dependencies.sh
```

This will install:
- Build tools (gcc, g++, cmake, make)
- Development libraries (OpenSSL, SQLite3, ACE+TAO)
- Qt6 (for mobile app development)
- System utilities

### 2. Build the System

```bash


```

This compiles all components:
- Common library
- USB client
- PC client
- Account server
- Relay server

### 3. Install the System

```bash
sudo ./install.sh
```

This will:
- Install binaries to `/usr/local/bin`
- Create configuration directories
- Install systemd services
- Setup autostart for USB client

## Configuration

### Account Server

Configuration file: `/etc/remote-access/account_server.conf`

```ini
[server]
port=2809
database=/var/lib/remote-access/accounts.db
log_file=/var/log/remote-access/account_server.log

[security]
session_timeout=3600
max_failed_logins=5
```

### Relay Server

Configuration file: `/etc/remote-access/relay_server.conf`

```ini
[server]
ipv4_port=3000
ipv6_port=3000
max_connections=1000
log_file=/var/log/remote-access/relay_server.log

[timeouts]
connection_timeout=300
heartbeat_interval=30
```

### PC Client

Configuration file: `<USB_DRIVE>/.remote_access/pc_client.conf`

```ini
[account]
username=your_username
pc_name=My Computer

[servers]
account_server=corbaloc::192.168.1.100:2809/AccountServer
relay_server=192.168.1.100:3000

[file_sharing]
http_port=8080
share_timeout=86400
```

## Usage

### Server Setup

1. **Start Account Server:**
   ```bash
   sudo systemctl start remote-access-account-server
   sudo systemctl enable remote-access-account-server
   ```

2. **Start Relay Server:**
   ```bash
   sudo systemctl start remote-access-relay-server
   sudo systemctl enable remote-access-relay-server
   ```

3. **Check Status:**
   ```bash
   sudo systemctl status remote-access-*
   ```

### PC Client Setup

1. **Prepare USB Drive:**
   ```bash
   # Copy PC client to USB drive
   sudo cp /usr/local/bin/pc_client /media/username/USB_DRIVE/
   
   # Make it executable
   sudo chmod +x /media/username/USB_DRIVE/pc_client
   ```

2. **Run USB Client:**
   ```bash
   /usr/local/bin/usb_client
   ```
   
   The USB client will:
   - Generate/load USB hardware ID
   - Launch PC client automatically
   - PC client will prompt for username/password on first run

3. **Keep USB Plugged In:**
   - PC must have USB drive inserted to accept connections
   - This provides physical security layer

### Mobile App Setup

1. **Build Mobile App:**
   ```bash
   cd mobile-app
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

2. **Install on Device:**
   - For Android: Use Qt Creator or `androiddeployqt`
   - For iOS: Use Qt Creator with Xcode

3. **Configure App:**
   - Enter account server address
   - Login with username/password
   - Bind PCs you want to access

### Mobile App Usage

1. **Bind a PC:**
   - Tap "Add PC" button
   - Enter PC ID (displayed on PC client)
   - Confirm binding

2. **Connect to PC:**
   - Select PC from list (must be online)
   - Tap "Connect"

3. **Browse Files:**
   - Navigate directories
   - Long-press for options (download, share, delete)

4. **Download Files:**
   - Tap file to select
   - Tap "Download" button
   - File downloads to device

5. **Upload Files:**
   - Tap "Upload" button
   - Select file from device
   - Choose destination on PC

6. **Share Files:**
   - Tap file to select
   - Tap "Share" button
   - Copy generated URL
   - Send URL to others

## Security

### Layers of Protection

1. **Physical Security:**
   - USB drive must be inserted (hardware key)
   - PC hardware binding

2. **Authentication:**
   - Username/password required
   - Password hashed with PBKDF2-SHA256
   - Session tokens with expiration

3. **Network Security:**
   - TLS encryption for all connections
   - AES-256-GCM for data encryption
   - Certificate validation

4. **Access Control:**
   - Device binding (only bound devices can connect)
   - File permission checking
   - Path traversal prevention

### Best Practices

- Use strong passwords (12+ characters)
- Keep USB drive secure (it's your key)
- Regularly update the system
- Monitor logs for suspicious activity
- Use firewall to restrict server access
- Enable fail2ban for brute force protection

## Troubleshooting

### USB Client Won't Start

```bash
# Check USB is mounted
mount | grep /media

# Check USB client permissions
ls -l /usr/local/bin/usb_client

# Run with verbose output
/usr/local/bin/usb_client --verbose
```

### PC Client Can't Connect

```bash
# Check servers are running
sudo systemctl status remote-access-*

# Check network connectivity
ping <server_ip>
telnet <server_ip> 2809  # Account server
telnet <server_ip> 3000  # Relay server

# Check firewall
sudo ufw status
sudo ufw allow 2809/tcp
sudo ufw allow 3000/tcp
```

### Mobile App Can't Connect

1. Verify account server address
2. Check network connectivity
3. Verify username/password
4. Check PC is online
5. Review mobile app logs

### File Transfer Fails

```bash
# Check disk space
df -h

# Check file permissions
ls -l




# Navigate to your project directory
cd ~/Documents/Desktop/remote-access-system

# Create the script file (this will open nano editor)
nano start_everything.sh

# Now paste the script content into nano
# Then press: Ctrl+O (to save), Enter (confirm), Ctrl+X (to exit)

# Make it executable
chmod +x start_everything.sh

# NOW RUN IT!
sudo ./start_everything.sh



cd ~/Documents/Desktop/remote-access-system/mobile-app/build

# Run the app
./RemoteAccessMobile