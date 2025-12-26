#include <QCoreApplication>
#include <QDebug>
#include <QDBusConnection>

// Core modules
#include "CoreState.h"
#include "CoreDbusAdaptor.h"

// GNSS & navigation
#include "GnssReader.h"
#include "SimulatedGnssReader.h"
#include "GnssManager.h"
#include "locator.h"

// Logic
#include "LandmarkEngine.h"
#include "alertmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInfo() << "FogPASS Core Service starting...";

    // Core shared state
    CoreState *coreState = new CoreState(&app);

    // Real GNSS (UART)
    GnssReader *realGnss = new GnssReader(&app);
    realGnss->configure("/dev/ttyS6", 115200);

    // Simulated GNSS
    SimulatedGnssReader *simGnss = new SimulatedGnssReader(&app);
    //simGnss->loadCsvFile("/data/routes/ROUTE1.csv");

    // GNSS manager (runtime switch)
    GnssManager *gnssManager =
        new GnssManager(realGnss, simGnss, &app);
    gnssManager->start();

    // Locator (position abstraction)
    Locator *locator = new Locator(gnssManager, &app);

    // Landmark engine (route logic)
    LandmarkEngine *landmarkEngine =
        new LandmarkEngine(locator, coreState, &app);
    // REMOVED: landmarkEngine->start(); 
    // The engine now starts in IDLE mode and waits for UI commands via D-Bus.

    // Sync: When a route is selected in LandmarkEngine, load it into Simulation too
    QObject::connect(landmarkEngine, &LandmarkEngine::routeFileLoaded,
                     simGnss, &SimulatedGnssReader::loadCsvFile);

    // Alert manager (decision layer)
    AlertManager *alertManager =
        new AlertManager(coreState, &app);

    // GNSS stability → AlertManager
    QObject::connect(gnssManager, &IGnssSource::gnssStabilityChanged,
                     alertManager, &AlertManager::onGnssStabilityChanged,
                     Qt::QueuedConnection);

    // Landmark updates → AlertManager
    QObject::connect(coreState, &CoreState::nextLandmarksUpdated,
                     alertManager, &AlertManager::onNextLandmarksUpdated,
                     Qt::QueuedConnection);

    // D-Bus adaptor
    CoreDbusAdaptor *dbusAdaptor =
        new CoreDbusAdaptor(coreState, landmarkEngine, &app);

    // GNSS mode change from UI
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::gnssModeChangeRequested,
                     gnssManager, [gnssManager](int mode) {
        GnssManager::Mode m =
            (mode == 1) ? GnssManager::Mode::Simulation
                        : GnssManager::Mode::Real;
        qInfo() << "GNSS mode changed via D-Bus:" << mode;
        gnssManager->setMode(m);
    });

    // Weather / fog mode
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::weatherModeChangeRequested,
                     coreState, &CoreState::setFogMode);

    // Register D-Bus service
    QDBusConnection bus = QDBusConnection::systemBus();

    if (!bus.registerService("com.fogpass.Core")) {
        qCritical() << "Failed to register D-Bus service:"
                    << bus.lastError().message();
    }

    if (!bus.registerObject("/com/fogpass/Core",
                            dbusAdaptor,
                            QDBusConnection::ExportAllSlots |
                            QDBusConnection::ExportAllSignals)) {
        qCritical() << "Failed to register D-Bus object:"
                    << bus.lastError().message();
    }

    qInfo() << "FogPASS Core initialized. Entering event loop.";
    return app.exec();
}
