#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <iostream>

#include "CoreDBusAdaptor.h"
#include "CoreState.h"

// Business logic declaration
#include "landmark.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Connect to system bus
    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        std::cerr << "Failed to connect to system D-Bus\n";
        return 1;
    }

    // Register service
    if (!bus.registerService("com.fogpass.Core")) {
        std::cerr << "Failed to register D-Bus service: "
                  << bus.lastError().message().toStdString()
                  << std::endl;
        return 1;
    }

    // 1. Create core state (single source of truth)
    CoreState state;

    // 2. Create IPC adaptor
    CoreDBusAdaptor adaptor(&state);

    // 3. Wire internal logic
    setupLandmarkLogic(&state);

    // 4. Register D-Bus object
    if (!bus.registerObject(
            "/com/fogpass/Core",
            &adaptor,
            QDBusConnection::ExportAllSlots |
            QDBusConnection::ExportAllSignals)) {

        std::cerr << "Failed to register D-Bus object\n";
        return 1;
    }

    std::cout << "[CORE] fogpass-core started, waiting for D-Bus calls...\n";
    return app.exec();
}
