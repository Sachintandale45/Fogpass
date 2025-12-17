import QtQuick 2.15
import QtQuick.Controls 2.15
import trial1 1.0
import trial1.config 1.0

Rectangle {
    id: headerBar
    width: parent.width
    height: 60
    color: "#333333"

    Row {
        id: contentRow
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
        height: parent.height

        // Volume Control
        Rectangle {
            id: volumeIcon
            width: 40
            height: 40
            color: "transparent"
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: volumeText
                font.pixelSize: 20
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
                // Use a Binding to ensure the text is always updated.
                Binding { target: volumeText; property: "text"; value: "🔊 " + AppSettings.volumeLevel }
                text: "🔊 " + AppSettings.volumeLevel

                // Explicitly react to changes in the AppSettings singleton
                Connections {
                    target: AppSettings
                    function onVolumeLevelChanged() { volumeText.text = "🔊 " + AppSettings.volumeLevel; }
                }
            }
        }

        // Mode Indicator
        Rectangle {
            id: modeIndicator
            width: 40
            height: 40
            radius: 20
            color: "#555555"
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: modeText
                font.bold: true
                font.pixelSize: 20
                color: "white"
                anchors.centerIn: parent
                // Use a Binding to ensure the text is always updated.
                Binding { target: modeText; property: "text"; value: AppSettings.currentMode === "Foggy" ? "F" : "NF" }
                text: AppSettings.currentMode === "Foggy" ? "F" : "NF"

                // Explicitly react to changes in the AppSettings singleton
                Connections {
                    target: AppSettings
                    function onCurrentModeChanged() { modeText.text = AppSettings.currentMode === "Foggy" ? "F" : "NF"; }
                }
            }
        }

        // Battery Indicator
        Rectangle {
            id: batteryIndicator
            width: 60
            height: 30
            color: "transparent"
            border.color: "white"
            border.width: 2
            radius: 5
            anchors.verticalCenter: parent.verticalCenter

            Rectangle {
                id: batteryFill
                height: parent.height - 4
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2
                Binding { target: batteryFill; property: "width"; value: (batteryIndicator.width - 4) * (AppSettings.batteryLevel / 100) }
                Binding { target: batteryFill; property: "color"; value: AppSettings.batteryLevel > 20 ? "green" : "red" }

                // Explicitly react to changes in the AppSettings singleton
                Connections {
                    target: AppSettings
                    function onBatteryLevelChanged() {
                        batteryFill.width = (batteryIndicator.width - 4) * (AppSettings.batteryLevel / 100);
                        batteryFill.color = AppSettings.batteryLevel > 20 ? "green" : "red";
                    }
                }
            }
        }
    }
}