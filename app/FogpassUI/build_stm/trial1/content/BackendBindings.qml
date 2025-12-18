import QtQuick 2.15
import trial1 1.0

Item {
    // This component's only job is to listen for signals from the C++ BackendManager
    // and update the global AppSettings. It has no visual content.
    visible: false

    // The 'BackendManager' object must be registered with the QML engine from C++
    // for these connections to work.
    Connections {
        target: Backend // This must match the context property name from main.cpp

        // This function name must match the C++ signal: onBatteryLevelUpdated(int newLevel)
        function onBatteryLevelUpdated(newLevel) {
            console.log("BackendBindings: Received new battery level from C++:", newLevel);
            AppSettings.batteryLevel = newLevel;
        }

        // This function name must match the C++ signal: onVolumeChanged(int newVolume)
        function onVolumeChanged(newVolume) {
            console.log("BackendBindings: Received new volume from C++:", newVolume);
            AppSettings.volumeLevel = newVolume;
        }

        // This function name must match the C++ signal: onModeUpdated(string mode)
        function onModeUpdated(newMode) {
            console.log("BackendBindings: Received new weather mode from C++:", newMode);
            AppSettings.currentMode = newMode;
        }

    }
}
