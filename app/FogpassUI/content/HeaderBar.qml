import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: headerBar
    property int batteryLevel: 0
    property int volumeLevel: 75

    width: batteryRow.implicitWidth + volumeRow.implicitWidth + 32
    height: 40

    Row {
        id: mainRow
        anchors.fill: parent
        spacing: 14

        Row {
            id: batteryRow
            spacing: 8
            // Battery icon
            Item {
                width: 68
                height: 32

                Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: "#1b1b2b"
                    opacity: 0.65
                    border.width: 2
                    border.color: "#ffffff"
                }

                // Battery cap
                Rectangle {
                    width: 6
                    height: 12
                    radius: 2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: -6
                    color: "#ffffff"
                    opacity: 0.9
                }

                // Battery fill
                Rectangle {
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                        leftMargin: 3
                        rightMargin: 10
                    }
                    width: Math.max(6, (batteryLevel / 100) * (parent.width - 13))
                    radius: 4
                    color: batteryLevel >= 80 ? "#2ecc71" : (batteryLevel >= 50 ? "#f1c40f" : "#e74c3c")
                    opacity: 0.95
                }
            }

            Text {
                text: batteryLevel + "%"
                color: "#ffffff"
                font.pointSize: 14
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Row {
            id: volumeRow
            spacing: 8

            // Speaker icon with curves
            Item {
                id: volumeIcon
                width: 56
                height: 32

                Canvas {
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        // speaker body
                        ctx.fillStyle = "#ffffff"
                        ctx.beginPath()
                        ctx.moveTo(4, height*0.30)
                        ctx.lineTo(16, height*0.30)
                        ctx.lineTo(28, 6)
                        ctx.lineTo(28, height-6)
                        ctx.lineTo(16, height*0.70)
                        ctx.lineTo(4, height*0.70)
                        ctx.closePath()
                        ctx.fill()

                        // bars based on volume
                        var v = Math.max(0, Math.min(100, volumeLevel)) / 100
                        var bars = 3
                        for (var i = 1; i <= bars; i++) {
                            var strength = v - (i - 1) * 0.25
                            if (strength > 0) {
                                var alpha = Math.min(1, strength * 1.6)
                                ctx.strokeStyle = "rgba(79,195,247," + alpha + ")"
                                ctx.lineWidth = 2
                                ctx.beginPath()
                                var offset = 10 + i * 7
                                ctx.moveTo(28 + offset, height * 0.25 - i * 2)
                                ctx.quadraticCurveTo(28 + offset + 6, height * 0.5, 28 + offset, height * 0.75 + i * 2)
                                ctx.stroke()
                            }
                        }
                    }
                    Component.onCompleted: requestPaint()
                    onWidthChanged: requestPaint()
                    onHeightChanged: requestPaint()
                }
            }

            Text {
                text: volumeLevel + "%"
                color: "#ffffff"
                font.pointSize: 14
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}

