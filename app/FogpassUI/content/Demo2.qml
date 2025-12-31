import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import trial1 1.0 // For AppSettings

Item {
    id: demo2Root

    // This property is set by UserMenu.qml when this page is pushed
    property var parentWindow

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("Demo2: no stack available");
        return s;
    }

    // Debug: Log backend values to console to verify QML access
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: console.log("Demo2 Backend Data:", Backend.landmark1, Backend.distance1)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        Text {
            text: "Manual Mode Landmarks"
            font.pixelSize: 24
            font.bold: true
            color: "#aaaaaa"
            Layout.alignment: Qt.AlignLeft
            Layout.topMargin: 10
        }

        // Spacer to push content to center vertically
        Item { Layout.fillHeight: true }

        // Landmark 1 - Double size (approx 2 inch visual)
        Text {
            text: (Backend.landmark1 || "N/A") + " - " + (Backend.distance1 || "0") + " m"
            color: "#4fc3f7" // Highlight color
            font.pixelSize: 100
            font.bold: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
        }

        // Landmark 2 & 3 - Standard size (approx 1 inch visual)
        Text {
            text: (Backend.landmark2 || "N/A") + " - " + (Backend.distance2 || "0") + " m"
            color: "white"
            font.pixelSize: 60
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
        }
        Text {
            text: (Backend.landmark3 || "N/A") + " - " + (Backend.distance3 || "0") + " m"
            color: "white"
            font.pixelSize: 60
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
        }

        // Spacer
        Item { Layout.fillHeight: true }

        Button {
            text: "Back"
            Layout.minimumWidth: 160
            Layout.minimumHeight: 46
            font.pointSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter

            background: Rectangle {
                radius: 8
                color: parent.pressed ? "#ffffff" : "#ffffff"
                opacity: parent.pressed ? 0.9 : 1.0
                border.width: 2
                border.color: "#333333"
            }
            contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }

            onClicked: {
                Backend.ClearRoute();
                var s = demo2Root.stackRef();
                if (s) s.pop();
            }
        }
    }
}