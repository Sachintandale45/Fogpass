import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
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

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 110
        spacing: 10
        width: parent ? parent.width * 0.8 : 640

    Text {
            id: titleText
            text: "FOGPASS & Neuromotion Systems"
        font.pointSize: 36
        font.bold: true
            color: "#ffffff"
        horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "H/W VERSION: 1.0.0"
            font.pointSize: 22
            color: "#ffffff"
            horizontalAlignment: Text.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }

        Text {
            text: "S/W VERSION: 1.0.0"
            font.pointSize: 22
            color: "#ffffff"
            horizontalAlignment: Text.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }

        Text {
            text: "DB VERSION: 1.0.0"
            font.pointSize: 22
            color: "#ffffff"
            horizontalAlignment: Text.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }

        Text {
            text: "UNIT ID: 001"
            font.pointSize: 22
            color: "#ffffff"
            horizontalAlignment: Text.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }
    }

    Timer {
        id: autoTimer
        interval: 5000
        repeat: false
        running: true
        onTriggered: {
            if (parentWindow && parentWindow.stack) {
                parentWindow.stack.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/SplashPage.qml"), { parentWindow: parentWindow })
            }
        }
    }
}
