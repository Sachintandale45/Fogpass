#include <QCoreApplication>
#include <QDebug>

// Core modules
#include "CoreState.h"
#include "CoreDBusAdaptor.h"

// GNSS & navigation
#include "GnssReader.h"              // REAL GNSS
#include "SimulatedGnssReader.h"     // SIM GNSS
#include "GnssManager.h"             // GNSS switch
#include "locator.h"

// Logic
#include "LandmarkEngine.h"
#include "alertmanager.h"

int main(int argc, char *argv[])
{
    // ------------------------------------------------------------
    // 1️⃣ Qt core application (event loop owner)
    // ------------------------------------------------------------
    QCoreApplication app(argc, argv);
    qInfo() << "FogPass Core Service starting...";

    // ------------------------------------------------------------
    // 2️⃣ CoreState (single source of truth)
    // ------------------------------------------------------------
    CoreState *coreState = new CoreState(&app);

    // ------------------------------------------------------------
    // 3️⃣ GNSS SOURCES
    // ------------------------------------------------------------

    // Real GNSS (UART-based)
    GnssReader *realGnss = new GnssReader(&app);
    // Configure the real GNSS reader, but don't start it directly.
    // The GnssManager will handle starting/stopping.
    realGnss->configure("/dev/ttyS6", 115200);

    // Simulated GNSS (CSV-based)
    SimulatedGnssReader *simGnss = new SimulatedGnssReader(&app);

    // Load simulation file (can be changed later via UI / D-Bus)
    simGnss->loadCsvFile("/data/sim/gnss_simulation.csv");

    // ------------------------------------------------------------
    // 4️⃣ GNSS MANAGER (runtime switch)
    // ------------------------------------------------------------
    GnssManager *gnssManager =
        new GnssManager(realGnss, simGnss, &app);

    // Start GNSS manager (will activate current mode)
    gnssManager->start();

    // ------------------------------------------------------------
    // 5️⃣ Locator (position abstraction)
    // ------------------------------------------------------------
    Locator *locator = new Locator(gnssManager, &app);

    // ------------------------------------------------------------
    // 6️⃣ Landmark engine (navigation logic)
    // ------------------------------------------------------------
    LandmarkEngine *landmarkEngine =
        new LandmarkEngine(locator, coreState, &app);

    landmarkEngine->loadRouteFile("/data/routes/route1.csv");
    landmarkEngine->start();

    // ------------------------------------------------------------
    // 7️⃣ Alert manager (decision logic)
    // ------------------------------------------------------------
    AlertManager *alertManager =
        new AlertManager(coreState, &app);

    // ------------------------------------------------------------
    // 🔗 SIGNAL WIRING (composition root)
    // ------------------------------------------------------------

    // GNSS stability → AlertManager
    QObject::connect(
        gnssManager, &IGnssSource::gnssStabilityChanged,
        alertManager, &AlertManager::onGnssStabilityChanged,
        Qt::QueuedConnection
    );

    // Landmark updates → AlertManager
    QObject::connect(
        coreState, &CoreState::nextLandmarksUpdated,
        alertManager, &AlertManager::onNextLandmarksUpdated,
        Qt::QueuedConnection
    );

    // ------------------------------------------------------------
    // 8️⃣ D-Bus adaptor (Core → UI IPC)
    // ------------------------------------------------------------
    CoreDbusAdaptor *dbusAdaptor =
        new CoreDbusAdaptor(coreState);

    // Wire up D-Bus GNSS mode switch to GnssManager
    QObject::connect(dbusAdaptor, &CoreDbusAdaptor::gnssModeChangeRequested,
                     gnssManager, [gnssManager](int mode){
        GnssManager::Mode m = (mode == 1) ? GnssManager::Mode::Simulation
                                          : GnssManager::Mode::Real;
        qInfo() << "D-Bus requested GNSS mode change to:" << (mode == 1 ? "Simulation" : "Real");
        gnssManager->setMode(m);
    });

    qInfo() << "FogPass Core Service initialized. Entering event loop.";

    // ------------------------------------------------------------
    // 9️⃣ Start Qt event loop
    // ------------------------------------------------------------
    return app.exec();
}
