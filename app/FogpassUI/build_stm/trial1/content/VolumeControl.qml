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
            GradientStop { position: 0.0; color: "#667eea" }
            GradientStop { position: 0.5; color: "#764ba2" }
            GradientStop { position: 1.0; color: "#f093fb" }
        }
    }

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("VolumeControl: no stack available");
        return s;
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 18
        width: parent ? parent.width * 0.7 : 560

        Text {
            text: "Adjust Volume"
            font.pointSize: 26
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            id: valueText
            text: (parentWindow ? parentWindow.volumeLevel : 0) + "%"
            font.pointSize: 22
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Slider {
            id: volumeSlider
            Layout.fillWidth: true
            from: 0
            to: 100
            value: parentWindow ? parentWindow.volumeLevel : 75
            stepSize: 1
            onValueChanged: {
                if (parentWindow) {
                    parentWindow.volumeLevel = value
                    valueText.text = Math.round(value) + "%"
                }
            }
        }

        RowLayout {
            spacing: 12
            Layout.alignment: Qt.AlignHCenter
            Button {
                text: "Back"
                Layout.minimumWidth: 160
                Layout.minimumHeight: 46
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
            onClicked: { var s = stackRef(); if (s) s.pop(); else console.warn("VolumeControl: no stack on back"); }
            }
        }
    }
}

