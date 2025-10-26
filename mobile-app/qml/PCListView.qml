import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f5f5f5"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "Remote PCs"
            font.pixelSize: 24
            font.bold: true
        }

        ListView {
            id: pcList
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            model: ListModel {
                id: pcListModel
            }

            delegate: Rectangle {
                width: pcList.width
                height: 80
                color: "#ffffff"
                border.color: "#e0e0e0"
                border.width: 1
                radius: 8

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 4

                    Text {
                        text: model.pcName || "Unknown PC"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        text: "Status: " + (model.status || "Offline")
                        font.pixelSize: 12
                        color: "#666666"
                    }
                }
            }
        }

        Button {
            text: "Add PC"
            Layout.fillWidth: true
        }
    }
}
