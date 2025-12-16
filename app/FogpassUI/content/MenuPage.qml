import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

    function stackRef() {
        var s = (parentWindow && parentWindow.stack) || StackView.view;
        if (!s) console.warn("MenuPage: no stack available for navigation");
        return s;
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
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/TestMenuPage.qml"), { parentWindow: root.parentWindow });
            }
        }
        Button { 
            text: "USB Menu"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
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
                if (s) s.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/PlaceholderPage.qml"), { title: "USB Menu", parentWindow: root.parentWindow });
            }
        }
        Button { 
            text: "GPS Simulation"; 
            Layout.minimumWidth: 240; 
            Layout.minimumHeight: 50
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
}
