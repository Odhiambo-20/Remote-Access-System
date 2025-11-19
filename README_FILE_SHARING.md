# Remote Access System - File Sharing Implementation

## Overview
This implementation adds complete file sharing functionality to the remote access system:

### Features Implemented:
1. **File Browser**: Browse directories on remote PC
2. **File Upload**: Upload files from mobile to PC
3. **File Download**: Download files from PC to mobile
4. **Share URL Generation**: Generate public URLs for file sharing
5. **HTTP Server**: Public file sharing via browser

## Architecture

### Components:

1. **Mobile App (Qt/QML)**
   - File browser interface
   - Upload/download progress tracking
   - Share URL generation
   - Files: `mobile-app/qml/main.qml`, `mobile-app/src/filemanager.{h,cpp}`

2. **Relay Server**
   - File transfer handler (port 2811)
   - HTTP server for public sharing (port 8080)
   - Files: `relay-server/file_transfer_handler.{h,cpp}`, `relay-server/http_server.{h,cpp}`

3. **PC Client**
   - File handler for responding to requests
   - Directory listing
   - File sending/receiving
   - Files: `pc-client/file_handler.{h,cpp}`

## Ports Used:
- **2809**: Account server
- **2810**: Relay server (PC list queries)
- **2811**: File transfer handler
- **8080**: HTTP server (public file sharing)

## Building and Running

### Build Everything:
```bash
./build_and_run.sh
```

### Manual Build:

#### Relay Server:
```bash
cd relay-server/build
cmake ..
make
./relay_server
```

# 1. Kill existing process with sudo
sudo pkill -9 relay_server

# 2. Confirm port is free
sudo netstat -tlnp | grep :2810

# 3. Start relay server (without sudo)
./relay_server

#### PC Client:
```bash
cd pc-client/build
cmake ..
make
./pc_client
```

#### Mobile App:
```bash
cd mobile-app/build
cmake ..
make
./RemoteAccessMobile
```

####  Start everything at once
./deploy_and_run.sh start

## Usage Flow:

1. **Start Services**:
   - Start relay server first
   - Start PC client (will register with relay)
   - Start mobile app

2. **Connect to PC**:
   - Login with credentials
   - Select PC from list
   - Click "Connect"

3. **File Operations**:
   - **Browse**: Navigate through directories
   - **Download**: Click download button on files
   - **Upload**: Click "Upload" button in toolbar
   - **Share**: Click "Share" button to generate public URL

4. **Public Sharing**:
   - Share URLs work in any browser
   - Format: `http://192.168.100.89:8080/share/<token>`
   - Anyone with the URL can download the file

## File Transfer Protocol:

### Mobile to Relay:
```
MOBILE_FILE|<pc_id>
LIST_DIR|<pc_id>|<path>
DOWNLOAD|<pc_id>|<file_path>
UPLOAD|<pc_id>|<remote_path>|<file_size>
GENERATE_URL|<pc_id>|<file_path>
```

### Relay to PC:
```
PC_FILE|<pc_id>
LIST_DIR|<pc_id>|<path>
DOWNLOAD|<pc_id>|<file_path>
UPLOAD|<pc_id>|<file_path>|<file_size>
```

### PC to Relay:
```
DIR_LIST|<file1>|type|size;<file2>|type|size;...
DOWNLOAD_START|<file_size>
<binary file data>
```

## Security Considerations:

1. **Authentication**: Only authenticated mobile clients can access files
2. **PC Binding**: Files can only be accessed from bound PCs
3. **Share Tokens**: Random 32-character tokens for public sharing
4. **Token Expiration**: Implement token expiration (TODO)

## TODO/Future Enhancements:

1. Token expiration for share URLs
2. File upload progress optimization
3. Multiple file selection
4. File deletion
5. File rename/move operations
6. Directory creation
7. File search functionality
8. Thumbnail generation for images
9. Video streaming support
10. Automatic folder synchronization

## Testing:

1. **List Directory**:
   - Connect to PC
   - Should see home directory contents
   - Navigate to subdirectories

2. **Download File**:
   - Click download on any file
   - Check `/tmp/` directory on mobile device
   - Verify file integrity

3. **Upload File**:
   - Click "Upload" button
   - Select local file
   - Verify file appears on PC

4. **Share URL**:
   - Click "Share" on a file
   - Copy generated URL
   - Open in browser
   - Should see download page

## Troubleshooting:

### Connection Issues:
- Verify relay server is running
- Check firewall settings for ports 2811 and 8080
- Verify PC client is registered with relay

### File Transfer Failures:
- Check file permissions on PC
- Verify sufficient disk space
- Check network connectivity
- Review logs for error messages

### Share URL Not Working:
- Verify HTTP server is running (port 8080)
- Check token is valid
- Ensure file still exists on PC

## Log Messages:

- `[FileManager]`: Mobile app file operations
- `[FileTransfer]`: Relay server file transfer handler
- `[HTTP]`: HTTP server for public sharing
- `[FileHandler]`: PC client file handler

## Performance Notes:

- File transfers use 8KB buffers
- Progress updates every chunk
- Concurrent transfers supported
- Large files may take time depending on network speed

