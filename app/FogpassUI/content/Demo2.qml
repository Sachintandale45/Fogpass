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
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Manual Mode Landmarks"
            font.pixelSize: 28
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            width: 400
            height: 150
            color: "#00000033"
            radius: 8
            Layout.alignment: Qt.AlignHCenter

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15

                Text { text: (Backend.landmark1 || "N/A") + " - " + (Backend.distance1 || "0") + " m"; color: "white"; font.pixelSize: 22 }
                Text { text: (Backend.landmark2 || "N/A") + " - " + (Backend.distance2 || "0") + " m"; color: "white"; font.pixelSize: 22 }
                Text { text: (Backend.landmark3 || "N/A") + " - " + (Backend.distance3 || "0") + " m"; color: "white"; font.pixelSize: 22 }
            }
        }

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