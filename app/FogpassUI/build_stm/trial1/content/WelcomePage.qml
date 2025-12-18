import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    // ensure page sizes match the StackView container
    width: parent ? parent.width : 400
    height: parent ? parent.height : 320

    // window passed in from Main.qml when pushed/loaded
    property var parentWindow: null

    // opaque background so this page fully covers any previous page
    Rectangle {
        anchors.fill: parent
        color: "#ffffff"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            id: welcomeText
            text: "Welcome — enter your name and press Enter"
            font.pointSize: 18
            color: parentWindow ? parentWindow.textColor : "#000000"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
        }

        TextField {
            id: nameField
            placeholderText: "Your name"
            Layout.fillWidth: true
            onAccepted: {
                // push DemoPage and pass the entered name and the parent window
                if (parentWindow) parentWindow.stack.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/DemoPage.qml"), { userName: text, parentWindow: parentWindow })
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                id: enterBtn
                text: "Enter"
                Layout.minimumWidth: 100
                onClicked: { if (parentWindow) parentWindow.stack.push(Qt.resolvedUrl("qrc:/qt/qml/trial1/content/DemoPage.qml"), { userName: nameField.text, parentWindow: parentWindow }) }
            }

            Button {
                id: quitBtn
                text: "Quit"
                Layout.alignment: Qt.AlignRight
                Layout.minimumWidth: 80
                onClicked: Qt.quit()
            }
        }
    }
}
