#include <QCoreApplication>
#include <QDebug>

// Core modules
#include "CoreState.h"
#include "CoreDBusAdaptor.h"

// Logic modules
#include "GnssReader.h"
#include "locator.h"
#include "LandmarkEngine.h"
#include "alertmanager.h"

int main(int argc, char *argv[])
{
    // 1️⃣ Create Qt core application (event loop owner)
    QCoreApplication app(argc, argv);
    qInfo() << "FogPass Core Service starting...";

    // 2️⃣ Create CoreState (single source of truth)
    CoreState *coreState = new CoreState(&app);

    // 3️⃣ Create GNSS reader (low-level, POSIX-based)
    GnssReader *gnssReader = new GnssReader(&app);

    // Start GNSS (example UART — adjust as per DTS)
    if (!gnssReader->start("/dev/ttyS6", 115200)) {
        qCritical() << "Failed to start GNSS reader";
    }

    // 4️⃣ Create Locator (position abstraction)
    Locator *locator = new Locator(gnssReader, &app);

    // 5️⃣ Create Landmark engine (periodic navigation logic)
    LandmarkEngine *landmarkEngine =
        new LandmarkEngine(locator, coreState, &app);

    // Load route / landmark data
    landmarkEngine->loadRouteFile("/data/routes/route1.csv");

    // Start periodic landmark processing
    landmarkEngine->start();

    // 6️⃣ Create AlertManager (decision logic)
    AlertManager *alertManager = new AlertManager(coreState, &app);

    // ---- SIGNAL WIRING (composition root) ----

    // GNSS → AlertManager (GNSS stability alerts)
    QObject::connect(
        gnssReader, &GnssReader::gnssStabilityChanged,
        alertManager, &AlertManager::onGnssStabilityChanged,
        Qt::QueuedConnection
    );

    // Landmark updates → AlertManager (distance threshold alerts)
    QObject::connect(
        coreState, &CoreState::nextLandmarksUpdated,
        alertManager, &AlertManager::onNextLandmarksUpdated,
        Qt::QueuedConnection
    );

    // 7️⃣ Create D-Bus adaptor (Core → UI IPC)
    CoreDbusAdaptor *dbusAdaptor =
        new CoreDbusAdaptor(coreState);

    Q_UNUSED(dbusAdaptor);

    qInfo() << "FogPass Core Service initialized. Entering event loop.";

    // 8️⃣ Start Qt event loop (MANDATORY)
    return app.exec();
}
