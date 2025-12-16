
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "content"

ApplicationWindow {
    id: appWindow
    flags: Qt.Window | Qt.CustomizeWindowHint | Qt.WindowTitleHint
    width: 1024
    height: 720
    visible: true
    title: qsTr("Basic UI")

    // Simple color theme properties
    property color bgColor: "#1e1e2e"
    property color panelColor: "#252535"
    property color primaryColor: "#4fc3f7"
    property color textColor: "#ffffff"
    property color mutedTextColor: "#cfcfe0"
    property int volumeLevel: 75
    // apply window background
    color: bgColor

    // expose stack to pages
    property alias stack: stackView

    StackView {
        id: stackView
        anchors.fill: parent
        // Some QML parsers may reject an object-literal for `initialItem`.
        // Instead, push the initial page with properties once the component is ready.
        Component.onCompleted: {
            // start at the splash page
            stackView.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/IntroPage.qml"), { parentWindow: appWindow })
        }
    }

    HeaderBar {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 16
        z: 2
        batteryLevel: typeof backend !== 'undefined' ? backend.batteryLevel : 0
        volumeLevel: appWindow.volumeLevel
    }
}
