#include "LandmarkEngine.h"
#include "Locator.h"
#include "CoreState.h"

#include <QDebug>

LandmarkEngine::LandmarkEngine(Locator *locator,
                               CoreState *coreState,
                               QObject *parent)
    : QObject(parent),
      m_locator(locator),
      m_coreState(coreState)
{
    Q_ASSERT(m_locator);
    Q_ASSERT(m_coreState);

    // Timer setup (1 second update)
    connect(&m_timer, &QTimer::timeout,
            this, &LandmarkEngine::process);
}

bool LandmarkEngine::loadRouteFile(const QString &filePath)
{
    Q_UNUSED(filePath)

    // TODO:
    // - Open CSV
    // - Parse landmarks
    // - Populate m_routeLandmarks

    qDebug() << "[LandmarkEngine] Route file loaded:" << filePath;
    return true;
}

void LandmarkEngine::start()
{
    if (!m_timer.isActive()) {
        m_timer.start(1000);  // every 1 second
        qDebug() << "[LandmarkEngine] Started";
    }
}

void LandmarkEngine::stop()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        qDebug() << "[LandmarkEngine] Stopped";
    }
}

void LandmarkEngine::process()
{
    // 1️⃣ Get current position from Locator
    double lat = 0.0;
    double lon = 0.0;
    double speed = 0.0;

    if (!m_locator->position(lat, lon, speed)) {
        qDebug() << "[LandmarkEngine] Position not available";
        return;
    }

    // 2️⃣ Compute next landmarks (logic later)
    computeNextLandmarks(lat, lon);

    // 3️⃣ Compute distances (dummy values for now)
    int d1 = 1200;
    int d2 = 2400;
    int d3 = 3600;

    // 4️⃣ Push continuous state to CoreState
    m_coreState->updateNextLandmarks(
        m_next1.name, d1,
        m_next2.name, d2,
        m_next3.name, d3
    );
}

void LandmarkEngine::computeNextLandmarks(double curLat, double curLon)
{
    Q_UNUSED(curLat)
    Q_UNUSED(curLon)

    // TODO:
    // - Find closest landmark ahead
    // - Select next 3 landmarks
    // - Compute remaining distances

    // Placeholder data
    m_next1.name = "Station A";
    m_next2.name = "Bridge";
    m_next3.name = "Tunnel";
}
