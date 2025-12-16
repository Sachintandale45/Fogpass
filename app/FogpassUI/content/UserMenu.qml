import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var parentWindow
    property int volumeLevel: 75
    // batteryLevel is provided by the C++ backend as `backend.batteryLevel`
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    // Beautiful gradient background with mixed colors
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#667eea" }  // Purple-blue
            GradientStop { position: 0.5; color: "#764ba2" }  // Purple
            GradientStop { position: 1.0; color: "#f093fb" }  // Pink
        }
    }

    HeaderBar {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 16
        batteryLevel: typeof backend !== 'undefined' ? backend.batteryLevel : 0
        volumeLevel: root.volumeLevel
    }

    ColumnLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 24
        anchors.topMargin: 80
        spacing: 14

        Text {
            text: "User Menu"
            font.pointSize: 24
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }

        ColumnLayout {
            spacing: 12
            Layout.fillWidth: true

            function stackRef() {
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (!s) console.warn("UserMenu: no stack available");
                return s;
            }

            function go(title) {
                var s = stackRef();
                if (s) {
                    s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/ModePlaceholder.qml"), { parentWindow: root.parentWindow, title: title })
                }
            }

            function goVolume() {
                var s = stackRef();
                if (s) {
                    s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/VolumeControl.qml"), { parentWindow: root.parentWindow })
                }
            }

            Button { text: "1) Auto Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: go("Auto Mode")
            }
            Button { text: "2) Manual Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: go("Manual Mode")
            }
            Button { text: "3) Weather Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: go("Weather Mode")
            }
            Button { text: "4) Adjust Volume"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: goVolume()
            }
            Button { text: "5) Adjust Brightness"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: go("Adjust Brightness")
            }
            Button { text: "6) Display All Route"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignLeft
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter; anchors.left: parent.left; anchors.leftMargin: 12 }
                onClicked: go("Display All Route")
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout { Layout.alignment: Qt.AlignLeft; spacing: 12
            Button { 
                text: "Back"; 
                Layout.minimumWidth: 200
                Layout.minimumHeight: 50
                font.pointSize: 16
                font.bold: true
                background: Rectangle {
                    radius: 8
                    color: parent.pressed ? "#ffffff" : "#ffffff"
                    opacity: parent.pressed ? 0.9 : 1.0
                    border.width: 2
                    border.color: "#333333"
                }
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#333333"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: { var s = (parentWindow && parentWindow.stack) || StackView.view; if (s) s.pop(); else console.warn("UserMenu: no stack on back"); }
            }
        }
    }
}
