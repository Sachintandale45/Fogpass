import QtQuick 2.15
import QtQuick.Controls 2.15
import trial1 1.0

Rectangle {
    id: headerBar
    width: parent.width
    height: 60
    color: "transparent" // Match parent window color

    Row {
        id: contentRow
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
        height: parent.height

        // Operation Mode Indicator (Rectangular)
        Rectangle {
            width: 80
            height: 30
            color: "black"
            border.color: "#ffffff"
            border.width: 1
            radius: 4
            anchors.verticalCenter: parent.verticalCenter

            Text {
                anchors.centerIn: parent
                text: Backend.operationModeLabel
                color: "#ffffff"
                font.pixelSize: 12
                font.bold: true
            }
        }

        // Status LED (Yellow Circle)
        Rectangle {
            id: statusLed
            width: 18
            height: 18
            radius: 9
            color: "#ffeb3b" // Yellow
            border.color: "#ffffff"
            border.width: 1
            anchors.verticalCenter: parent.verticalCenter

            Timer {
                interval: 500
                repeat: true
                running: !Backend.isGnssStable
                onTriggered: statusLed.opacity = (statusLed.opacity === 1.0 ? 0.0 : 1.0)
                onRunningChanged: if (!running) statusLed.opacity = 1.0
            }
        }

        // Battery Indicator
        Rectangle {
            id: batteryIndicator
            width: 60
            height: 30
            border.color: "white"
            border.width: 2
            radius: 5
            anchors.verticalCenter: parent.verticalCenter
            color: "transparent"

            Rectangle {
                id: batteryFill
                // Set initial state
                width: (batteryIndicator.width - 4) * (AppSettings.batteryLevel / 100)
                color: AppSettings.batteryLevel > 20 ? "green" : "red"
                height: parent.height - 4
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2

            }

            Text {
                id: batteryText
                anchors.centerIn: parent
                color: "white"
                font.pixelSize: 16
                font.bold: true
                text: AppSettings.batteryLevel + "%"

            }
        }

        // Volume Control
        Rectangle {
            id: volumeIcon
            width: 70 // Increased width to accommodate text
            height: 40
            color: "transparent"
            anchors.verticalCenter: parent.verticalCenter

            Text {
                id: volumeText
                font.pixelSize: 20
                color: "white"
                anchors.centerIn: parent
                text: "🔊 " + AppSettings.volumeLevel

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
                text: AppSettings.currentMode === "Foggy" ? "F" : "NF"

            }
        }
    }
}