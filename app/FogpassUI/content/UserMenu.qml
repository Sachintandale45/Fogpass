import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// Import our singleton to access global application settings
import trial1

Item {
    id: userMenuRoot
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    property string pendingAction: ""
    property bool volumeUnlocked: false

    // Beautiful gradient background with mixed colors
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#667eea" }  // Purple-blue
            GradientStop { position: 0.5; color: "#764ba2" }  // Purple
            GradientStop { position: 1.0; color: "#f093fb" }  // Pink
        }
    }

    // Listen for authentication results from the backend
    Connections {
        target: Backend

        function onAccessGranted(capability) {
            if (capability === "ADMIN") {
                passwordPopup.close();
                if (pendingAction === "VOLUME") {
                    goVolume();
                }
                if (pendingAction === "VOLUME_UNLOCK") {
                    volumeUnlocked = true;
                }
                pendingAction = "";
            }
        }

        function onAccessDenied(capability) {
            if (capability === "ADMIN") {
                passwordPopup.errorText = "Access Denied. Please try again.";
                passwordPopup.passwordInput = ""; // Clear password field
            }
        }
    }

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("UserMenu: no stack available");
        return s;
    }


    function go(title) {
        var s = stackRef();
        if (s) {
            s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/ModePlaceholder.qml"), { parentWindow: userMenuRoot.parentWindow, title: title })
        }
    }

    function goToAutoRoutePage() {
        // Safety: Ensure we are in REAL mode when starting from main menu
        Backend.setGnssMode(false);

        // 1. Set Operation Mode to Auto (2)
        Backend.SetOperationMode(2);

        confirmPopup.close();
        var s = stackRef();
        if (s) {
            s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/AutoRoutePage.qml"), { 
                parentWindow: userMenuRoot.parentWindow 
            });
        }
    }

    function requestVolumeUnlock() {
        pendingAction = "VOLUME_UNLOCK";
        passwordPopup.open();
    }

    function goVolume() {
        var s = stackRef();
        if (s) {
            var page = s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/VolumeControl.qml"), {
                parentWindow: userMenuRoot.parentWindow,
                // Pass the current volume level to the new page
                volumeLevel: AppSettings.volumeLevel,
                userMenu: userMenuRoot // Pass reference so VolumeControl can request unlock
            });
            // Connect to the new page's signal
            page.volumeChanged.connect(function(newVolume) {
                AppSettings.volumeLevel = newVolume;
            });
        }
    }

    function goBrightness() {
        var s = stackRef();
        if (s) {
            s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/BrightnessControl.qml"), { parentWindow: userMenuRoot.parentWindow });
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 14

        Text {
            text: "User Menu"
            font.pointSize: 24
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        ColumnLayout {
            spacing: 12
            Layout.fillWidth: true

            Button { text: "Auto Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                // The function goToAutoRoutePage now handles the mode setting
                onClicked: confirmPopup.open()
            }
            Button { text: "Manual Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: {
                    // 1. Set the backend to Manual Mode. This makes it ready to accept a route.
                    Backend.SetOperationMode(1); // 1 = Manual

                    // 2. Get the list of available routes from the backend.
                    var routes = Backend.GetAvailableRoutes();

                    // 3. Navigate to the selection page (pass empty list if none found)
                    var s = stackRef();
                    if (s) {
                        s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/RouteSelectionPage.qml"), {
                            parentWindow: userMenuRoot.parentWindow,
                            routeList: routes || []
                        });
                    }
                }
            }
            Button { text: "Weather Mode"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                // Corrected: Open the weather mode popup
                onClicked: weatherModePopup.open()
            }
            Button { text: "Adjust Volume"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: goVolume()
            }
            Button { text: "Adjust Brightness"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: goBrightness()
            }

            Button { text: "Display All Route"; Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
                background: Rectangle { radius: 8; color: parent.pressed ? "#ffffff" : "#ffffff"; opacity: parent.pressed ? 0.9 : 1.0; border.width: 2; border.color: "#333333" }
                contentItem: Text { text: parent.text; font: parent.font; color: "#333333"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: go("Display All Route")
            }

        }

        Item { Layout.fillHeight: true }

        RowLayout { Layout.alignment: Qt.AlignHCenter; spacing: 12
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
                onClicked: { var s = (parentWindow && parentWindow.stack) || StackView.view; if (s) s.pop(); else console.warn("UserMenu: no stack on back"); }
            }

        }
    }

    Popup {
        id: confirmPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 450
        height: 220
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

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
                text: "Do you want to select auto route?"
                color: "white"
                font.pointSize: 18
                font.bold: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 25

                Button {
                    text: "YES"
                    Layout.minimumWidth: 120
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: "#27ae60"; radius: 8 }
                    onClicked: goToAutoRoutePage()
                }

                Button {
                    text: "NO"
                    Layout.minimumWidth: 120
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: "#c0392b"; radius: 8 }
                    onClicked: confirmPopup.close()
                }
            }
        }
    }

    Popup {
        id: weatherModePopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 450
        height: 220
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

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
                text: "Select Weather Mode"
                color: "white"
                font.pointSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 15

                Button {
                    text: "Foggy Mode"
                    Layout.minimumWidth: 180
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: AppSettings.currentMode === "Foggy" ? "#2980b9" : "#566573"; radius: 8 }
                    onClicked: {
                        Backend.setWeatherMode(true); // Command to UI backend
                        weatherModePopup.close();
                    }
                }

                Button {
                    text: "Non-Foggy Mode"
                    Layout.minimumWidth: 180
                    Layout.minimumHeight: 45
                    font.pointSize: 16
                    background: Rectangle { color: AppSettings.currentMode === "Non-Foggy" ? "#2980b9" : "#566573"; radius: 8 }
                    onClicked: {
                        Backend.setWeatherMode(false); // Command to UI backend
                        weatherModePopup.close();
                    }
                }
            }
        }
    }

    // --- Password Prompt Popup ---
    Popup {
        id: passwordPopup
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 450
        height: 280
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

        property alias passwordInput: passwordField.text
        property alias errorText: errorLabel.text

        // Reset on close
        onClosed: {
            errorText = "";
            passwordInput = "";
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
                text: "Enter Admin Password"
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
                onAccepted: Backend.requestAccess("ADMIN", passwordField.text)
            }

            Text {
                id: errorLabel
                color: "#e74c3c" // Red for errors
                font.pointSize: 14
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 25

                Button { text: "Submit"; Layout.minimumWidth: 120; Layout.minimumHeight: 45; font.pointSize: 16; background: Rectangle { color: "#27ae60"; radius: 8 }
                    onClicked: Backend.requestAccess("ADMIN", passwordField.text)
                }
                Button { text: "Cancel"; Layout.minimumWidth: 120; Layout.minimumHeight: 45; font.pointSize: 16; background: Rectangle { color: "#c0392b"; radius: 8 }
                    onClicked: passwordPopup.close()
                }
            }
        }
    }
}
