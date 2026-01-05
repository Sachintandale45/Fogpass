import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import trial1

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

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        width: 400

        Text {
            text: "Change Admin Password"
            font.pointSize: 24
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: oldPassField
            placeholderText: "Old Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            font.pointSize: 16
            color: "white"
            background: Rectangle { color: "#2c3e50"; radius: 4; border.color: "white"; border.width: 1 }
        }

        TextField {
            id: newPassField
            placeholderText: "New Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            font.pointSize: 16
            color: "white"
            background: Rectangle { color: "#2c3e50"; radius: 4; border.color: "white"; border.width: 1 }
        }

        TextField {
            id: confirmPassField
            placeholderText: "Re-enter New Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            font.pointSize: 16
            color: "white"
            background: Rectangle { color: "#2c3e50"; radius: 4; border.color: "white"; border.width: 1 }
        }

        Text {
            id: statusText
            text: ""
            color: "yellow"
            font.pointSize: 14
            visible: text !== ""
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 20

            Button {
                text: "Update"
                Layout.minimumWidth: 120
                font.pointSize: 16
                background: Rectangle { color: "#27ae60"; radius: 8 }
                onClicked: {
                    if (newPassField.text !== confirmPassField.text) {
                        statusText.text = "New passwords do not match!";
                        statusText.color = "red";
                        return;
                    }
                    if (oldPassField.text === "") {
                        statusText.text = "Enter old password";
                        statusText.color = "red";
                        return;
                    }
                    
                    // Call backend to change password
                    var success = Backend.changePassword("ADMIN", oldPassField.text, newPassField.text);
                    if (success) {
                        statusText.text = "Password changed successfully!";
                        statusText.color = "#00ff00";
                        oldPassField.text = "";
                        newPassField.text = "";
                        confirmPassField.text = "";
                    } else {
                        statusText.text = "Update Failed. Check password or system permissions.";
                        statusText.color = "red";
                    }
                }
            }

            Button {
                text: "Back"
                Layout.minimumWidth: 120
                font.pointSize: 16
                background: Rectangle { color: "#c0392b"; radius: 8 }
                onClicked: {
                    var s = (parentWindow && parentWindow.stack) || StackView.view;
                    if (s) s.pop();
                }
            }
        }
    }
}