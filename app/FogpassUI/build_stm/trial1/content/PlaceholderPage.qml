import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    property string title: "Placeholder"
    property var parentWindow: null
    property int volumeLevel: 75

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
        anchors.centerIn: parent
        spacing: 16
        width: parent ? parent.width * 0.8 : 640

        Text { 
            text: title; 
            font.pointSize: 22; 
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter; 
            Layout.alignment: Qt.AlignHCenter 
        }

        Text { 
            text: "This is a placeholder page for " + title; 
            color: "#ffffff"
            wrapMode: Text.WordWrap 
        }

        RowLayout { 
            Layout.alignment: Qt.AlignHCenter; 
            spacing: 12
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
                onClicked: if (parentWindow) parentWindow.stack.pop() 
            }
        }
    }
}
