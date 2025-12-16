import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// Import our singleton to access global application settings
import "../config" as App

Item {
    id: userMenuRoot
    property var parentWindow
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

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("UserMenu: no stack available");
        return s;
    }

    function go(title) {
        var s = stackRef();
        if (s) {
            s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/ModePlaceholder.qml"), { parentWindow: userMenuRoot.parentWindow, title: title })
        }
    }

    function goVolume() {
        var s = stackRef();
        if (s) {
            var page = s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/VolumeControl.qml"), { 
                parentWindow: userMenuRoot.parentWindow,
                // Pass the current volume level to the new page
                volumeLevel: App.AppSettings.volumeLevel 
            });
            // Connect to the new page's signal
            page.volumeChanged.connect(function(newVolume) {
                App.AppSettings.volumeLevel = newVolume;
            });
        }
    }

    function goBrightness() {
        var s = stackRef();
        if (s) {
            s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/BrightnessControl.qml"), { parentWindow: userMenuRoot.parentWindow });
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 14

        Text {
            text: "User Menu"
            font.pointSize: 24
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        ColumnLayout {
            spacing: 12
            Layout.fillWidth: true

            Button { text: "Auto Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: go("Auto Mode")
            }
            Button { text: "Manual Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: go("Manual Mode")
            }
            Button { text: "Weather Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: go("Weather Mode")
            }
            Button { text: "Adjust Volume"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: goVolume()
            }
            Button { text: "Adjust Brightness"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: goBrightness()
            }
            Button { text: "Display All Route"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: go("Display All Route")
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout { Layout.alignment: Qt.AlignHCenter; spacing: 12
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
