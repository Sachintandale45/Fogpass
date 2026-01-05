import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import trial1

Item {
    id: root
    property bool isAuthenticated: false
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    property string pendingTargetPage: ""

    Connections {
        target: Backend
        function onAccessGranted(capability) {
            if (capability === "ADMIN") {
                root.isAuthenticated = true;
                passwordPopup.close();
                
                var s = root.stackRef();
                if (s) {
                    if (root.pendingTargetPage === "TestMenu") {
                        s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/TestMenuPage.qml"), { parentWindow: root.parentWindow });
                    } else if (root.pendingTargetPage === "UsbMenu") {
                        s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/PlaceholderPage.qml"), { title: "USB Menu", parentWindow: root.parentWindow });
                    }
                }
                root.isAuthenticated = false; // Reset auth after navigation
                root.pendingTargetPage = "";
            }
        }
        function onAccessDenied(capability) {
            if (capability === "ADMIN") {
                passwordPopup.errorText = "Access Denied";
                passwordPopup.passwordInput = "";
            }
        }
    }

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("MenuPage: no stack available for navigation");
        return s;
    }

    function goTestMenu() {
        if (!root.isAuthenticated) {
            passwordPopup.open();
            root.pendingTargetPage = "TestMenu";
        }
    }

    function goUsbMenu() {
        if (!root.isAuthenticated) {
            passwordPopup.open();
            root.pendingTargetPage = "UsbMenu";
        }
    }

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
        spacing: 12
        width: parent ? parent.width * 0.6 : 480

        Text { 
            text: "Main Menu"; 
            font.pointSize: 20; 
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter; 
            Layout.alignment: Qt.AlignHCenter 
        }

        Button { 
            text: "User Menu"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
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
                var s = root.stackRef();
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/UserMenu.qml"), { parentWindow: root.parentWindow });
            }
        }
        Button { 
            text: "Test Menu"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
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
                goTestMenu();
            }
        }
        Button { 
            text: "USB Menu"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
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
                goUsbMenu();
            }
        }
        Button { 
            text: "GPS Simulation"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
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
                var s = root.stackRef();
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/GPSSimulationPage2.qml"), { parentWindow: root.parentWindow });
            }
        }

        Button { 
            text: "Change Password"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 14
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
                var s = root.stackRef();
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/ChangePasswordPage.qml"), { parentWindow: root.parentWindow });
            }
        }

    }

    Popup {
        id: passwordPopup
        x: (root.width - width) / 2
        y: (root.height - height) / 2
        width: 450
        height: 280
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        property alias passwordInput: passwordField.text
        property alias errorText: errorLabel.text
        onClosed: {
            passwordField.text = "";
            errorText = "";
            // pendingTargetPage is managed in onAccessGranted
        }

        background: Rectangle {
            color: "#34495e"
            border.color: "#4fc3f7"
            border.width: 2
            radius: 12
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Text {
                text: "Enter Password"
                color: "white"
                font.pointSize: 18
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                placeholderText: "Password"
                echoMode: TextInput.Password
                color: "white"
                font.pointSize: 16
                background: Rectangle {
                    color: "#2c3e50"
                    border.color: "white"
                    border.width: 1
                    radius: 4
                }
                onAccepted: Backend.requestAccess("ADMIN", text)
            }

            Text {
                id: errorLabel
                color: "#e74c3c"
                font.pointSize: 14
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 25

                Button { 
                    text: "Submit"
                    Layout.minimumWidth: 120
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: "#27ae60"; radius: 8 }
                    onClicked: Backend.requestAccess("ADMIN", passwordField.text)
                }
                Button { 
                    text: "Cancel"
                    Layout.minimumWidth: 120
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: "#c0392b"; radius: 8 }
                    onClicked: passwordPopup.close()
                }
            }
        }
    }
}
