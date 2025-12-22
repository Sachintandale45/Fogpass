#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <iostream>

#include "CoreState.h"
#include "LandmarkEngine.h"
#include "landmark.h"
#include "CoreDBusAdaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 1. Create the core state and logic engine objects.
    CoreState state;
    LandmarkEngine engine;

    // 2. Create the D-Bus adaptor and connect it to the logic engines.
    new CoreDBusAdaptor(&state, &engine, &app);

    // 3. Register the service on the D-Bus system bus.
    QDBusConnection bus = QDBusConnection::systemBus();
    const QString serviceName = "com.fogpass.Core";

    if (!bus.registerService(serviceName)) {
        std::cerr << "Failed to register D-Bus service: " << bus.lastError().message().toStdString() << std::endl;
        return 1;
    }

    if (!bus.registerObject("/com/fogpass/Core", &state)) {
        std::cerr << "Failed to register D-Bus object: " << bus.lastError().message().toStdString() << std::endl;
        return 1;
    }

    std::cout << "[CORE] fogpass-core started, waiting for D-Bus calls..." << std::endl;

    // 4. Start the landmark simulation logic.
    // This now correctly calls the function with both required arguments.
    setupLandmarkLogic(&state, &engine);

    return app.exec();
}