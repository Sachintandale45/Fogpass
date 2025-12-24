import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2c3e50" }
            GradientStop { position: 1.0; color: "#3498db" }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Auto Route Feature"
            font.pointSize: 28
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "This feature is under development."
            font.pointSize: 18
            color: "#ecf0f1"
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            text: "Back"
            Layout.topMargin: 20
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumWidth: 180
            Layout.minimumHeight: 50
            font.pointSize: 16
            background: Rectangle {
                color: "#3498db"
                radius: 8
                border.color: "#ffffff"
                border.width: 2
            }
            onClicked: {
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (s) s.pop();
            }
        }
    }
}