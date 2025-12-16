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
        anchors.centerIn: parent
        spacing: 24
        width: parent ? parent.width * 0.7 : 560

        // Title
        Text {
            text: "Test Menu"
            font.pointSize: 28
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 20
        }

        // Option A
        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 80
            radius: 12
            color: "#ffffff"
            opacity: optionA.pressed ? 0.8 : 1.0
            
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ffecd2" }  // Light orange
                GradientStop { position: 1.0; color: "#fcb69f" }  // Peach
            }
            
            border.width: 2
            border.color: "#ffffff"
            
            Text {
                anchors.centerIn: parent
                text: "Option A"
                font.pointSize: 22
                font.bold: true
                color: "#333333"
            }
            
            MouseArea {
                id: optionA
                anchors.fill: parent
                onClicked: {
                    // Add your Option A action here
                    console.log("Option A clicked")
                }
            }
        }

        // Option B
        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 80
            radius: 12
            color: "#ffffff"
            opacity: optionB.pressed ? 0.8 : 1.0
            
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#a8edea" }  // Light cyan
                GradientStop { position: 1.0; color: "#fed6e3" }  // Light pink
            }
            
            border.width: 2
            border.color: "#ffffff"
            
            Text {
                anchors.centerIn: parent
                text: "Option B"
                font.pointSize: 22
                font.bold: true
                color: "#333333"
            }
            
            MouseArea {
                id: optionB
                anchors.fill: parent
                onClicked: {
                    // Add your Option B action here
                    console.log("Option B clicked")
                }
            }
        }

        // Option C
        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 80
            radius: 12
            color: "#ffffff"
            opacity: optionC.pressed ? 0.8 : 1.0
            
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#d299c2" }  // Light purple
                GradientStop { position: 1.0; color: "#fef9d7" }  // Light yellow
            }
            
            border.width: 2
            border.color: "#ffffff"
            
            Text {
                anchors.centerIn: parent
                text: "Option C"
                font.pointSize: 22
                font.bold: true
                color: "#333333"
            }
            
            MouseArea {
                id: optionC
                anchors.fill: parent
                onClicked: {
                    // Add your Option C action here
                    console.log("Option C clicked")
                }
            }
        }

        // Spacer
        Item { Layout.fillHeight: true }

        // Back Button
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumWidth: 200
            Layout.minimumHeight: 50
            text: "Back"
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
            
            onClicked: {
                if (parentWindow && parentWindow.stack) {
                    parentWindow.stack.pop()
                }
            }
        }
    }
}

