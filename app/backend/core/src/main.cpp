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
#include "Auth/SecurityManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInfo() << "FogPASS Core Service starting...";

    // Core shared state
    CoreState *coreState = new CoreState(&app);

    // Security Manager (Access Control)
    SecurityManager *securityManager = new SecurityManager(&app);

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

    // When a route is selected, notify the simulation reader so it loads the same file.
    QObject::connect(landmarkEngine, &LandmarkEngine::routeSelected,
                     simGnss, &SimulatedGnssReader::onRouteSelected);

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

    // Speed updates: GnssManager -> CoreState
    QObject::connect(gnssManager, &IGnssSource::positionUpdated,
                     coreState, [coreState](double, double, double speed) {
        qDebug() << "[Main] GNSS Speed Update:" << speed;
        coreState->setSpeed(static_cast<int>(speed));
    });

    // D-Bus adaptor
    CoreDbusAdaptor *dbusAdaptor =
        new CoreDbusAdaptor(coreState, landmarkEngine, securityManager, &app);

    // Connect GnssManager stability signal directly to D-Bus adaptor
    QObject::connect(gnssManager, &IGnssSource::gnssStabilityChanged,
                     dbusAdaptor, &CoreDbusAdaptor::onGnssStabilityChanged);

    // Sync initial stability state to adaptor so UI can query it on startup
    dbusAdaptor->onGnssStabilityChanged(gnssManager->isGnssStable());

    // GNSS mode change from UI
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::gnssModeChangeRequested,
                     gnssManager, [gnssManager](int mode) {
        GnssManager::Mode m =
            (mode == 1) ? GnssManager::Mode::Simulation
                        : GnssManager::Mode::Real;
        qInfo() << "GNSS mode changed via D-Bus:" << mode;
        gnssManager->setMode(m);
    });

    // Notify LandmarkEngine of mode changes so it knows whether to clear stale data
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::gnssModeChangeRequested,
                     landmarkEngine, &LandmarkEngine::setGnssMode);

    // Operation Mode updates: LandmarkEngine -> D-Bus
    QObject::connect(landmarkEngine, &LandmarkEngine::operationModeChanged,
                     dbusAdaptor, &CoreDbusAdaptor::OperationModeUpdated);

    // Weather / fog mode
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::weatherModeChangeRequested,
                     coreState, &CoreState::setFogMode);

    // Speed updates: CoreState -> D-Bus
    QObject::connect(coreState, &CoreState::speedChanged,
                     dbusAdaptor, &CoreDbusAdaptor::SpeedUpdated);

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
