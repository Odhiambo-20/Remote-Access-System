import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Page {
    id: root
    title: "File Explorer"
    
    property var fileManager: null
    property string currentPath: "/"
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        
        // Current Path Display
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#f0f0f0"
            radius: 5
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                
                Label {
                    text: "Path:"
                    font.bold: true
                }
                
                Label {
                    text: currentPath
                    Layout.fillWidth: true
                }
                
                Button {
                    text: "⬆ Up"
                    onClicked: {
                        var parts = currentPath.split('/')
                        parts.pop()
                        currentPath = parts.join('/') || '/'
                        fileManager.listDirectory(currentPath)
                    }
                }
                
                Button {
                    text: "🔄 Refresh"
                    onClicked: {
                        fileManager.listDirectory(currentPath)
                    }
                }
            }
        }
        
        // File List
        ListView {
            id: fileListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: fileListModel
            spacing: 5
            clip: true
            
            delegate: Rectangle {
                width: ListView.view.width
                height: 60
                color: mouseArea.containsMouse ? "#e0e0e0" : "white"
                radius: 3
                border.color: "#cccccc"
                border.width: 1
                
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    
                    onClicked: {
                        if (model.isDirectory) {
                            currentPath = currentPath + "/" + model.name
                            fileManager.listDirectory(currentPath)
                        }
                    }
                    
                    onPressAndHold: {
                        contextMenu.fileName = model.name
                        contextMenu.filePath = currentPath + "/" + model.name
                        contextMenu.popup()
                    }
                }
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10
                    
                    Label {
                        text: model.isDirectory ? "📁" : "📄"
                        font.pixelSize: 24
                    }
                    
                    Column {
                        Layout.fillWidth: true
                        
                        Label {
                            text: model.name
                            font.pixelSize: 14
                            font.bold: true
                        }
                        
                        Label {
                            text: model.size
                            font.pixelSize: 11
                            color: "#666666"
                        }
                    }
                    
                    Button {
                        text: "..."
                        visible: !model.isDirectory
                        onClicked: {
                            contextMenu.fileName = model.name
                            contextMenu.filePath = currentPath + "/" + model.name
                            contextMenu.popup()
                        }
                    }
                }
            }
        }
        
        // Action Buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Button {
                text: "📤 Upload"
                Layout.fillWidth: true
                onClicked: fileDialog.open()
            }
            
            Button {
                text: "🔙 Back"
                Layout.fillWidth: true
                onClicked: stackView.pop()
            }
        }
        
        // Progress Bar
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            from: 0
            to: 100
            value: 0
            visible: false
        }
        
        Label {
            id: statusLabel
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            color: "blue"
        }
    }
    
    // File List Model
    ListModel {
        id: fileListModel
    }
    
    // Context Menu for file operations
    Menu {
        id: contextMenu
        property string fileName: ""
        property string filePath: ""
        
        MenuItem {
            text: "📥 Download"
            onTriggered: {
                statusLabel.text = "Downloading " + contextMenu.fileName + "..."
                progressBar.visible = true
                progressBar.value = 0
                fileManager.downloadFile(contextMenu.filePath, "/tmp/" + contextMenu.fileName)
            }
        }
        
        MenuItem {
            text: "🔗 Share Link"
            onTriggered: {
                statusLabel.text = "Generating share link..."
                fileManager.generateShareLink(contextMenu.filePath, 24)
            }
        }
        
        MenuItem {
            text: "🗑 Delete"
            onTriggered: {
                statusLabel.text = "Deleting " + contextMenu.fileName + "..."
                fileManager.deleteFile(contextMenu.filePath)
            }
        }
    }
    
    // File Upload Dialog
    FileDialog {
        id: fileDialog
        title: "Select file to upload"
        onAccepted: {
            var fileName = fileDialog.selectedFile.toString().split('/').pop()
            statusLabel.text = "Uploading " + fileName + "..."
            progressBar.visible = true
            progressBar.value = 0
            fileManager.uploadFile(fileDialog.selectedFile, currentPath + "/" + fileName)
        }
    }
    
    // Connect FileManager signals
    Connections {
        target: fileManager
        
        function onDirectoryListed(entries) {
            console.log("[FileExplorer] Directory listed:", entries.length, "entries")
            fileListModel.clear()
            
            for (var i = 0; i < entries.length; i++) {
                var parts = entries[i].split('|')
                if (parts.length >= 3) {
                    fileListModel.append({
                        name: parts[0],
                        size: parts[1],
                        isDirectory: parts[2] === "DIR"
                    })
                }
            }
        }
        
        function onUploadProgress(percent) {
            progressBar.value = percent
        }
        
        function onDownloadProgress(percent) {
            progressBar.value = percent
        }
        
        function onShareLinkGenerated(url) {
            statusLabel.text = "Share link: " + url
            shareLinkDialog.shareUrl = url
            shareLinkDialog.open()
        }
        
        function onCompleted() {
            statusLabel.text = "Operation completed successfully"
            progressBar.visible = false
            fileManager.listDirectory(currentPath)
        }
        
        function onError(message) {
            statusLabel.text = "Error: " + message
            progressBar.visible = false
        }
    }
    
    // Share Link Dialog
    Dialog {
        id: shareLinkDialog
        title: "Share Link Generated"
        property string shareUrl: ""
        
        standardButtons: Dialog.Ok | Dialog.Close
        
        ColumnLayout {
            spacing: 10
            
            Label {
                text: "Share this link with others:"
                font.bold: true
            }
            
            TextField {
                id: linkField
                text: shareLinkDialog.shareUrl
                readOnly: true
                selectByMouse: true
                Layout.fillWidth: true
                Layout.minimumWidth: 400
            }
            
            Button {
                text: "📋 Copy to Clipboard"
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    linkField.selectAll()
                    linkField.copy()
                    statusLabel.text = "Link copied to clipboard!"
                }
            }
            
            Label {
                text: "Link expires in 24 hours"
                font.pixelSize: 10
                color: "#666666"
            }
        }
    }
    
    Component.onCompleted: {
        if (fileManager) {
            fileManager.listDirectory(currentPath)
        }
    }
}
