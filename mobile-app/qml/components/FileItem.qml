import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: fileItemRoot
    
    property string fileName: ""
    property string filePath: ""
    property bool isDirectory: false
    property string fileSize: ""
    
    signal clicked()
    signal shareClicked()
    
    width: parent.width
    height: 80
    radius: 12
    
    // DARK BACKGROUND!
    color: fileMouseArea.containsMouse ? "#1e2442" : "#161b33"
    border.color: fileMouseArea.containsMouse ? "#6c5ce7" : "#2a2f4a"
    border.width: 1.5
    
    Behavior on color { ColorAnimation { duration: 150 } }
    Behavior on border.color { ColorAnimation { duration: 150 } }
    
    MouseArea {
        id: fileMouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: isDirectory ? Qt.PointingHandCursor : Qt.ArrowCursor
        
        onClicked: fileItemRoot.clicked()
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
                GradientStop { position: 0.0; color: isDirectory ? "#6c5ce7" : "#fd79a8" }
                GradientStop { position: 1.0; color: isDirectory ? "#a29bfe" : "#ffeaa7" }
            }
            
            Text {
                anchors.centerIn: parent
                text: isDirectory ? "📁" : "📄"
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
                text: fileName
                font.pixelSize: 17
                font.bold: true
                color: "#FFFFFF"  // PURE WHITE!
                elide: Text.ElideRight
                maximumLineCount: 1
            }
            
            // FILE SIZE - BRIGHT CYAN!
            Label {
                text: isDirectory ? "📂 Folder" : "📊 " + fileSize
                font.pixelSize: 14
                color: "#00d9ff"  // BRIGHT CYAN!
                font.weight: Font.Medium
            }
        }
        
        // Share Button - Only for files
        Button {
            visible: !isDirectory
            implicitWidth: 110
            implicitHeight: 45
            
            background: Rectangle {
                radius: 22
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: parent.parent.hovered ? "#00e5d0" : "#00b894" }
                    GradientStop { position: 1.0; color: parent.parent.hovered ? "#8e7ef5" : "#6c5ce7" }
                }
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
            
            onClicked: fileItemRoot.shareClicked()
        }
    }
}
