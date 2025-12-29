import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// This page is replaced by GPSSimulationPage2.qml

Page {
    id: root
    title: "GNSS Simulation"

    background: Rectangle {
        color: "black"
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 40

        Label {
            text: "GNSS Source Control"
            color: "white"
            font.pixelSize: 28
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // Button to Enable Simulation (Mode 1)
        Button {
            text: "Activate Simulation"
            Layout.preferredWidth: 300
            Layout.preferredHeight: 70

            contentItem: Text {
                text: parent.text
                font.pixelSize: 22
                font.bold: true
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: parent.down ? "#aaaaaa" : "#4CAF50" // Green
                radius: 10
            }

            onClicked: {
                console.log("UI: Activating Simulation Mode")
                // true -> 1 (Simulation)
                Backend.setGnssMode(true)
            }
        }

        // Button to Disable Simulation (Mode 0 - Real GPS)
        Button {
            text: "Deactivate Simulation"
            Layout.preferredWidth: 300
            Layout.preferredHeight: 70

            contentItem: Text {
                text: parent.text
                font.pixelSize: 22
                font.bold: true
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: parent.down ? "#555555" : "#F44336" // Red
                radius: 10
            }

            onClicked: {
                console.log("UI: Deactivating Simulation (Switching to Real GNSS)")
                // false -> 0 (Real)
                Backend.setGnssMode(false)
            }
        }
        
        Label {
            text: "Note: Switching mode restarts the GNSS subsystem."
            color: "gray"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        // Back Button
        Button {
            text: "Back"
            Layout.preferredWidth: 300
            Layout.preferredHeight: 70
            Layout.alignment: Qt.AlignHCenter

            contentItem: Text {
                text: parent.text
                font.pixelSize: 22
                font.bold: true
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: parent.down ? "#aaaaaa" : "#333333" // Dark Gray
                radius: 10
                border.color: "white"
                border.width: 1
            }

            onClicked: {
                if (root.StackView.view) root.StackView.view.pop()
            }
        }
    }
}
