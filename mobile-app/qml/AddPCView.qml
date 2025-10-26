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
            text: "Add PC"
            font.pixelSize: 24
            font.bold: true
        }

        TextField {
            id: pcAddressField
            placeholderText: "PC Address"
            Layout.fillWidth: true
        }

        TextField {
            id: pcNameField
            placeholderText: "PC Name"
            Layout.fillWidth: true
        }

        Button {
            text: "Connect"
            Layout.fillWidth: true
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Back"
            Layout.fillWidth: true
        }
    }
}
