import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import trial1

Page {
    id: gpsSimPage
    property var parentWindow

    background: Rectangle {
        color: AppSettings.bgColor || "#1e1e2e"
    }

    // When this page is loaded, automatically switch the backend to Simulation Mode.
    Component.onCompleted: {
        console.log("Entering Simulation Page: Activating Simulation Mode")
        Backend.setGnssMode(true) // true = Simulation
    }

    header: Pane {
        Layout.fillWidth: true
        background: Rectangle { color: "transparent" }
        RowLayout {
            anchors.fill: parent
            Button {
                text: "Back"
                Layout.preferredHeight: 40
                Layout.preferredWidth: 80
                font.pointSize: 14
                background: Rectangle {
                    color: parent.pressed ? "#666666" : "transparent"
                    border.color: AppSettings.textColor || "white"
                    border.width: 1
                    radius: 4
                }
                contentItem: Text { text: parent.text; font: parent.font; color: AppSettings.textColor || "white"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: {
                    // When leaving, always switch the backend back to Real Mode for safety.
                    console.log("Leaving Simulation Page: Reverting to Real Mode")
                    Backend.setGnssMode(false) // false = Real
                    var s = (parentWindow && parentWindow.stack) || StackView.view
                    if (s) s.pop()
                }
            }
            Label {
                text: "GPS Simulation"
                font.pointSize: 20
                font.bold: true
                color: AppSettings.textColor || "white"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 14

        // These buttons now operate within the simulation context
        Button {
            text: "Auto Mode"
            Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
            onClicked: {
                Backend.SetOperationMode(2); // Auto
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (s) {
                    s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/AutoRoutePage.qml"), {
                        parentWindow: gpsSimPage.parentWindow
                    });
                }
            }
        }
        Button {
            text: "Manual Mode"
            Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
            onClicked: {
                Backend.SetOperationMode(1); // Manual
                var routes = Backend.GetAvailableRoutes();
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (s) {
                    s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/RouteSelectionPage.qml"), {
                        parentWindow: gpsSimPage.parentWindow,
                        routeList: routes || []
                    });
                }
            }
        }
        Button {
            text: "Configure Modules"
            Layout.minimumWidth: 260; Layout.minimumHeight: 46; font.pointSize: 18; font.bold: true; Layout.alignment: Qt.AlignHCenter
            onClicked: {
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (s) {
                    s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/ModePlaceholder.qml"), { parentWindow: gpsSimPage.parentWindow, title: "Configure Modules" })
                }
            }
        }
    }
}