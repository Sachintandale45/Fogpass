#include "backendmanager.h"
#include <QDebug>
#include "AudioClient.h"
#include "PowerClient.h"
#include "CoreClient.h"

BackendManager::BackendManager(QObject *parent)
    : QObject(parent)
    , m_batteryLevel(87) // Start from the same default as AppSettings
{
    // --- Client Initialization ---
    m_audio = new AudioClient(this);
    m_power = new PowerClient(this);
    m_core = new CoreClient(this);

    // ---- Forward service signals to UI ----
    connect(m_audio, &AudioClient::volumeChanged, this, &BackendManager::volumeUpdated);
    connect(m_power, &PowerClient::batteryLevelChanged, this, &BackendManager::batteryLevelUpdated);
    // Connect the landmark locations signal from the CoreClient to the BackendManager's signal
    connect(m_core, &CoreClient::landmarkLocationsChanged, this, &BackendManager::landmarkLocationsUpdated);
    connect(m_core, &CoreClient::landmarksUpdated, this, &BackendManager::onLandmarksUpdated);

    // --- Battery Simulation Timer ---
    m_timer.setInterval(1000); // Update once per second
    m_timer.setTimerType(Qt::CoarseTimer);
    connect(&m_timer, &QTimer::timeout, this, &BackendManager::onTimeout);
}

void BackendManager::setVolume(int volume)
{
    // UI command -> system backend
    m_audio->setVolume(volume);
}

void BackendManager::requestLandmarks()
{
    m_core->requestLandmarkLocations();
}

QString BackendManager::landmark1() const { return m_landmark1; }
int BackendManager::distance1() const { return m_dist1; }
QString BackendManager::landmark2() const { return m_landmark2; }
int BackendManager::distance2() const { return m_dist2; }
QString BackendManager::landmark3() const { return m_landmark3; }
int BackendManager::distance3() const { return m_dist3; }

void BackendManager::onLandmarksUpdated(const QString &l1, int d1, const QString &l2, int d2, const QString &l3, int d3)
{
    bool changed = false;
    if (m_landmark1 != l1) { m_landmark1 = l1; changed = true; }
    if (m_dist1 != d1) { m_dist1 = d1; changed = true; }
    if (m_landmark2 != l2) { m_landmark2 = l2; changed = true; }
    if (m_dist2 != d2) { m_dist2 = d2; changed = true; }
    if (m_landmark3 != l3) { m_landmark3 = l3; changed = true; }
    if (m_dist3 != d3) { m_dist3 = d3; changed = true; }

    if (changed) {
        emit landmarksChanged();
    }
}

void BackendManager::start()
{
    qDebug() << "BackendManager: Starting battery simulation.";
    if (!m_timer.isActive())
        m_timer.start();
}

void BackendManager::setWeatherMode(bool foggy)
{
    // UI command -> system backend
    m_core->setWeatherMode(foggy);

    // After sending the command, emit the signal to update the UI immediately.
    // This provides instant feedback to the user.
    const QString newMode = foggy ? "Foggy" : "Non-Foggy";
    emit modeUpdated(newMode);
}

void BackendManager::onTimeout()
{
    if (m_batteryLevel > 25) { // Stop at 25
        m_batteryLevel -= 1; // simulate gradual drop
        emit batteryLevelUpdated(m_batteryLevel); // Emit signal with the new value
    } else {
        m_timer.stop();
    }
}
