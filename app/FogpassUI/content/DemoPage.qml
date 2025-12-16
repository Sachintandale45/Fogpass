import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: demoRoot
    // ensure page sizes match the StackView container
    width: parent ? parent.width : 400
    height: parent ? parent.height : 320

    // window passed in from Main.qml or WelcomePage
    property var parentWindow: null
    property string userName: ""

    // opaque background so this page fully covers any previous page
    Rectangle {
        anchors.fill: parent
        color: "#ffffff"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: userName.length > 0 ? "Hello, " + userName + "!" : "Hello!"
            font.pointSize: 18
            color: parentWindow ? parentWindow.textColor : "#000000"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            color: "#f5f5f5"
            radius: 8
            height: 120

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text {
                    text: "Location: Pune"
                    font.pointSize: 14
                }
                Text {
                    text: "Distance: 1200 meter"
                    font.pointSize: 14
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Text { text: "Battery:" }
                    ProgressBar {
                        id: batteryBar
                        value: 0.78
                        from: 0
                        to: 1
                        Layout.preferredWidth: 200
                    }
                    Text {
                        id: batteryText
                        text: Math.round(batteryBar.value*100) + "%"
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                id: backBtn
                text: "Back"
                Layout.minimumWidth: 80
                onClicked: { if (parentWindow) parentWindow.stack.pop() }
            }

            Button {
                id: refreshBtn
                text: "Refresh"
                Layout.minimumWidth: 80
                onClicked: {
                    if (batteryBar.value > 0.7)
                        batteryBar.value = 0.45
                    else
                        batteryBar.value = 0.78
                    batteryText.text = Math.round(batteryBar.value*100) + "%"
                }
            }
        }
    }

    // local progress control referenced above
    ProgressBar {
        id: progress
        visible: false
    }
}
