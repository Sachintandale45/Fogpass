import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import trial1 // For Backend and AppSettings

Page {
    id: routeSelectionPage
    property var parentWindow
    property var routeList: [] // Expects a string array from UserMenu.qml

    background: Rectangle {
        color: AppSettings.bgColor || "#1e1e2e"
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
                onClicked: (parentWindow && parentWindow.stack) ? parentWindow.stack.pop() : console.warn("No stack to pop from.")
            }
            Label {
                text: "Select Route"
                font.pointSize: 20
                font.bold: true
                color: AppSettings.textColor || "white"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                padding: 10
            }
        }
    }

    Label {
        anchors.centerIn: parent
        text: "No routes found.\nPlease check /data/routes/ on device."
        color: "#aaaaaa"
        font.pointSize: 16
        visible: routeSelectionPage.routeList.length === 0
        horizontalAlignment: Text.AlignHCenter
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.topMargin: 60 // Space for header
        anchors.bottomMargin: 60 // Space for footer
        model: routeSelectionPage.routeList
        spacing: 10
        clip: true

        delegate: Button {
            width: parent.width * 0.8
            height: 50
            anchors.horizontalCenter: parent.horizontalCenter
            text: modelData // The route filename
            font.pointSize: 16

            background: Rectangle {
                radius: 8
                color: parent.pressed ? "#4fc3f7" : "#252535"
                border.color: "#4fc3f7"
                border.width: 1
            }

            onClicked: {
                // 1. Tell the backend which route was selected.
                // The backend will load it and start processing automatically.
                Backend.SelectRoute(modelData);

                // 2. Navigate to the main driving display page.
                var s = (parentWindow && parentWindow.stack) || StackView.view;
                if (s) {
                    // Replace the current page with the driving screen so 'back' works correctly
                    s.replace(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/Demo2.qml"), { parentWindow: routeSelectionPage.parentWindow });
                }
            }
        }
    }

    footer: Button {
        text: "Back"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onClicked: (parentWindow && parentWindow.stack) ? parentWindow.stack.pop() : console.warn("No stack to pop from.")
    }
}
