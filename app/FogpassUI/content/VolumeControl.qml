import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// Import our singleton to access global application settings
import trial1

Item {
    // Define a signal that will be emitted when the volume is finalized
    signal volumeChanged(int newVolume)

    id: root
    property var parentWindow
    // This property receives the volume level from the previous page
    property bool isAuthenticated: false
    property int previousVolume: volumeLevel
    property int volumeLevel: 75 // This property receives the value from the previous page
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    Connections {
        target: Backend
        function onAccessGranted(capability) {
            if (capability === "ADMIN") {
                root.isAuthenticated = true;
                passwordPopup.close();
            }
        }
        function onAccessDenied(capability) {
            if (capability === "ADMIN") {
                passwordField.text = "";
                passwordField.placeholderText = "Try Again";
            }
        }
    }

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
            text: Math.round(volumeSlider.value) + "%"
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
            value: root.volumeLevel // Set slider's initial value
            stepSize: 1
            onValueChanged: {
                if (value < 60 && !root.isAuthenticated) {
                    // If moving below threshold without auth, open dialog
                    passwordPopup.open();
                    // Prevent slider from staying in the restricted area
                    volumeSlider.value = root.previousVolume;
                } else {
                    // Otherwise, allow the change
                    root.volumeLevel = value;
                    root.previousVolume = value;
                }
            }
            onPressedChanged: {
                // Store the volume when the user starts dragging
                if (pressed) root.previousVolume = volumeSlider.value;
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
                    if (s) {
                        // Emit the signal with the new volume before popping the page
                        root.volumeChanged(root.volumeLevel);
                        s.pop(); 
                    } else console.warn("VolumeControl: no stack on back"); 
                }
            }
        }
    }

    Popup {
        id: passwordPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 300
        height: 200
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: "#444"
            border.color: "#FFF"
            border.width: 1
            radius: 8
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Text {
                text: "Enter Password"
                color: "white"
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: "Password"
                echoMode: TextInput.Password
                color: "white"
                onAccepted: { 
                    Backend.requestAccess("ADMIN", text);
                }
            }
        }
    }
}
