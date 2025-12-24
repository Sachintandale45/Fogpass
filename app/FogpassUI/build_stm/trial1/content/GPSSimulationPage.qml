import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var parentWindow
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600

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
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Header
        Text {
            text: "GPS Simulation"
            font.pointSize: 28
            font.bold: true
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        // GPS Path Container
        Item {
            id: pathContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 400

            // Background for path area
            Rectangle {
                anchors.fill: parent
                color: "#000000"
                opacity: 0.2
                radius: 12
                border.width: 2
                border.color: "#ffffff"
            }

            // Animated GPS Path Line
            Canvas {
                id: pathCanvas
                anchors.fill: parent
                anchors.margins: 40
                
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    
                    ctx.strokeStyle = "#4fc3f7"
                    ctx.lineWidth = 4
                    ctx.lineCap = "round"
                    ctx.lineJoin = "round"
                    
                    ctx.beginPath()
                    ctx.moveTo(0, height * 0.5)
                    
                    // Create a curved path from left to right
                    ctx.quadraticCurveTo(width * 0.1, height * 0.2, width * 0.2, height * 0.3)
                    ctx.quadraticCurveTo(width * 0.3, height * 0.5, width * 0.4, height * 0.6)
                    ctx.quadraticCurveTo(width * 0.5, height * 0.35, width * 0.6, height * 0.4)
                    ctx.quadraticCurveTo(width * 0.7, height * 0.65, width * 0.8, height * 0.7)
                    ctx.lineTo(width, height * 0.5)
                    
                    ctx.stroke()
                }
                
                Component.onCompleted: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
            }

            // Animated dot moving along the path
            Rectangle {
                id: movingDot
                width: 16
                height: 16
                radius: 8
                color: "#ffeb3b"
                border.width: 2
                border.color: "#ffffff"
                
                x: pathContainer.width * 0.1 + (pathContainer.width * 0.8 * pathProgress) - 8
                y: calculateY(pathProgress) - 8

                property real pathProgress: 0.0

                SequentialAnimation on pathProgress {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: 0.0
                        to: 1.0
                        duration: 5000
                        easing.type: Easing.InOutQuad
                    }
                }

                // Glow effect
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width + 8
                    height: parent.height + 8
                    radius: width / 2
                    color: "#ffeb3b"
                    opacity: 0.3
                    SequentialAnimation on opacity {
                        running: true
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: 0.3
                            to: 0.6
                            duration: 1000
                        }
                        NumberAnimation {
                            from: 0.6
                            to: 0.3
                            duration: 1000
                        }
                    }
                }
            }

            // Numbered Points (1-7) along the path
            Repeater {
                model: 7
                
                Item {
                    property real progress: (index + 1) / 8.0  // Distribute points evenly
                    property real pointX: pathContainer.width * 0.1 + (pathContainer.width * 0.8 * progress)
                    property real pointY: calculateY(progress)
                    
                    x: pointX - 25
                    y: pointY - 25
                    width: 50
                    height: 50

                    // Outer circle with pulse animation
                    Rectangle {
                        anchors.centerIn: parent
                        width: 50
                        height: 50
                        radius: 25
                        color: "#4fc3f7"
                        opacity: 0.3
                        border.width: 2
                        border.color: "#ffffff"
                        
                        SequentialAnimation on scale {
                            running: true
                            loops: Animation.Infinite
                            NumberAnimation {
                                from: 1.0
                                to: 1.3
                                duration: 1500
                                easing.type: Easing.InOutQuad
                            }
                            NumberAnimation {
                                from: 1.3
                                to: 1.0
                                duration: 1500
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }

                    // Inner circle
                    Rectangle {
                        anchors.centerIn: parent
                        width: 40
                        height: 40
                        radius: 20
                        color: "#4fc3f7"
                        border.width: 2
                        border.color: "#ffffff"
                    }

                    // Number label
                    Text {
                        anchors.centerIn: parent
                        text: (index + 1).toString()
                        font.pointSize: 18
                        font.bold: true
                        color: "#ffffff"
                    }
                }
            }
        }

        // Back Button
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumWidth: 200
            Layout.minimumHeight: 50
            text: "Back"
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
            
            onClicked: {
                if (parentWindow && parentWindow.stack) {
                    parentWindow.stack.pop()
                }
            }
        }
    }

    // Function to calculate Y position along the curved path
    function calculateY(progress) {
        var containerHeight = pathContainer.height
        
        // Match the Canvas path curve calculations
        if (progress < 0.125) {
            // First curve: from 0.5 to 0.3
            var t = progress / 0.125
            return containerHeight * (0.5 - 0.2 * t * t)
        } else if (progress < 0.375) {
            // Second curve: from 0.3 to 0.6
            var t = (progress - 0.125) / 0.25
            return containerHeight * (0.3 + 0.3 * t * t)
        } else if (progress < 0.625) {
            // Third curve: from 0.6 to 0.4
            var t = (progress - 0.375) / 0.25
            return containerHeight * (0.6 - 0.2 * t)
        } else if (progress < 0.875) {
            // Fourth curve: from 0.4 to 0.7
            var t = (progress - 0.625) / 0.25
            return containerHeight * (0.4 + 0.3 * t)
        } else {
            // Final line: from 0.7 to 0.5
            var t = (progress - 0.875) / 0.125
            return containerHeight * (0.7 - 0.2 * t)
        }
    }
}

