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

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("MenuPage: no stack available for navigation");
        return s;
    }

    function goTestMenu() {
        if (!root.isAuthenticated) {
            passwordPopup.open();
            passwordPopup.targetPage = "TestMenu";
        } else {
            var s = root.stackRef();
            if (s) {
                s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/TestMenuPage.qml"), { parentWindow: root.parentWindow });
                root.isAuthenticated = false; // Reset auth after navigation
            }
        }
    }

    function goUsbMenu() {
        if (!root.isAuthenticated) {
            passwordPopup.open();
            passwordPopup.targetPage = "UsbMenu";
        } else {
            var s = root.stackRef();
            if (s) {
                s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/PlaceholderPage.qml"), { title: "USB Menu", parentWindow: root.parentWindow });
                root.isAuthenticated = false; // Reset auth after navigation
            }
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
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/GPSSimulationPage.qml"), { parentWindow: root.parentWindow });
            }
        }

    }

    Popup {
        id: passwordPopup
        property string targetPage: ""
        x: (root.width - width) / 2
        y: (root.height - height) / 2
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
                    if (text === AppSettings.volumePassword) {
                        root.isAuthenticated = true;
                        passwordPopup.close();
                        if (passwordPopup.targetPage === "TestMenu") {
                            goTestMenu();
                        } else if (passwordPopup.targetPage === "UsbMenu") {
                            goUsbMenu();
                        }
                    } else { text = ""; }
                }
            }
        }
    }
}
