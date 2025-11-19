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
            text: "Settings"
            font.pixelSize: 24
            font.bold: true
        }

        CheckBox { text: "Enable Auto-Connect" }
        CheckBox { text: "Show Notifications" }
        Item { Layout.fillHeight: true }
    }
}
