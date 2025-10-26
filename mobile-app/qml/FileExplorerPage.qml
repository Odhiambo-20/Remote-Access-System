import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import QtQuick.Effects

Page {
    id: root
    title: "File Explorer"
    
    property string currentPcId: ""
    property string currentPath: fileManager.currentPath
    
    // DARK BACKGROUND - NO WHITE!
    background: Rectangle {
        color: "#0a0e27"
    }
    
    header: Rectangle {
        height: 80
        color: "#12172e"
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#000000"
            shadowBlur: 0.4
            shadowVerticalOffset: 4
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12
            
            // Back Button
            Button {
                implicitWidth: 50
                implicitHeight: 50
                enabled: !fileManager.isBusy && !fileManager.isUploading
                
                background: Rectangle {
                    radius: 25
                    color: parent.enabled ? (parent.hovered ? "#6c5ce7" : "#5649c7") : "#2d3436"
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                
                contentItem: Text {
                    text: "←"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: fileManager.goBack()
            }
            
            // Home Button
            Button {
                implicitWidth: 50
                implicitHeight: 50
                enabled: !fileManager.isBusy && !fileManager.isUploading
                
                background: Rectangle {
                    radius: 25
                    color: parent.enabled ? (parent.hovered ? "#00cec9" : "#00b894") : "#2d3436"
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                
                contentItem: Text {
                    text: "⌂"
                    font.pixelSize: 26
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: fileManager.goHome()
            }
            
            // Path Display
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 25
                color: "#1a1f3a"
                border.color: "#6c5ce7"
                border.width: 1.5
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    spacing: 8
                    
                    Text {
                        text: "📁"
                        font.pixelSize: 20
                    }
                    
                    Label {
                        Layout.fillWidth: true
                        text: currentPath
                        elide: Label.ElideMiddle
                        font.pixelSize: 15
                        font.weight: Font.Medium
                        color: "#e1e8f0"
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            
            // Refresh Button
            Button {
                implicitWidth: 50
                implicitHeight: 50
                enabled: !fileManager.isBusy && !fileManager.isUploading
                
                background: Rectangle {
                    radius: 25
                    color: parent.enabled ? (parent.hovered ? "#fd79a8" : "#e84393") : "#2d3436"
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
                
                contentItem: Text {
                    text: "↻"
                    font.pixelSize: 26
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: fileManager.browseDirectory(currentPath)
            }
            
            // Upload Button
            Button {
                implicitWidth: 120
                implicitHeight: 50
                enabled: !fileManager.isBusy && !fileManager.isUploading
                
                background: Rectangle {
                    radius: 25
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: parent.parent.enabled ? "#6c5ce7" : "#2d3436" }
                        GradientStop { position: 1.0; color: parent.parent.enabled ? "#e84393" : "#2d3436" }
                    }
                }
                
                contentItem: RowLayout {
                    spacing: 8
                    
                    Text {
                        text: "⬆"
                        font.pixelSize: 20
                        color: "#ffffff"
                    }
                    
                    Text {
                        text: "Upload"
                        font.pixelSize: 15
                        font.bold: true
                        color: "#ffffff"
                    }
                }
                
                onClicked: fileDialog.open()
            }
        }
    }
    
    FileDialog {
        id: fileDialog
        title: "Select file to upload"
        onAccepted: {
            console.log("[QML] File selected:", fileDialog.currentFile)
            fileManager.uploadFile(fileDialog.currentFile, currentPath)
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // DARK CARD CONTAINER - Files will be clearly visible!
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "#12172e"  // DARK BACKGROUND
            border.color: "#6c5ce7"
            border.width: 1
            
            ListView {
                id: fileListView
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10
                clip: true
                
                model: fileManager.fileList
                
                // INLINE DELEGATE - NO EXTERNAL COMPONENT
                delegate: Rectangle {
                    width: fileListView.width - 24
                    height: 80
                    radius: 12
                    // DARK BACKGROUNDS - HIGH CONTRAST!
                    color: fileMouseArea.containsMouse ? "#1e2442" : "#161b33"
                    border.color: fileMouseArea.containsMouse ? "#6c5ce7" : "#2a2f4a"
                    border.width: 1.5
                    
                    Behavior on color { ColorAnimation { duration: 150 } }
                    Behavior on border.color { ColorAnimation { duration: 150 } }
                    
                    MouseArea {
                        id: fileMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: model.isDirectory ? Qt.PointingHandCursor : Qt.ArrowCursor
                        
                        onClicked: {
                            if (model.isDirectory) {
                                console.log("[QML] Opening directory:", model.path)
                                fileManager.browseDirectory(model.path)
                            }
                        }
                    }
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 16
                        
                        // Icon with gradient
                        Rectangle {
                            width: 55
                            height: 55
                            radius: 12
                            
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: model.isDirectory ? "#6c5ce7" : "#fd79a8" }
                                GradientStop { position: 1.0; color: model.isDirectory ? "#a29bfe" : "#ffeaa7" }
                            }
                            
                            Text {
                                anchors.centerIn: parent
                                text: model.isDirectory ? "📁" : "📄"
                                font.pixelSize: 32
                            }
                        }
                        
                        // File Info - WHITE TEXT ON DARK BACKGROUND!
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            
                            // FILE NAME - BRIGHT WHITE!
                            Label {
                                Layout.fillWidth: true
                                text: model.name
                                font.pixelSize: 17
                                font.bold: true
                                color: "#FFFFFF"  // PURE WHITE - MAXIMUM CONTRAST!
                                elide: Text.ElideRight
                                maximumLineCount: 1
                                
                                background: Rectangle {
                                    color: "transparent"  // Ensure no white background
                                }
                            }
                            
                            // FILE SIZE - BRIGHT CYAN!
                            Label {
                                text: model.isDirectory ? "📂 Folder" : "📊 " + formatFileSize(model.size)
                                font.pixelSize: 14
                                color: "#00d9ff"  // BRIGHT CYAN - HIGH VISIBILITY!
                                font.weight: Font.Medium
                                
                                background: Rectangle {
                                    color: "transparent"
                                }
                            }
                        }
                        
                        // Share Button - Only for files
                        Button {
                            visible: !model.isDirectory
                            implicitWidth: 110
                            implicitHeight: 45
                            
                            background: Rectangle {
                                radius: 22
                                gradient: Gradient {
                                    orientation: Gradient.Horizontal
                                    GradientStop { position: 0.0; color: parent.parent.hovered ? "#00e5d0" : "#00b894" }
                                    GradientStop { position: 1.0; color: parent.parent.hovered ? "#8e7ef5" : "#6c5ce7" }
                                }
                                
                                Behavior on opacity { NumberAnimation { duration: 200 } }
                            }
                            
                            contentItem: RowLayout {
                                spacing: 8
                                
                                Text {
                                    text: "🔗"
                                    font.pixelSize: 18
                                }
                                
                                Text {
                                    text: "Share"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#ffffff"
                                }
                            }
                            
                            onClicked: {
                                console.log("[QML] Creating share link for:", model.path)
                                fileManager.createShareLink(model.path)
                            }
                        }
                    }
                }
                
                // Custom Scrollbar
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    
                    contentItem: Rectangle {
                        implicitWidth: 10
                        radius: 5
                        color: parent.pressed ? "#a29bfe" : (parent.hovered ? "#6c5ce7" : "#74b9ff")
                        opacity: parent.active ? 0.8 : 0.4
                        
                        Behavior on color { ColorAnimation { duration: 200 } }
                        Behavior on opacity { NumberAnimation { duration: 200 } }
                    }
                    
                    background: Rectangle {
                        color: "#1a1f3a"
                        radius: 5
                        opacity: 0.3
                    }
                }
            }
        }
        
        // Upload Progress Card
        Rectangle {
            Layout.fillWidth: true
            height: 100
            visible: fileManager.isUploading
            radius: 16
            color: "#12172e"
            border.color: "#6c5ce7"
            border.width: 2
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    
                    Text {
                        text: "⬆️"
                        font.pixelSize: 24
                    }
                    
                    Label {
                        text: "Uploading your file..."
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Label {
                        text: fileManager.uploadProgress + "%"
                        font.pixelSize: 18
                        font.bold: true
                        color: "#00cec9"
                    }
                }
                
                // Progress Bar
                Rectangle {
                    Layout.fillWidth: true
                    height: 12
                    radius: 6
                    color: "#1a1f3a"
                    
                    Rectangle {
                        width: parent.width * (fileManager.uploadProgress / 100)
                        height: parent.height
                        radius: parent.radius
                        
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: "#6c5ce7" }
                            GradientStop { position: 0.5; color: "#00cec9" }
                            GradientStop { position: 1.0; color: "#fd79a8" }
                        }
                        
                        Behavior on width {
                            NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
                        }
                    }
                }
            }
        }
    }
    
    // Loading Overlay
    Rectangle {
        anchors.fill: parent
        visible: fileManager.isBusy
        color: "#0a0e27"
        opacity: 0.95
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20
            
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 120
                height: 120
                radius: 60
                color: "#12172e"
                border.color: "#6c5ce7"
                border.width: 3
                
                // Animated Spinner
                Repeater {
                    model: 8
                    Rectangle {
                        width: 8
                        height: 24
                        radius: 4
                        color: "#6c5ce7"
                        x: parent.width / 2 - width / 2
                        y: 20
                        transformOrigin: Item.Bottom
                        
                        opacity: 1.0 - (index * 0.1)
                        
                        transform: Rotation {
                            origin.x: 4
                            origin.y: 40
                            angle: index * 45
                            
                            SequentialAnimation on angle {
                                loops: Animation.Infinite
                                NumberAnimation {
                                    from: index * 45
                                    to: index * 45 + 360
                                    duration: 1200
                                }
                            }
                        }
                    }
                }
            }
            
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Loading..."
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
            }
        }
    }
    
    function formatFileSize(bytes) {
        if (bytes === 0) return "0 B"
        const k = 1024
        const sizes = ["B", "KB", "MB", "GB"]
        const i = Math.floor(Math.log(bytes) / Math.log(k))
        return Math.round((bytes / Math.pow(k, i)) * 100) / 100 + " " + sizes[i]
    }
    
    Connections {
        target: fileManager
        
        function onShareLinkCreated(url) {
            shareLinkDialog.shareUrl = url
            shareLinkDialog.open()
        }
        
        function onUploadFinished(success, message) {
            if (success) {
                successPopup.show()
                fileManager.browseDirectory(currentPath)
            } else {
                errorDialog.errorMessage = message
                errorDialog.open()
            }
        }
    }
    
    // Success Popup
    Popup {
        id: successPopup
        anchors.centerIn: parent
        width: 340
        height: 200
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        background: Rectangle {
            radius: 20
            color: "#12172e"
            border.color: "#00cec9"
            border.width: 2
        }
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20
            
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 80
                height: 80
                radius: 40
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#00cec9" }
                    GradientStop { position: 1.0; color: "#00b894" }
                }
                
                Text {
                    anchors.centerIn: parent
                    text: "✓"
                    font.pixelSize: 48
                    font.bold: true
                    color: "#ffffff"
                }
            }
            
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Upload Successful!"
                font.pixelSize: 20
                font.bold: true
                color: "#ffffff"
            }
        }
        
        function show() {
            open()
            closeTimer.start()
        }
        
        Timer {
            id: closeTimer
            interval: 2000
            onTriggered: successPopup.close()
        }
    }
    
    // Error Dialog
    Dialog {
        id: errorDialog
        anchors.centerIn: parent
        width: 420
        modal: true
        
        property string errorMessage: ""
        
        background: Rectangle {
            radius: 20
            color: "#12172e"
            border.color: "#ff7675"
            border.width: 2
        }
        
        header: Rectangle {
            width: parent.width
            height: 60
            radius: 20
            color: "#1a1f3a"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12
                
                Text {
                    text: "⚠️"
                    font.pixelSize: 28
                }
                
                Label {
                    text: "Error"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ff7675"
                }
            }
        }
        
        contentItem: Label {
            text: errorDialog.errorMessage
            wrapMode: Text.WordWrap
            color: "#ffffff"
            font.pixelSize: 15
            padding: 20
        }
        
        standardButtons: Dialog.Ok
    }
    
    // Share Link Dialog
    Dialog {
        id: shareLinkDialog
        anchors.centerIn: parent
        width: 540
        modal: true
        
        property string shareUrl: ""
        
        background: Rectangle {
            radius: 20
            color: "#12172e"
            border.color: "#6c5ce7"
            border.width: 2
        }
        
        header: Rectangle {
            width: parent.width
            height: 60
            radius: 20
            color: "#1a1f3a"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12
                
                Text {
                    text: "🔗"
                    font.pixelSize: 28
                }
                
                Label {
                    text: "Share Link Created"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#6c5ce7"
                }
            }
        }
        
        contentItem: ColumnLayout {
            spacing: 20
            
            Label {
                text: "Your share link is ready! Copy and share:"
                color: "#b2bec3"
                font.pixelSize: 14
            }
            
            Rectangle {
                Layout.fillWidth: true
                height: 60
                radius: 12
                color: "#1a1f3a"
                border.color: "#00cec9"
                border.width: 1.5
                
                TextEdit {
                    id: urlText
                    anchors.fill: parent
                    anchors.margins: 16
                    text: shareLinkDialog.shareUrl
                    color: "#00cec9"
                    font.pixelSize: 14
                    readOnly: true
                    selectByMouse: true
                    wrapMode: Text.Wrap
                    verticalAlignment: Text.AlignVCenter
                }
            }
            
            Button {
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: 160
                implicitHeight: 50
                
                background: Rectangle {
                    radius: 25
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: parent.parent.hovered ? "#00cec9" : "#00b894" }
                        GradientStop { position: 1.0; color: parent.parent.hovered ? "#a29bfe" : "#6c5ce7" }
                    }
                }
                
                contentItem: RowLayout {
                    spacing: 10
                    
                    Text {
                        text: "📋"
                        font.pixelSize: 20
                    }
                    
                    Text {
                        text: "Copy Link"
                        font.pixelSize: 15
                        font.bold: true
                        color: "#ffffff"
                    }
                }
                
                onClicked: {
                    urlText.selectAll()
                    urlText.copy()
                }
            }
        }
        
        standardButtons: Dialog.Close
    }
}