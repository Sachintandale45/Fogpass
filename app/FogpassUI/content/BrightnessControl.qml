import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// Import our singleton to access global application settings
import trial1

Item {
    id: root
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    // Bright yellow gradient background
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#f7b733" }
            GradientStop { position: 1.0; color: "#fc4a1a" }
        }
    }

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("BrightnessControl: no stack available");
        return s;
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 18
        width: parent ? parent.width * 0.7 : 560

        Text {
            text: "Adjust Brightness"
            font.pointSize: 26
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            // This text updates automatically because the slider's value is bound to the singleton
            text: Math.round(AppSettings.brightnessLevel) + "%"
            font.pointSize: 22
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Slider {
            id: brightnessSlider
            Layout.fillWidth: true
            from: 0
            to: 100
            stepSize: 1
            // This creates a two-way binding. Moving the slider updates the singleton directly.
            value: AppSettings.brightnessLevel
            onValueChanged: {
                AppSettings.brightnessLevel = value;
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
                onClicked: { 
                    var s = stackRef(); 
                    if (s) s.pop(); 
                    else console.warn("BrightnessControl: no stack on back"); 
                }
            }
        }
    }
}