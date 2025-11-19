import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import QtMultimedia
import RemoteAccess 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Remote Access Mobile"

    property string relayServerAddress: "127.0.0.1"
    property int relayServerPort: 2810
    property bool isConnectedToPC: false
    property bool shouldBrowseOnConnect: false
    property var selectedFiles: []
    property string currentDirectory: "/home"

    // MODERN LIGHT THEME BACKGROUND
    color: "#f5f5dc"

    // PC List Model at application level
    ListModel {
        id: pcListModel
    }

    // File List Model
    ListModel {
        id: fileListModel
    }

    ConnectionManager {
        id: connectionManager
        
        onLoginSuccess: {
            console.log("[QML] Login successful")
        }
        
        onLoginFailed: function(error) {
            console.log("[QML] Login failed:", error)
            errorDialog.text = "Login failed: " + error
            errorDialog.open()
        }
    }

    FileManager {
        id: fileManager
        
        onConnected: {
            console.log("[QML] FileManager connected")
            // Always browse directory when connected, regardless of isConnectedToPC flag
            if (shouldBrowseOnConnect) {
                console.log("[QML] Auto-browsing /home directory")
                fileManager.browseDirectory("/home")
                currentDirectory = "/home"
                shouldBrowseOnConnect = false
            }
        }
        
        onConnectionFailed: function(error) {
            console.log("[QML] FileManager connection failed:", error)
            errorDialog.text = "Connection failed: " + error
            errorDialog.open()
        }
        
        onClearFileList: {
            console.log("[QML] Clearing file list")
            fileListModel.clear()
            selectedFiles = []
        }
        
        onAddFileToList: function(name, path, type, size, isDirectory) {
            console.log("[QML] Adding file:", name, path, type, size, isDirectory)
            fileListModel.append({
                "name": name,
                "path": path,
                "type": type,
                "size": size,
                "isDirectory": isDirectory,
                "selected": false
            })
        }
        
        onUploadStarted: {
            console.log("[QML] Upload started")
            uploadDialog.open()
        }
        
        onUploadFinished: function(success, message) {
            console.log("[QML] Upload finished:", success, message)
            uploadDialog.close()
            if (success) {
                successDialog.text = message
                successDialog.open()
                fileManager.browseDirectory(currentDirectory)
            } else {
                errorDialog.text = message
                errorDialog.open()
            }
        }
        
        onShareLinkCreated: function(link) {
            console.log("[QML] Share link created:", link)
            shareLinkDialog.shareLink = link
            shareLinkDialog.open()
        }
        
        onErrorOccurred: function(error) {
            console.log("[QML] FileManager error:", error)
            errorDialog.text = error
            errorDialog.open()
        }
    }

    Connections {
        target: pcManager
        
        function onPcListUpdated(pcList) {
            console.log("[QML] PC list updated:", pcList)
        }
        
        function onConnectionEstablished(pcId) {
            console.log("[QML] Connected to PC:", pcId)
            if (!isConnectedToPC) {
                isConnectedToPC = true
                shouldBrowseOnConnect = true
                fileManager.connectToPC(pcId, relayServerAddress)
                stackView.push(remoteControlPage)
            }
        }
        
        function onConnectionFailed(error) {
            console.log("[QML] Connection failed:", error)
            errorDialog.text = "Connection failed: " + error
            errorDialog.open()
        }
        
        function onClearPCList() {
            console.log("[QML] Clearing PC list")
            pcListModel.clear()
        }
        
        function onAddPCToList(pcId, hostname, username, isOnline) {
            console.log("[QML] Adding PC:", pcId, hostname, username, isOnline)
            pcListModel.append({
                "pcId": pcId,
                "hostname": hostname,
                "username": username,
                "isOnline": isOnline
            })
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainMenuPage
        
        onCurrentItemChanged: {
            // Reset connection flag when navigating back to main menu
            if (currentItem && currentItem.objectName === "mainMenuPage") {
                isConnectedToPC = false
                shouldBrowseOnConnect = false
            }
        }
    }

    // MAIN MENU PAGE
    Component {
        id: mainMenuPage
        
        Page {
            title: "Remote Access Mobile"
            objectName: "mainMenuPage"
            background: Rectangle { color: "#0a0e27" }
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Header
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: "#0f1628"
                    
                    Label {
                        anchors.centerIn: parent
                        text: "Remote Access Mobile"
                        font.pixelSize: 20
                        font.bold: true
                        color: "#ffffff"
                    }
                }

                // Left sidebar and main content
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    // LEFT SIDEBAR
                    Rectangle {
                        Layout.preferredWidth: 240
                        Layout.fillHeight: true
                        color: "#0d1220"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 0
                            spacing: 0

                            Label {
                                text: "Available PCs"
                                font.pixelSize: 16
                                font.bold: true
                                color: "#ffffff"
                                Layout.margins: 20
                            }

                            // Login Button
                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                Layout.leftMargin: 12
                                Layout.rightMargin: 12
                                
                                background: Rectangle {
                                    color: parent.hovered ? "#3d5a80" : "#2a4365"
                                    radius: 8
                                }
                                
                                contentItem: RowLayout {
                                    spacing: 10
                                    
                                    Label {
                                        text: "→"
                                        font.pixelSize: 20
                                        color: "#ffffff"
                                    }
                                    
                                    Label {
                                        text: "Login"
                                        font.pixelSize: 14
                                        color: "#ffffff"
                                    }
                                }
                                
                                onClicked: {
                                    console.log("[QML] Login button clicked")
                                    stackView.push(loginPage)
                                }
                            }

                            // Bind PC Button
                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                Layout.leftMargin: 12
                                Layout.rightMargin: 12
                                Layout.topMargin: 8
                                
                                background: Rectangle {
                                    color: parent.hovered ? "#1e88a8" : "#0e7490"
                                    radius: 8
                                }
                                
                                contentItem: RowLayout {
                                    spacing: 10
                                    
                                    Label {
                                        text: "🔗"
                                        font.pixelSize: 18
                                        color: "#ffffff"
                                    }
                                    
                                    Label {
                                        text: "Bind PC"
                                        font.pixelSize: 14
                                        color: "#ffffff"
                                    }
                                }
                                
                                onClicked: {
                                    console.log("[QML] Bind PC button clicked - going to QR code")
                                    isConnectedToPC = false
                                    shouldBrowseOnConnect = false
                                    stackView.push(qrCodeScanPage)
                                }
                            }

                            // PC List
                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.margins: 12
                                spacing: 8
                                clip: true
                                
                                model: pcListModel
                                
                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 90
                                    color: "#161d31"
                                    radius: 8
                                    border.color: "#2a3a52"
                                    border.width: 1
                                    
                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 12
                                        spacing: 4
                                        
                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 8
                                            
                                            Rectangle {
                                                width: 10
                                                height: 10
                                                radius: 5
                                                color: model.isOnline ? "#10b981" : "#6b7280"
                                            }
                                            
                                            Label {
                                                text: "PC"
                                                font.pixelSize: 12
                                                color: "#9ca3af"
                                            }
                                        }
                                        
                                        Label {
                                            text: model.pcId
                                            font.pixelSize: 13
                                            font.bold: true
                                            color: "#ffffff"
                                            elide: Text.ElideMiddle
                                            Layout.fillWidth: true
                                        }
                                        
                                        Label {
                                            text: "User: " + model.username
                                            font.pixelSize: 11
                                            color: "#00d9ff"
                                        }
                                        
                                        Label {
                                            text: "ID: " + model.pcId
                                            font.pixelSize: 10
                                            color: "#6b7280"
                                            elide: Text.ElideMiddle
                                            Layout.fillWidth: true
                                        }
                                    }
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onEntered: parent.color = "#1e2842"
                                        onExited: parent.color = "#161d31"
                                        onClicked: {
                                            if (model.isOnline && !isConnectedToPC) {
                                                console.log("[QML] Connecting to PC:", model.pcId)
                                                pcManager.connectToPC(model.pcId, relayServerAddress)
                                            }
                                        }
                                    }
                                }
                            }

                            Item { Layout.fillHeight: true }
                        }
                    }

                    // MAIN CONTENT AREA
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#0a0e27"

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: 20

                            Label {
                                text: "Connected: PC-a987adb83f2bf5628a5919409cb5d00f"
                                font.pixelSize: 16
                                color: "#ffffff"
                                Layout.alignment: Qt.AlignHCenter
                            }

                            // Large PC Info Card
                            Rectangle {
                                Layout.preferredWidth: 500
                                Layout.preferredHeight: 300
                                color: "#0f1628"
                                radius: 16
                                border.color: "#2a3a52"
                                border.width: 2

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 20

                                    Label {
                                        text: "PC-a987adb83f2bf5628a5919409cb5d00f"
                                        font.pixelSize: 20
                                        font.bold: true
                                        color: "#ffffff"
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    RowLayout {
                                        spacing: 8
                                        Layout.alignment: Qt.AlignHCenter
                                        
                                        Rectangle {
                                            width: 12
                                            height: 12
                                            radius: 6
                                            color: "#10b981"
                                        }
                                        
                                        Label {
                                            text: "Available (Local)"
                                            font.pixelSize: 14
                                            color: "#00d9ff"
                                        }
                                    }

                                    Label {
                                        text: "User: victor"
                                        font.pixelSize: 16
                                        color: "#00d9ff"
                                        Layout.alignment: Qt.AlignHCenter
                                    }

                                    Button {
                                        text: "Connect"
                                        font.pixelSize: 16
                                        Layout.preferredWidth: 200
                                        Layout.preferredHeight: 50
                                        Layout.alignment: Qt.AlignHCenter
                                        
                                        background: Rectangle {
                                            color: parent.pressed ? "#4a5f82" : (parent.hovered ? "#5a6f92" : "#3d5a80")
                                            radius: 8
                                        }
                                        
                                        contentItem: Label {
                                            text: parent.text
                                            color: "#ffffff"
                                            font: parent.font
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        onClicked: {
                                            console.log("[QML] Connect button clicked")
                                            isConnectedToPC = false
                                            shouldBrowseOnConnect = false
                                            stackView.push(remoteControlPage)
                                        }
                                    }

                                    Button {
                                        text: "Unbind PC"
                                        font.pixelSize: 14
                                        Layout.preferredWidth: 200
                                        Layout.preferredHeight: 40
                                        Layout.alignment: Qt.AlignHCenter
                                        
                                        background: Rectangle {
                                            color: parent.pressed ? "#dc2696ff" : (parent.hovered ? "#44efd8ff" : "transparent")
                                            border.color: "#ef44adff"
                                            border.width: 2
                                            radius: 8
                                        }
                                        
                                        contentItem: Label {
                                            text: parent.text
                                            color: "#ef4444"
                                            font: parent.font
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        onClicked: {
                                            console.log("[QML] Unbind PC clicked")
                                        }
                                    }
                                }
                            }

                            Label {
                                text: "Query PC List"
                                font.pixelSize: 14
                                color: "#6b7280"
                                Layout.alignment: Qt.AlignHCenter
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        console.log("[QML] Query PC List clicked")
                                        pcManager.queryPCList(relayServerAddress, relayServerPort)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // LOGIN PAGE
    Component {
        id: loginPage
        
        Page {
            title: "Remote Access Login"
            background: Rectangle { color: "#0a0e27" }
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Back button
                ToolButton {
                    text: "←"
                    font.pixelSize: 24
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color: parent.pressed ? "#1e2842" : (parent.hovered ? "#161d31" : "transparent")
                    }
                    
                    onClicked: stackView.pop()
                }

                // Login form centered
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 20
                        width: 400

                        Label {
                            text: "Remote Access Login"
                            font.pixelSize: 28
                            font.bold: true
                            color: "#ffffff"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        TextField {
                            id: usernameField
                            text: "victor"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: "#1e2842"
                                radius: 8
                                border.color: parent.activeFocus ? "#3d5a80" : "#2a3a52"
                                border.width: 2
                            }
                            
                            color: "#ffffff"
                            placeholderTextColor: "#6b7280"
                        }

                        TextField {
                            id: passwordField
                            placeholderText: "Password"
                            echoMode: TextInput.Password
                            text: "password"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: "#1e2842"
                                radius: 8
                                border.color: parent.activeFocus ? "#3d5a80" : "#2a3a52"
                                border.width: 2
                            }
                            
                            color: "#ffffff"
                            placeholderTextColor: "#6b7280"
                        }

                        Button {
                            text: "Login"
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            font.pixelSize: 16
                            
                            background: Rectangle {
                                color: parent.pressed ? "#4a5f82" : (parent.hovered ? "#5a6f92" : "#3d5a80")
                                radius: 8
                            }
                            
                            contentItem: Label {
                                text: parent.text
                                color: "#ffffff"
                                font: parent.font
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                console.log("[QML] Login button clicked")
                                connectionManager.login(usernameField.text, passwordField.text)
                                stackView.push(pcListPage)
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: "#2a3a52"
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                        }
                    }
                }
            }
        }
    }

    // QR CODE SCAN PAGE
    Component {
        id: qrCodeScanPage
        
        Page {
            title: "Bind New PC"
            objectName: "qrCodeScanPage"
            background: Rectangle { color: "#0a0e27" }
            
            property string pcQrCodeUrl: ""
            property bool qrCodeLoaded: false
            property string errorMessage: ""
            
            Timer {
                id: scanCheckTimer
                interval: 1000
                repeat: true
                running: qrCodeLoaded && !isConnectedToPC
                onTriggered: {
                    checkForSuccessfulScan()
                }
            }

            function loadQRCode() {
                console.log("[QML] Loading QR code from PC...")
                var xhr = new XMLHttpRequest()
                xhr.open("GET", "http://localhost:8080/qr", true)
                xhr.onreadystatechange = function() {
                    if (xhr.readyState === XMLHttpRequest.DONE) {
                        if (xhr.status === 200) {
                            console.log("[QML] QR code loaded successfully")
                            pcQrCodeUrl = "http://localhost:8080/qr?" + Date.now()
                            qrCodeLoaded = true
                            errorMessage = ""
                        } else {
                            console.log("[QML] Failed to load QR code:", xhr.status)
                            qrCodeLoaded = false
                            errorMessage = "Cannot connect to PC\nMake sure the PC client is running"
                        }
                    }
                }
                xhr.send()
            }

            function checkForSuccessfulScan() {
                if (isConnectedToPC) {
                    console.log("[QML] Already connected, stopping scan check")
                    scanCheckTimer.stop()
                    return
                }
                
                console.log("[QML] Checking for successful scan...")
                var xhr = new XMLHttpRequest()
                xhr.open("GET", "http://localhost:8080/scan-status", true)
                xhr.onreadystatechange = function() {
                    if (xhr.readyState === XMLHttpRequest.DONE) {
                        if (xhr.status === 200) {
                            try {
                                var status = JSON.parse(xhr.responseText)
                                if (status.scanned && status.pc_id) {
                                    console.log("[QML] QR code scanned successfully! PC ID:", status.pc_id)
                                    scanCheckTimer.stop()
                                    connectToPCFromQR(status.pc_id, status.username, status.relay_server, status.relay_port, status.auth_token)
                                }
                            } catch (e) {
                                console.log("[QML] Failed to parse scan status:", e)
                            }
                        }
                    }
                }
                xhr.send()
            }

            function connectToPCFromQR(pcId, username, relayServer, relayPort, authToken) {
                console.log("[QML] Connecting to PC from QR scan:", pcId)
                
                scanCheckTimer.stop()
                shouldBrowseOnConnect = true
                isConnectedToPC = true
                
                pcManager.connectToPC(pcId, relayServer)
                fileManager.connectToPC(pcId, relayServer)
                
                stackView.push(remoteControlPage)
            }
            
            Component.onCompleted: {
                loadQRCode()
            }
            
            Component.onDestruction: {
                scanCheckTimer.stop()
            }
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ToolButton {
                    text: "←"
                    font.pixelSize: 24
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        font: parent.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    background: Rectangle {
                        color: parent.pressed ? "#1e2842" : (parent.hovered ? "#161d31" : "transparent")
                    }
                    
                    onClicked: {
                        scanCheckTimer.stop()
                        stackView.pop()
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 30
                        width: 500

                        Label {
                            text: "Bind New PC"
                            font.pixelSize: 32
                            font.bold: true
                            color: "#ffffff"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Rectangle {
                            Layout.preferredWidth: 400
                            Layout.preferredHeight: 400
                            Layout.alignment: Qt.AlignHCenter
                            color: "#ffffff"
                            radius: 12
                            
                            Image {
                                id: qrCodeImage
                                anchors.fill: parent
                                anchors.margins: 20
                                source: pcQrCodeUrl
                                fillMode: Image.PreserveAspectFit
                                visible: qrCodeLoaded
                                cache: false
                                
                                onStatusChanged: {
                                    if (status === Image.Error) {
                                        console.log("[QML] QR code image failed to load")
                                        qrCodeLoaded = false
                                        errorMessage = "Failed to load QR code"
                                    } else if (status === Image.Ready) {
                                        console.log("[QML] QR code image loaded")
                                        qrCodeLoaded = true
                                        errorMessage = ""
                                    }
                                }
                            }
                            
                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 20
                                visible: !qrCodeLoaded
                                
                                Label {
                                    text: errorMessage !== "" ? "❌" : "⏳"
                                    font.pixelSize: 80
                                    color: errorMessage !== "" ? "#ef4444" : "#6b7280"
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                
                                Label {
                                    text: errorMessage !== "" ? errorMessage : "Loading QR code..."
                                    font.pixelSize: 14
                                    color: "#6b7280"
                                    Layout.alignment: Qt.AlignHCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    wrapMode: Text.WordWrap
                                    Layout.maximumWidth: 300
                                }
                                
                                Button {
                                    text: "Retry"
                                    Layout.alignment: Qt.AlignHCenter
                                    visible: errorMessage !== ""
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#4a5f82" : (parent.hovered ? "#5a6f92" : "#3d5a80")
                                        radius: 8
                                    }
                                    
                                    contentItem: Label {
                                        text: parent.text
                                        color: "#ffffff"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    onClicked: {
                                        loadQRCode()
                                    }
                                }
                            }
                        }
                        
                        Label {
                            text: "Use your phone camera or QR scanner app to\nscan the QR code displayed above"
                            font.pixelSize: 13
                            color: "#9ca3af"
                            Layout.alignment: Qt.AlignHCenter
                            horizontalAlignment: Text.AlignHCenter
                        }

                        ColumnLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 12
                            
                            RowLayout {
                                spacing: 12
                                
                                Label {
                                    text: "1."
                                    font.pixelSize: 14
                                    color: "#00d9ff"
                                    font.bold: true
                                }
                                
                                Label {
                                    text: "Run the Remote Access program on your PC"
                                    font.pixelSize: 14
                                    color: "#d1d5db"
                                }
                            }
                            
                            RowLayout {
                                spacing: 12
                                
                                Label {
                                    text: "2."
                                    font.pixelSize: 14
                                    color: "#00d9ff"
                                    font.bold: true
                                }
                                
                                Label {
                                    text: "The QR code will appear above"
                                    font.pixelSize: 14
                                    color: "#d1d5db"
                                }
                            }
                            
                            RowLayout {
                                spacing: 12
                                
                                Label {
                                    text: "3."
                                    font.pixelSize: 14
                                    color: "#00d9ff"
                                    font.bold: true
                                }
                                
                                Label {
                                    text: "Scan with your phone camera"
                                    font.pixelSize: 14
                                    color: "#d1d5db"
                                }
                            }
                            
                            RowLayout {
                                spacing: 12
                                
                                Label {
                                    text: "4."
                                    font.pixelSize: 14
                                    color: "#00d9ff"
                                    font.bold: true
                                }
                                
                                Label {
                                    text: "You'll be automatically connected to the file browser"
                                    font.pixelSize: 14
                                    color: "#d1d5db"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: pcListPage
        
        Page {
            title: "Available PCs"
            background: Rectangle { color: "#0a0e27" }
            
            header: ToolBar {
                background: Rectangle { color: "#12172e" }
                
                RowLayout {
                    anchors.fill: parent
                    
                    ToolButton {
                        text: "←"
                        onClicked: stackView.pop()
                    }
                    
                    Label {
                        text: "Available PCs"
                        font.pixelSize: 20
                        color: "#ffffff"
                        Layout.fillWidth: true
                    }
                    
                    ToolButton {
                        text: "⟳"
                        onClicked: {
                            console.log("[QML] Refresh button clicked")
                            pcManager.queryPCList(relayServerAddress, relayServerPort)
                        }
                    }
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Label {
                    text: "Connected PCs: " + pcListModel.count
                    font.pixelSize: 16
                    color: "#ffffff"
                    Layout.alignment: Qt.AlignHCenter
                }

                ListView {
                    id: pcListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    model: pcListModel
                    
                    delegate: ItemDelegate {
                        width: pcListView.width
                        height: 100
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 15
                            
                            Rectangle {
                                width: 16
                                height: 16
                                radius: 8
                                color: model.isOnline ? "green" : "gray"
                            }
                            
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 5
                                
                                Label {
                                    text: model.hostname
                                    font.pixelSize: 18
                                    font.bold: true
                                    color: "#ffffff"
                                }
                                
                                Label {
                                    text: "User: " + model.username
                                    font.pixelSize: 14
                                    color: "#00d9ff"
                                }
                                
                                Label {
                                    text: "ID: " + model.pcId
                                    font.pixelSize: 12
                                    color: "#b2bec3"
                                }
                            }
                            
                            Button {
                                text: model.isOnline ? "Connect" : "Offline"
                                enabled: model.isOnline
                                Layout.preferredWidth: 100
                                onClicked: {
                                    if (!isConnectedToPC) {
                                        console.log("[QML] Connecting to PC:", model.pcId)
                                        pcManager.connectToPC(model.pcId, relayServerAddress)
                                    }
                                }
                            }
                        }
                        
                        background: Rectangle {
                            color: parent.hovered ? "#1e2442" : "#161b33"
                            border.color: "#6c5ce7"
                            border.width: 1
                            radius: 5
                        }
                    }
                    
                    ScrollBar.vertical: ScrollBar {}
                }

                Button {
                    text: "Query PC List"
                    Layout.fillWidth: true
                    highlighted: true
                    onClicked: {
                        console.log("[QML] Query PC List button clicked")
                        pcManager.queryPCList(relayServerAddress, relayServerPort)
                    }
                }
            }
        }
    }

    Component {
        id: remoteControlPage
        
        Page {
            title: "Remote Control"
            objectName: "remoteControlPage"
            background: Rectangle { color: "#f5f5dc" }
            
            header: ToolBar {
                background: Rectangle { color: "#ffffff" }
                
                RowLayout {
                    anchors.fill: parent
                    
                    ToolButton {
                        text: "←"
                        contentItem: Label {
                            text: parent.text
                            color: "#333333"
                            font.pixelSize: 24
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            pcManager.disconnectFromPC()
                            isConnectedToPC = false
                            shouldBrowseOnConnect = false
                            selectedFiles = []
                            stackView.pop()
                        }
                    }
                    
                    Label {
                        text: "Connected to: " + pcManager.connectedPcId
                        font.pixelSize: 18
                        color: "#333333"
                        Layout.fillWidth: true
                    }
                    
                    ToolButton {
                        text: "✕"
                        contentItem: Label {
                            text: parent.text
                            color: "#333333"
                            font.pixelSize: 20
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            pcManager.disconnectFromPC()
                            isConnectedToPC = false
                            shouldBrowseOnConnect = false
                            selectedFiles = []
                            stackView.pop()
                        }
                    }
                }
            }

            TabBar {
                id: tabBar
                width: parent.width
                background: Rectangle { color: "#ffffff" }
                
                TabButton {
                    text: "Remote Desktop"
                    contentItem: Label {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                TabButton {
                    text: "File Browser"
                    contentItem: Label {
                        text: parent.text
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            StackLayout {
                width: parent.width
                anchors.top: tabBar.bottom
                anchors.bottom: parent.bottom
                currentIndex: tabBar.currentIndex

                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 20

                        Label {
                            text: "Remote Desktop View"
                            font.pixelSize: 24
                            color: "#333333"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "black"
                            border.color: "#cccccc"
                            border.width: 2
                            
                            Label {
                                anchors.centerIn: parent
                                text: "Remote screen will appear here\n(Not yet implemented)"
                                color: "white"
                                font.pixelSize: 18
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

                // FILE BROWSER TAB - MODERN LAYOUT WITH FULL FUNCTIONALITY
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        // Search bar at top
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 60
                            color: "#f5f5dc"
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10
                                
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 40
                                    color: "#ffffff"
                                    radius: 20
                                    border.color: "#dddddd"
                                    border.width: 1
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.leftMargin: 15
                                        anchors.rightMargin: 15
                                        spacing: 10
                                        
                                        Label {
                                            text: "🔍"
                                            font.pixelSize: 18
                                            color: "#666666"
                                        }
                                        
                                        TextField {
                                            id: searchField
                                            placeholderText: "Please enter the file name you want to search for"
                                            Layout.fillWidth: true
                                            font.pixelSize: 14
                                            color: "#333333"
                                            
                                            background: Rectangle {
                                                color: "transparent"
                                            }
                                        }
                                    }
                                }
                                
                                Button {
                                    Layout.preferredWidth: 50
                                    Layout.preferredHeight: 40
                                    
                                    contentItem: Label {
                                        text: "⟳"
                                        font.pixelSize: 24
                                        color: "#333333"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered ? "#eeeeee" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Refresh file list")
                                        fileManager.browseDirectory(currentDirectory)
                                    }
                                }
                                
                                Button {
                                    Layout.preferredWidth: 50
                                    Layout.preferredHeight: 40
                                    
                                    contentItem: Label {
                                        text: selectedFiles.length > 0 ? "✓ " + selectedFiles.length : "☰"
                                        font.pixelSize: 18
                                        color: selectedFiles.length > 0 ? "#4CAF50" : "#333333"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered ? "#eeeeee" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        if (selectedFiles.length > 0) {
                                            // Deselect all
                                            selectedFiles = []
                                            for (var i = 0; i < fileListModel.count; i++) {
                                                fileListModel.setProperty(i, "selected", false)
                                            }
                                        } else {
                                            // Select all
                                            var allFiles = []
                                            for (var j = 0; j < fileListModel.count; j++) {
                                                fileListModel.setProperty(j, "selected", true)
                                                allFiles.push({
                                                    name: fileListModel.get(j).name,
                                                    path: fileListModel.get(j).path,
                                                    isDirectory: fileListModel.get(j).isDirectory
                                                })
                                            }
                                            selectedFiles = allFiles
                                        }
                                    }
                                }
                            }
                        }

                        // Current path breadcrumb
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            color: "#ffffff"
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 20
                                spacing: 5
                                
                                Label {
                                    text: "📁"
                                    font.pixelSize: 16
                                }
                                
                                Label {
                                    text: currentDirectory
                                    font.pixelSize: 14
                                    color: "#666666"
                                    elide: Text.ElideMiddle
                                    Layout.fillWidth: true
                                }
                                
                                Button {
                                    text: "↑ Parent"
                                    visible: currentDirectory !== "/"
                                    Layout.preferredHeight: 30
                                    Layout.rightMargin: 10
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered ? "#eeeeee" : "#f5f5f5")
                                        radius: 6
                                    }
                                    
                                    contentItem: Label {
                                        text: parent.text
                                        color: "#333333"
                                        font.pixelSize: 12
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    onClicked: {
                                        var parentPath = currentDirectory.substring(0, currentDirectory.lastIndexOf("/"))
                                        if (parentPath === "") parentPath = "/"
                                        currentDirectory = parentPath
                                        fileManager.browseDirectory(parentPath)
                                        selectedFiles = []
                                    }
                                }
                            }
                        }

                        // FILE LIST - Clean modern style with selection
                        ListView {
                            id: fileListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 0
                            clip: true
                            
                            model: fileListModel
                            
                            delegate: Rectangle {
                                width: fileListView.width
                                height: 90
                                color: model.selected ? "#e3f2fd" : (index % 2 === 0 ? "#ffffff" : "#fafaf5")
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 20
                                    anchors.rightMargin: 20
                                    spacing: 15
                                    
                                    // Folder/File icon
                                    Rectangle {
                                        width: 50
                                        height: 50
                                        color: "transparent"
                                        
                                        Label {
                                            anchors.centerIn: parent
                                            text: model.isDirectory ? "📁" : getFileIcon(model.name)
                                            font.pixelSize: 40
                                        }
                                    }
                                    
                                    // File info
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        
                                        Label {
                                            text: model.name
                                            font.pixelSize: 16
                                            font.bold: false
                                            color: "#333333"
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }
                                        
                                        RowLayout {
                                            spacing: 10
                                            
                                            Label {
                                                text: model.isDirectory ? "Folder" : formatFileSize(model.size)
                                                font.pixelSize: 12
                                                color: "#999999"
                                            }
                                            
                                            Label {
                                                text: "•"
                                                font.pixelSize: 12
                                                color: "#999999"
                                                visible: !model.isDirectory
                                            }
                                            
                                            Label {
                                                text: getFileType(model.name)
                                                font.pixelSize: 12
                                                color: "#c9b896"
                                                visible: !model.isDirectory
                                            }
                                        }
                                    }
                                    
                                    // Selection indicator (circular checkbox)
                                    Rectangle {
                                        width: 28
                                        height: 28
                                        radius: 14
                                        color: model.selected ? "#4CAF50" : "transparent"
                                        border.color: model.selected ? "#4CAF50" : "#dddddd"
                                        border.width: 2
                                        
                                        Label {
                                            anchors.centerIn: parent
                                            text: "✓"
                                            color: "#ffffff"
                                            font.pixelSize: 16
                                            font.bold: true
                                            visible: model.selected
                                        }
                                    }
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    
                                    onClicked: {
                                        // Single click for directories navigates
                                        if (model.isDirectory) {
                                            currentDirectory = model.path
                                            fileManager.browseDirectory(model.path)
                                            selectedFiles = []
                                        } else {
                                            // Toggle selection for files
                                            var isCurrentlySelected = model.selected
                                            fileListModel.setProperty(index, "selected", !isCurrentlySelected)
                                            
                                            if (!isCurrentlySelected) {
                                                // Add to selection
                                                var newSelection = selectedFiles.slice()
                                                newSelection.push({
                                                    name: model.name,
                                                    path: model.path,
                                                    isDirectory: model.isDirectory
                                                })
                                                selectedFiles = newSelection
                                            } else {
                                                // Remove from selection
                                                var updatedSelection = []
                                                for (var i = 0; i < selectedFiles.length; i++) {
                                                    if (selectedFiles[i].path !== model.path) {
                                                        updatedSelection.push(selectedFiles[i])
                                                    }
                                                }
                                                selectedFiles = updatedSelection
                                            }
                                        }
                                    }
                                    
                                    onPressAndHold: {
                                        // Long press toggles selection for both files and folders
                                        var isCurrentlySelected = model.selected
                                        fileListModel.setProperty(index, "selected", !isCurrentlySelected)
                                        
                                        if (!isCurrentlySelected) {
                                            var newSelection = selectedFiles.slice()
                                            newSelection.push({
                                                name: model.name,
                                                path: model.path,
                                                isDirectory: model.isDirectory
                                            })
                                            selectedFiles = newSelection
                                        } else {
                                            var updatedSelection = []
                                            for (var i = 0; i < selectedFiles.length; i++) {
                                                if (selectedFiles[i].path !== model.path) {
                                                    updatedSelection.push(selectedFiles[i])
                                                }
                                            }
                                            selectedFiles = updatedSelection
                                        }
                                    }
                                }
                            }
                            
                            ScrollBar.vertical: ScrollBar {
                                contentItem: Rectangle {
                                    implicitWidth: 6
                                    radius: 3
                                    color: "#cccccc"
                                }
                            }
                        }

                        // Bottom action bar
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 80
                            color: "#ffffff"
                            border.color: "#eeeeee"
                            border.width: 1
                            
                            RowLayout {
                                anchors.centerIn: parent
                                spacing: 20
                                
                                // Upload button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    enabled: true
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "⬆️"
                                            font.pixelSize: 28
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                        
                                        Label {
                                            text: "Upload"
                                            font.pixelSize: 11
                                            color: parent.parent.enabled ? "#666666" : "#cccccc"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Upload button clicked")
                                        fileDialog.open()
                                    }
                                }
                                
                                // Download button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    enabled: selectedFiles.length > 0
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "⬇️"
                                            font.pixelSize: 28
                                            Layout.alignment: Qt.AlignHCenter
                                            opacity: parent.parent.enabled ? 1.0 : 0.3
                                        }
                                        
                                        Label {
                                            text: "Download"
                                            font.pixelSize: 11
                                            color: parent.parent.enabled ? "#666666" : "#cccccc"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered && parent.enabled ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Download button clicked for files:", JSON.stringify(selectedFiles))
                                        if (selectedFiles.length === 1) {
                                            fileManager.downloadFile(selectedFiles[0].path)
                                            successDialog.text = "Downloading: " + selectedFiles[0].name
                                            successDialog.open()
                                        } else {
                                            fileManager.downloadMultipleFiles(selectedFiles)
                                            successDialog.text = "Downloading " + selectedFiles.length + " files..."
                                            successDialog.open()
                                        }
                                    }
                                }
                                
                                // Share button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    enabled: selectedFiles.length > 0
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "🔗"
                                            font.pixelSize: 28
                                            Layout.alignment: Qt.AlignHCenter
                                            opacity: parent.parent.enabled ? 1.0 : 0.3
                                        }
                                        
                                        Label {
                                            text: "Share"
                                            font.pixelSize: 11
                                            color: parent.parent.enabled ? "#666666" : "#cccccc"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered && parent.enabled ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Share button clicked for files:", JSON.stringify(selectedFiles))
                                        if (selectedFiles.length > 0) {
                                            fileManager.createShareLink(selectedFiles[0].path)
                                        }
                                    }
                                }
                                
                                // Copy to button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    enabled: selectedFiles.length > 0
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "📋"
                                            font.pixelSize: 28
                                            Layout.alignment: Qt.AlignHCenter
                                            opacity: parent.parent.enabled ? 1.0 : 0.3
                                        }
                                        
                                        Label {
                                            text: "Copy to"
                                            font.pixelSize: 11
                                            color: parent.parent.enabled ? "#666666" : "#cccccc"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered && parent.enabled ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Copy to button clicked")
                                        destinationDialog.dialogTitle = "Copy To"
                                        destinationDialog.operation = "copy"
                                        destinationDialog.open()
                                    }
                                }
                                
                                // Move to button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    enabled: selectedFiles.length > 0
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "✂️"
                                            font.pixelSize: 28
                                            Layout.alignment: Qt.AlignHCenter
                                            opacity: parent.parent.enabled ? 1.0 : 0.3
                                        }
                                        
                                        Label {
                                            text: "Move to"
                                            font.pixelSize: 11
                                            color: parent.parent.enabled ? "#666666" : "#cccccc"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered && parent.enabled ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] Move to button clicked")
                                        destinationDialog.dialogTitle = "Move To"
                                        destinationDialog.operation = "move"
                                        destinationDialog.open()
                                    }
                                }
                                
                                // More button
                                Button {
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 70
                                    
                                    contentItem: ColumnLayout {
                                        spacing: 4
                                        
                                        Label {
                                            text: "⋯"
                                            font.pixelSize: 32
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                        
                                        Label {
                                            text: "More"
                                            font.pixelSize: 11
                                            color: "#666666"
                                            Layout.alignment: Qt.AlignHCenter
                                        }
                                    }
                                    
                                    background: Rectangle {
                                        color: parent.pressed ? "#dddddd" : (parent.hovered ? "#f5f5f5" : "transparent")
                                        radius: 8
                                    }
                                    
                                    onClicked: {
                                        console.log("[QML] More button clicked")
                                        moreActionsDialog.open()
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            Component.onCompleted: {
                tabBar.currentIndex = 1
            }
        }
    }

    // FILE DIALOG FOR UPLOADS
    FileDialog {
        id: fileDialog
        title: "Select file to upload"
        fileMode: FileDialog.OpenFile
        onAccepted: {
            console.log("[QML] File selected:", selectedFile)
            fileManager.uploadFile(selectedFile, currentDirectory)
        }
    }

    // UPLOAD PROGRESS DIALOG
    Dialog {
        id: uploadDialog
        title: "Uploading File"
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            spacing: 20
            width: 300
            
            Label {
                text: "Uploading file..."
                color: "#333333"
                font.pixelSize: 16
                Layout.alignment: Qt.AlignHCenter
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
                color: "#e0e0e0"
                radius: 5
                
                Rectangle {
                    width: parent.width * (fileManager.uploadProgress / 100)
                    height: parent.height
                    color: "#4CAF50"
                    radius: 5
                    
                    Behavior on width {
                        NumberAnimation { duration: 100 }
                    }
                }
            }
            
            Label {
                text: Math.round(fileManager.uploadProgress) + "%"
                color: "#4CAF50"
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: 20
                font.bold: true
            }
        }
    }

    // DESTINATION DIALOG FOR COPY/MOVE
    Dialog {
        id: destinationDialog
        title: dialogTitle
        modal: true
        anchors.centerIn: parent
        width: 500
        
        property string dialogTitle: "Select Destination"
        property string operation: "copy"
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "Enter destination path:"
                font.bold: true
                color: "#333333"
                font.pixelSize: 14
            }
            
            TextField {
                id: destinationField
                text: currentDirectory
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                font.pixelSize: 14
                placeholderText: "/path/to/destination"
                
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: parent.activeFocus ? "#4CAF50" : "#dddddd"
                    border.width: 2
                }
                
                color: "#333333"
            }
            
            Label {
                text: selectedFiles.length + " file(s) selected"
                font.pixelSize: 12
                color: "#666666"
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#f5f5f5")
                        radius: 8
                        border.color: "#dddddd"
                        border.width: 1
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#666666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                    
                    onClicked: destinationDialog.close()
                }
                
                Button {
                    text: destinationDialog.operation === "copy" ? "Copy Here" : "Move Here"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                        radius: 8
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    
                    onClicked: {
                        var destination = destinationField.text
                        console.log("[QML]", destinationDialog.operation, "files to:", destination)
                        
                        if (destinationDialog.operation === "copy") {
                            for (var i = 0; i < selectedFiles.length; i++) {
                                fileManager.copyFile(selectedFiles[i].path, destination)
                            }
                            successDialog.text = "Copied " + selectedFiles.length + " file(s) to " + destination
                        } else {
                            for (var j = 0; j < selectedFiles.length; j++) {
                                fileManager.moveFile(selectedFiles[j].path, destination)
                            }
                            successDialog.text = "Moved " + selectedFiles.length + " file(s) to " + destination
                        }
                        
                        destinationDialog.close()
                        successDialog.open()
                        
                        // Refresh the current directory
                        setTimeout(function() {
                            fileManager.browseDirectory(currentDirectory)
                            selectedFiles = []
                        }, 500)
                    }
                }
            }
        }
    }

    // MORE ACTIONS DIALOG
    Dialog {
        id: moreActionsDialog
        title: "More Actions"
        modal: true
        anchors.centerIn: parent
        width: 400
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 10
            
            Label {
                text: selectedFiles.length > 0 ? selectedFiles.length + " file(s) selected" : "No files selected"
                font.pixelSize: 14
                color: "#666666"
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }
            
            // Rename button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                enabled: selectedFiles.length === 1
                
                background: Rectangle {
                    color: parent.pressed ? "#e0e0e0" : (parent.hovered && parent.enabled ? "#f5f5f5" : "#fafafa")
                    radius: 8
                    border.color: "#eeeeee"
                    border.width: 1
                }
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    Label {
                        text: "✏️"
                        font.pixelSize: 24
                        Layout.leftMargin: 15
                        opacity: parent.parent.enabled ? 1.0 : 0.3
                    }
                    
                    Label {
                        text: "Rename"
                        font.pixelSize: 16
                        color: parent.parent.enabled ? "#333333" : "#cccccc"
                        Layout.fillWidth: true
                    }
                }
                
                onClicked: {
                    moreActionsDialog.close()
                    renameDialog.oldName = selectedFiles[0].name
                    renameDialog.filePath = selectedFiles[0].path
                    renameDialog.open()
                }
            }
            
            // Delete button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                enabled: selectedFiles.length > 0
                
                background: Rectangle {
                    color: parent.pressed ? "#ffcdd2" : (parent.hovered && parent.enabled ? "#ffebee" : "#fafafa")
                    radius: 8
                    border.color: "#eeeeee"
                    border.width: 1
                }
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    Label {
                        text: "🗑️"
                        font.pixelSize: 24
                        Layout.leftMargin: 15
                        opacity: parent.parent.enabled ? 1.0 : 0.3
                    }
                    
                    Label {
                        text: "Delete"
                        font.pixelSize: 16
                        color: parent.parent.enabled ? "#f44336" : "#cccccc"
                        Layout.fillWidth: true
                    }
                }
                
                onClicked: {
                    moreActionsDialog.close()
                    deleteConfirmDialog.open()
                }
            }
            
            // Create folder button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                
                background: Rectangle {
                    color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f5f5f5" : "#fafafa")
                    radius: 8
                    border.color: "#eeeeee"
                    border.width: 1
                }
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    Label {
                        text: "📁"
                        font.pixelSize: 24
                        Layout.leftMargin: 15
                    }
                    
                    Label {
                        text: "Create Folder"
                        font.pixelSize: 16
                        color: "#333333"
                        Layout.fillWidth: true
                    }
                }
                
                onClicked: {
                    moreActionsDialog.close()
                    createFolderDialog.open()
                }
            }
            
            // File properties button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                enabled: selectedFiles.length === 1
                
                background: Rectangle {
                    color: parent.pressed ? "#e0e0e0" : (parent.hovered && parent.enabled ? "#f5f5f5" : "#fafafa")
                    radius: 8
                    border.color: "#eeeeee"
                    border.width: 1
                }
                
                contentItem: RowLayout {
                    spacing: 15
                    
                    Label {
                        text: "ℹ️"
                        font.pixelSize: 24
                        Layout.leftMargin: 15
                        opacity: parent.parent.enabled ? 1.0 : 0.3
                    }
                    
                    Label {
                        text: "Properties"
                        font.pixelSize: 16
                        color: parent.parent.enabled ? "#333333" : "#cccccc"
                        Layout.fillWidth: true
                    }
                }
                
                onClicked: {
                    moreActionsDialog.close()
                    propertiesDialog.open()
                }
            }
            
            // Close button
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                Layout.topMargin: 10
                
                background: Rectangle {
                    color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#ffffff")
                    radius: 8
                    border.color: "#4CAF50"
                    border.width: 2
                }
                
                contentItem: Label {
                    text: "Close"
                    font.pixelSize: 16
                    color: "#4CAF50"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: moreActionsDialog.close()
            }
        }
    }

    // RENAME DIALOG
    Dialog {
        id: renameDialog
        title: "Rename File"
        modal: true
        anchors.centerIn: parent
        width: 450
        
        property string oldName: ""
        property string filePath: ""
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "Old name: " + renameDialog.oldName
                font.pixelSize: 13
                color: "#666666"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            Label {
                text: "New name:"
                font.bold: true
                color: "#333333"
                font.pixelSize: 14
            }
            
            TextField {
                id: renameField
                text: renameDialog.oldName
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                font.pixelSize: 14
                selectByMouse: true
                
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: parent.activeFocus ? "#4CAF50" : "#dddddd"
                    border.width: 2
                }
                
                color: "#333333"
                
                Component.onCompleted: {
                    selectAll()
                    forceActiveFocus()
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#f5f5f5")
                        radius: 8
                        border.color: "#dddddd"
                        border.width: 1
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#666666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                    
                    onClicked: renameDialog.close()
                }
                
                Button {
                    text: "Rename"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                        radius: 8
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    
                    onClicked: {
                        var newName = renameField.text
                        console.log("[QML] Renaming", renameDialog.filePath, "to", newName)
                        fileManager.renameFile(renameDialog.filePath, newName)
                        renameDialog.close()
                        successDialog.text = "Renamed to: " + newName
                        successDialog.open()
                        
                        setTimeout(function() {
                            fileManager.browseDirectory(currentDirectory)
                            selectedFiles = []
                        }, 500)
                    }
                }
            }
        }
    }

    // CREATE FOLDER DIALOG
    Dialog {
        id: createFolderDialog
        title: "Create New Folder"
        modal: true
        anchors.centerIn: parent
        width: 450
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "Folder will be created in: " + currentDirectory
                font.pixelSize: 12
                color: "#666666"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            
            Label {
                text: "Folder name:"
                font.bold: true
                color: "#333333"
                font.pixelSize: 14
            }
            
            TextField {
                id: folderNameField
                placeholderText: "New Folder"
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                font.pixelSize: 14
                
                background: Rectangle {
                    color: "#f5f5f5"
                    radius: 8
                    border.color: parent.activeFocus ? "#4CAF50" : "#dddddd"
                    border.width: 2
                }
                
                color: "#333333"
                
                Component.onCompleted: {
                    forceActiveFocus()
                }
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#f5f5f5")
                        radius: 8
                        border.color: "#dddddd"
                        border.width: 1
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#666666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                    
                    onClicked: {
                        folderNameField.text = ""
                        createFolderDialog.close()
                    }
                }
                
                Button {
                    text: "Create"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    enabled: folderNameField.text.length > 0
                    
                    background: Rectangle {
                        color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                        radius: 8
                        opacity: parent.enabled ? 1.0 : 0.5
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    
                    onClicked: {
                        var folderName = folderNameField.text
                        var folderPath = currentDirectory + "/" + folderName
                        console.log("[QML] Creating folder:", folderPath)
                        fileManager.createFolder(folderPath)
                        folderNameField.text = ""
                        createFolderDialog.close()
                        successDialog.text = "Created folder: " + folderName
                        successDialog.open()
                        
                        setTimeout(function() {
                            fileManager.browseDirectory(currentDirectory)
                        }, 500)
                    }
                }
            }
        }
    }

    // DELETE CONFIRMATION DIALOG
    Dialog {
        id: deleteConfirmDialog
        title: "Confirm Delete"
        modal: true
        anchors.centerIn: parent
        width: 450
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#f44336"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "⚠️"
                font.pixelSize: 60
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: "Are you sure you want to delete " + selectedFiles.length + " file(s)?"
                font.pixelSize: 16
                font.bold: true
                color: "#333333"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Label {
                text: "This action cannot be undone."
                font.pixelSize: 13
                color: "#f44336"
                Layout.alignment: Qt.AlignHCenter
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Layout.topMargin: 10
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#f5f5f5")
                        radius: 8
                        border.color: "#dddddd"
                        border.width: 1
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#666666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                    
                    onClicked: deleteConfirmDialog.close()
                }
                
                Button {
                    text: "Delete"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#c62828" : (parent.hovered ? "#e53935" : "#f44336")
                        radius: 8
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    
                    onClicked: {
                        console.log("[QML] Deleting files:", JSON.stringify(selectedFiles))
                        for (var i = 0; i < selectedFiles.length; i++) {
                            fileManager.deleteFile(selectedFiles[i].path)
                        }
                        deleteConfirmDialog.close()
                        successDialog.text = "Deleted " + selectedFiles.length + " file(s)"
                        successDialog.open()
                        
                        setTimeout(function() {
                            fileManager.browseDirectory(currentDirectory)
                            selectedFiles = []
                        }, 500)
                    }
                }
            }
        }
    }

    // PROPERTIES DIALOG
    Dialog {
        id: propertiesDialog
        title: "File Properties"
        modal: true
        anchors.centerIn: parent
        width: 450
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 15
            
            // File icon and name
            RowLayout {
                spacing: 15
                Layout.fillWidth: true
                
                Label {
                    text: selectedFiles.length > 0 ? (selectedFiles[0].isDirectory ? "📁" : getFileIcon(selectedFiles[0].name)) : "📄"
                    font.pixelSize: 50
                }
                
                Label {
                    text: selectedFiles.length > 0 ? selectedFiles[0].name : ""
                    font.pixelSize: 18
                    font.bold: true
                    color: "#333333"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#eeeeee"
            }
            
            // Properties grid
            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 15
                Layout.fillWidth: true
                
                Label {
                    text: "Type:"
                    font.bold: true
                    color: "#666666"
                    font.pixelSize: 13
                }
                
                Label {
                    text: selectedFiles.length > 0 ? (selectedFiles[0].isDirectory ? "Folder" : getFileType(selectedFiles[0].name)) : ""
                    color: "#333333"
                    font.pixelSize: 13
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
                
                Label {
                    text: "Location:"
                    font.bold: true
                    color: "#666666"
                    font.pixelSize: 13
                }
                
                Label {
                    text: selectedFiles.length > 0 ? selectedFiles[0].path : ""
                    color: "#333333"
                    font.pixelSize: 13
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    elide: Text.ElideMiddle
                }
                
                Label {
                    text: "Size:"
                    font.bold: true
                    color: "#666666"
                    font.pixelSize: 13
                    visible: selectedFiles.length > 0 && !selectedFiles[0].isDirectory
                }
                
                Label {
                    text: selectedFiles.length > 0 ? formatFileSize(selectedFiles[0].size) : ""
                    color: "#333333"
                    font.pixelSize: 13
                    Layout.fillWidth: true
                    visible: selectedFiles.length > 0 && !selectedFiles[0].isDirectory
                }
            }
            
            Button {
                text: "Close"
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                Layout.topMargin: 10
                
                background: Rectangle {
                    color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                    radius: 8
                }
                
                contentItem: Label {
                    text: parent.text
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                
                onClicked: propertiesDialog.close()
            }
        }
    }

    // SHARE LINK DIALOG
    Dialog {
        id: shareLinkDialog
        title: "Share Link Created"
        modal: true
        anchors.centerIn: parent
        width: 500
        
        property string shareLink: ""
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "🔗"
                font.pixelSize: 60
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: "Your shareable link:"
                font.bold: true
                color: "#333333"
                font.pixelSize: 14
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                color: "#f5f5f5"
                radius: 8
                border.color: "#dddddd"
                border.width: 1
                
                TextField {
                    id: linkField
                    anchors.fill: parent
                    anchors.margins: 10
                    text: shareLinkDialog.shareLink
                    readOnly: true
                    selectByMouse: true
                    font.pixelSize: 13
                    color: "#333333"
                    
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
            
            Label {
                text: "Anyone with this link can download the file"
                font.pixelSize: 12
                color: "#666666"
                Layout.alignment: Qt.AlignHCenter
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    text: "Copy Link"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                        radius: 8
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    
                    onClicked: {
                        linkField.selectAll()
                        linkField.copy()
                        successDialog.text = "Link copied to clipboard!"
                        successDialog.open()
                        shareLinkDialog.close()
                    }
                }
                
                Button {
                    text: "Close"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    
                    background: Rectangle {
                        color: parent.pressed ? "#e0e0e0" : (parent.hovered ? "#f0f0f0" : "#f5f5f5")
                        radius: 8
                        border.color: "#dddddd"
                        border.width: 1
                    }
                    
                    contentItem: Label {
                        text: parent.text
                        color: "#666666"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                    
                    onClicked: shareLinkDialog.close()
                }
            }
        }
    }

    // SUCCESS DIALOG
    Dialog {
        id: successDialog
        title: "Success"
        property alias text: successLabel.text
        modal: true
        anchors.centerIn: parent
        width: 400
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#4CAF50"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "✓"
                font.pixelSize: 60
                color: "#4CAF50"
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                id: successLabel
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                color: "#333333"
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "OK"
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#45a049" : (parent.hovered ? "#66BB6A" : "#4CAF50")
                    radius: 8
                }
                
                contentItem: Label {
                    text: parent.text
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                
                onClicked: successDialog.close()
            }
        }
    }

    // ERROR DIALOG
    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorLabel.text
        modal: true
        anchors.centerIn: parent
        width: 400
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#f44336"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            width: parent.width
            spacing: 20
            
            Label {
                text: "✕"
                font.pixelSize: 60
                color: "#f44336"
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                id: errorLabel
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                color: "#333333"
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "OK"
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#c62828" : (parent.hovered ? "#e53935" : "#f44336")
                    radius: 8
                }
                
                contentItem: Label {
                    text: parent.text
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                
                onClicked: errorDialog.close()
            }
        }
    }

    // HELPER FUNCTIONS
    function formatFileSize(bytes) {
        if (bytes === 0) return "0 B"
        if (bytes === undefined || bytes === null) return "Unknown"
        const k = 1024
        const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
        const i = Math.floor(Math.log(bytes) / Math.log(k))
        return (bytes / Math.pow(k, i)).toFixed(2) + ' ' + sizes[i]
    }

    function formatDate(path) {
        return new Date().toLocaleDateString()
    }
    
    function getFileIcon(filename) {
        var ext = filename.split('.').pop().toLowerCase()
        
        if (ext === "pdf") return "📕"
        if (ext === "doc" || ext === "docx") return "📘"
        if (ext === "xls" || ext === "xlsx") return "📗"
        if (ext === "ppt" || ext === "pptx") return "📙"
        if (ext === "zip" || ext === "rar" || ext === "7z") return "📦"
        if (ext === "jpg" || ext === "jpeg" || ext === "png" || ext === "gif") return "🖼️"
        if (ext === "mp3" || ext === "wav" || ext === "flac") return "🎵"
        if (ext === "mp4" || ext === "avi" || ext === "mkv") return "🎬"
        if (ext === "txt") return "📝"
        if (ext === "html" || ext === "css" || ext === "js") return "💻"
        
        return "📄"
    }
    
    function getFileType(filename) {
        var ext = filename.split('.').pop().toLowerCase()
        
        if (ext === "pdf") return "PDF Document"
        if (ext === "doc" || ext === "docx") return "Word Document"
        if (ext === "xls" || ext === "xlsx") return "Excel Spreadsheet"
        if (ext === "ppt" || ext === "pptx") return "PowerPoint Presentation"
        if (ext === "zip" || ext === "rar" || ext === "7z") return "Archive"
        if (ext === "jpg" || ext === "jpeg" || ext === "png" || ext === "gif") return "Image"
        if (ext === "mp3" || ext === "wav" || ext === "flac") return "Audio"
        if (ext === "mp4" || ext === "avi" || ext === "mkv") return "Video"
        if (ext === "txt") return "Text Document"
        if (ext === "html" || ext === "css" || ext === "js") return "Web Document"
        if (ext === "py") return "Python Script"
        if (ext === "cpp" || ext === "c" || ext === "h") return "C/C++ Source"
        if (ext === "java") return "Java Source"
        
        return ext.toUpperCase() + " File"
    }

    Component.onCompleted: {
        console.log("[QML] Application started")
        console.log("[QML] Relay server:", relayServerAddress, ":", relayServerPort)
    }
}