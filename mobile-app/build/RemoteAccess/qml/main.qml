import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
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
        initialItem: loginPage
    }

    Component {
        id: loginPage
        
        Page {
            title: "Login"
            background: Rectangle { color: "#0a0e27" }
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 20
                width: 300

                Label {
                    text: "Remote Access Login"
                    font.pixelSize: 24
                    color: "#ffffff"
                    Layout.alignment: Qt.AlignHCenter
                }

                TextField {
                    id: serverAddressField
                    placeholderText: "Account Server Address"
                    text: "127.0.0.1:8080"
                    Layout.fillWidth: true
                }

                TextField {
                    id: usernameField
                    placeholderText: "Username"
                    text: "victor"
                    Layout.fillWidth: true
                }

                TextField {
                    id: passwordField
                    placeholderText: "Password"
                    echoMode: TextInput.Password
                    text: "password"
                    Layout.fillWidth: true
                }

                Button {
                    text: "Login"
                    Layout.fillWidth: true
                    onClicked: {
                        console.log("[QML] Login button clicked")
                        connectionManager.setAccountServerAddress(serverAddressField.text)
                        connectionManager.login(usernameField.text, passwordField.text)
                        stackView.push(pcListPage)
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
