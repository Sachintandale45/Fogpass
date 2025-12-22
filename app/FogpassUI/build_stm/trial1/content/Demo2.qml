import QtQuick 2.15
import QtQuick.Controls 2.15
import trial1 1.0 // For AppSettings

Item {
    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Manual Mode Landmarks"
            font.pixelSize: 28
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView {
            id: landmarkView
            width: 400
            height: 200
            clip: true

            // The model is our global list of landmarks
            model: AppSettings.landmarkLocations

            // The delegate defines how each item in the model is displayed
            delegate: Item {
                width: landmarkView.width
                height: 40

                Text {
                    text: "📍 " + modelData // modelData holds the string for the current item
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    font.pixelSize: 20
                }
            }
        }
    }
}