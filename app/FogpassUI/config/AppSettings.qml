import QtQuick 2.15

// By declaring this as a Singleton, QML ensures only one instance of this
// component ever exists. It can be imported and used from anywhere in the app.
pragma Singleton

Item {
    // Global application settings that persist across all pages.

    // --- State Properties ---
    property int volumeLevel: 75
    property int brightnessLevel: 80
    property int batteryLevel: 87 // Default value, will be updated by the backend.
    property string currentMode: "Non-Foggy" // Can be "Foggy" or "Non-Foggy"

    // --- Configuration Constants ---
    readonly property int volumeThreshold: 60
}
