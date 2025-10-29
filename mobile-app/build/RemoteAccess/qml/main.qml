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

    // DARK THEME BACKGROUND
    color: "#0a0e27"

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
            fileManager.browseDirectory("/home")
        }
        
        onConnectionFailed: function(error) {
            console.log("[QML] FileManager connection failed:", error)
            errorDialog.text = "Connection failed: " + error
            errorDialog.open()
        }
        
        onClearFileList: {
            console.log("[QML] Clearing file list")
            fileListModel.clear()
        }
        
        onAddFileToList: function(name, path, type, size, isDirectory) {
            console.log("[QML] Adding file:", name, path, type, size, isDirectory)
            fileListModel.append({
                "name": name,
                "path": path,
                "type": type,
                "size": size,
                "isDirectory": isDirectory
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
                fileManager.browseDirectory(fileManager.currentPath)
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
            fileManager.connectToPC(pcId, relayServerAddress)
            stackView.push(remoteControlPage)
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
    }

    // MAIN MENU PAGE (IMAGE 1)
    Component {
        id: mainMenuPage
        
        Page {
            title: "Remote Access Mobile"
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
                                    console.log("[QML] Bind PC button clicked")
                                    stackView.push(bindPCPage)
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
                                            if (model.isOnline) {
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
                                            color: parent.pressed ? "#dc2626" : (parent.hovered ? "#ef4444" : "transparent")
                                            border.color: "#ef4444"
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

    // LOGIN PAGE (IMAGE 2)
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
                            id: serverAddressField
                            text: "127.0.0.1:8080"
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
                                connectionManager.setAccountServerAddress(serverAddressField.text)
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

                        Label {
                            text: "OR"
                            font.pixelSize: 14
                            color: "#6b7280"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            font.pixelSize: 16
                            
                            background: Rectangle {
                                color: "transparent"
                                border.color: "#00d9ff"
                                border.width: 2
                                radius: 8
                            }
                            
                            contentItem: RowLayout {
                                spacing: 10
                                
                                Label {
                                    text: "🔗"
                                    font.pixelSize: 18
                                    color: "#00d9ff"
                                }
                                
                                Label {
                                    text: "Bind a New PC"
                                    font.pixelSize: 16
                                    color: "#00d9ff"
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                            
                            onClicked: {
                                console.log("[QML] Bind a New PC clicked")
                                stackView.push(bindPCPage)
                            }
                        }

                        Label {
                            text: "You can bind PCs without logging in to the account server"
                            font.pixelSize: 12
                            color: "#6b7280"
                            Layout.alignment: Qt.AlignHCenter
                            wrapMode: Text.WordWrap
                            Layout.maximumWidth: 400
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }

    // BIND PC PAGE WITH QR CODE SCANNER
    Component {
        id: bindPCPage
        
        Page {
            title: "Bind New PC"
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

                // Main content centered
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

                        // Tab buttons for QR Code / Manual Entry
                        RowLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 15

                            Button {
                                id: qrCodeTabButton
                                text: "📱 Scan QR Code"
                                font.pixelSize: 14
                                Layout.preferredWidth: 160
                                Layout.preferredHeight: 45
                                checkable: true
                                checked: true
                                
                                background: Rectangle {
                                    color: parent.checked ? "#3d5a80" : "#1e2842"
                                    radius: 22
                                    border.color: parent.checked ? "#5a7ba6" : "#2a3a52"
                                    border.width: 2
                                }
                                
                                contentItem: Label {
                                    text: parent.text
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                
                                onClicked: {
                                    qrCodeTabButton.checked = true
                                    manualEntryTabButton.checked = false
                                }
                            }

                            Button {
                                id: manualEntryTabButton
                                text: "✏️ Manual Entry"
                                font.pixelSize: 14
                                Layout.preferredWidth: 160
                                Layout.preferredHeight: 45
                                checkable: true
                                checked: false
                                
                                background: Rectangle {
                                    color: parent.checked ? "#3d5a80" : "#1e2842"
                                    radius: 22
                                    border.color: parent.checked ? "#5a7ba6" : "#2a3a52"
                                    border.width: 2
                                }
                                
                                contentItem: Label {
                                    text: parent.text
                                    color: "#ffffff"
                                    font: parent.font
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                
                                onClicked: {
                                    manualEntryTabButton.checked = true
                                    qrCodeTabButton.checked = false
                                }
                            }
                        }

                        // QR Code Scanner View (shown when qrCodeTabButton is checked)
                        Rectangle {
                            Layout.preferredWidth: 350
                            Layout.preferredHeight: 350
                            Layout.alignment: Qt.AlignHCenter
                            color: "#ffffff"
                            radius: 12
                            visible: qrCodeTabButton.checked
                            
                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 20
                                
                                // QR Code Icon
                                Label {
                                    text: "⊞:⊟\n⊟:⊡"
                                    font.pixelSize: 80
                                    font.bold: true
                                    color: "#000000"
                                    Layout.alignment: Qt.AlignHCenter
                                    lineHeight: 0.8
                                }
                                
                                Label {
                                    text: "QR Scanner View"
                                    font.pixelSize: 14
                                    color: "#6b7280"
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                        
                        Label {
                            text: "Point your camera at the QR code displayed on\nthe PC"
                            font.pixelSize: 13
                            color: "#9ca3af"
                            Layout.alignment: Qt.AlignHCenter
                            horizontalAlignment: Text.AlignHCenter
                            visible: qrCodeTabButton.checked
                        }

                        // Instructions
                        ColumnLayout {
                            Layout.alignment: Qt.AlignHCenter
                            spacing: 12
                            visible: qrCodeTabButton.checked
                            
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
                                    text: "The program will display a QR code"
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
                                    text: "Scan the QR code with this device"
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
                                    console.log("[QML] Connecting to PC:", model.pcId)
                                    pcManager.connectToPC(model.pcId, relayServerAddress)
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
            background: Rectangle { color: "#0a0e27" }
            
            header: ToolBar {
                background: Rectangle { color: "#12172e" }
                
                RowLayout {
                    anchors.fill: parent
                    
                    ToolButton {
                        text: "←"
                        onClicked: {
                            pcManager.disconnectFromPC()
                            stackView.pop()
                        }
                    }
                    
                    Label {
                        text: "Connected to: " + pcManager.connectedPcId
                        font.pixelSize: 18
                        color: "#ffffff"
                        Layout.fillWidth: true
                    }
                    
                    ToolButton {
                        text: "✕"
                        onClicked: {
                            pcManager.disconnectFromPC()
                            stackView.pop()
                        }
                    }
                }
            }

            TabBar {
                id: tabBar
                width: parent.width
                background: Rectangle { color: "#12172e" }
                
                TabButton {
                    text: "Remote Desktop"
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                TabButton {
                    text: "File Browser"
                    contentItem: Label {
                        text: parent.text
                        color: "#ffffff"
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
                            color: "#ffffff"
                            Layout.alignment: Qt.AlignHCenter
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "black"
                            border.color: "#6c5ce7"
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

                // FILE BROWSER TAB - DARK THEME!!!
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            
                            Button {
                                text: "⌂"
                                onClicked: fileManager.goHome()
                            }
                            
                            Button {
                                text: "←"
                                onClicked: fileManager.goBack()
                            }
                            
                            Label {
                                text: fileManager.currentPath
                                font.pixelSize: 14
                                color: "#ffffff"
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                            }
                            
                            Button {
                                text: "⟳"
                                onClicked: fileManager.browseDirectory(fileManager.currentPath)
                            }
                            
                            Button {
                                text: "Upload"
                                onClicked: fileDialog.open()
                            }
                        }

                        // FILE LIST WITH DARK THEME!
                        ListView {
                            id: fileListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            
                            model: fileListModel
                            spacing: 8
                            
                            delegate: ItemDelegate {
                                width: fileListView.width
                                height: 70
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 15
                                    
                                    Rectangle {
                                        width: 50
                                        height: 50
                                        radius: 10
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: model.isDirectory ? "#6c5ce7" : "#fd79a8" }
                                            GradientStop { position: 1.0; color: model.isDirectory ? "#a29bfe" : "#ffeaa7" }
                                        }
                                        
                                        Label {
                                            anchors.centerIn: parent
                                            text: model.isDirectory ? "📁" : "📄"
                                            font.pixelSize: 28
                                        }
                                    }
                                    
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        
                                        // FILE NAME - BRIGHT WHITE!
                                        Label {
                                            text: model.name
                                            font.pixelSize: 16
                                            font.bold: true
                                            color: "#FFFFFF"  // PURE WHITE!
                                        }
                                        
                                        // FILE SIZE - BRIGHT CYAN!
                                        Label {
                                            text: model.isDirectory ? "📂 Folder" : "📊 " + formatFileSize(model.size)
                                            font.pixelSize: 13
                                            color: "#00d9ff"  // BRIGHT CYAN!
                                        }
                                    }
                                    
                                    Button {
                                        text: "Share"
                                        visible: !model.isDirectory
                                        Layout.preferredWidth: 90
                                        
                                        background: Rectangle {
                                            radius: 20
                                            gradient: Gradient {
                                                orientation: Gradient.Horizontal
                                                GradientStop { position: 0.0; color: "#00b894" }
                                                GradientStop { position: 1.0; color: "#6c5ce7" }
                                            }
                                        }
                                        
                                        contentItem: Label {
                                            text: "🔗 Share"
                                            color: "#ffffff"
                                            horizontalAlignment: Text.AlignHCenter
                                            font.bold: true
                                        }
                                        
                                        onClicked: {
                                            console.log("[QML] Creating share link for:", model.path)
                                            fileManager.createShareLink(model.path)
                                        }
                                    }
                                }
                                
                                onClicked: {
                                    if (model.isDirectory) {
                                        fileManager.browseDirectory(model.path)
                                    } else {
                                        fileManager.openFile(model.path)
                                    }
                                }
                                
                                // DARK BACKGROUND WITH HIGH CONTRAST!
                                background: Rectangle {
                                    color: parent.hovered ? "#1e2442" : "#161b33"
                                    border.color: parent.hovered ? "#6c5ce7" : "#2a2f4a"
                                    border.width: 1.5
                                    radius: 12
                                    
                                    Behavior on color { ColorAnimation { duration: 150 } }
                                    Behavior on border.color { ColorAnimation { duration: 150 } }
                                }
                            }
                            
                            ScrollBar.vertical: ScrollBar {
                                contentItem: Rectangle {
                                    implicitWidth: 10
                                    radius: 5
                                    color: "#6c5ce7"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select file to upload"
        fileMode: FileDialog.OpenFile
        onAccepted: {
            console.log("[QML] File selected:", selectedFile)
            fileManager.uploadFile(selectedFile, fileManager.currentPath)
        }
    }

    Dialog {
        id: uploadDialog
        title: "Uploading File"
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        
        background: Rectangle {
            color: "#12172e"
            border.color: "#6c5ce7"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            spacing: 20
            width: 300
            
            Label {
                text: "Uploading file..."
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
            }
            
            ProgressBar {
                from: 0
                to: 100
                value: fileManager.uploadProgress
                Layout.fillWidth: true
            }
            
            Label {
                text: fileManager.uploadProgress + "%"
                color: "#00cec9"
                Layout.alignment: Qt.AlignHCenter
                font.pixelSize: 18
            }
        }
    }

    Dialog {
        id: shareLinkDialog
        title: "Share Link Created"
        modal: true
        anchors.centerIn: parent
        
        property string shareLink: ""
        
        background: Rectangle {
            color: "#12172e"
            border.color: "#6c5ce7"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            spacing: 20
            width: 400
            
            Label {
                text: "Your shareable link:"
                font.bold: true
                color: "#ffffff"
            }
            
            TextField {
                id: linkField
                text: shareLinkDialog.shareLink
                readOnly: true
                Layout.fillWidth: true
                selectByMouse: true
            }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                
                Button {
                    text: "Copy Link"
                    Layout.fillWidth: true
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
                    onClicked: shareLinkDialog.close()
                }
            }
        }
    }

    Dialog {
        id: successDialog
        title: "Success"
        property alias text: successLabel.text
        modal: true
        anchors.centerIn: parent
        
        background: Rectangle {
            color: "#12172e"
            border.color: "#00cec9"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            spacing: 20
            
            Label {
                id: successLabel
                wrapMode: Text.WordWrap
                Layout.maximumWidth: 400
                color: "#ffffff"
            }
            
            Button {
                text: "OK"
                Layout.alignment: Qt.AlignHCenter
                onClicked: successDialog.close()
            }
        }
    }

    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorLabel.text
        modal: true
        anchors.centerIn: parent
        
        background: Rectangle {
            color: "#12172e"
            border.color: "#ff7675"
            border.width: 2
            radius: 16
        }
        
        ColumnLayout {
            spacing: 20
            
            Label {
                id: errorLabel
                wrapMode: Text.WordWrap
                Layout.maximumWidth: 400
                color: "#ffffff"
            }
            
            Button {
                text: "OK"
                Layout.alignment: Qt.AlignHCenter
                onClicked: errorDialog.close()
            }
        }
    }

    function formatFileSize(bytes) {
        if (bytes === 0) return "0 B"
        const k = 1024
        const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
        const i = Math.floor(Math.log(bytes) / Math.log(k))
        return (bytes / Math.pow(k, i)).toFixed(2) + ' ' + sizes[i]
    }

    Component.onCompleted: {
        console.log("[QML] Application started")
        console.log("[QML] Relay server:", relayServerAddress, ":", relayServerPort)
    }
}