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
    connect(m_core, SIGNAL(speedUpdated(int)), this, SLOT(onSpeedUpdated(int)));
    connect(m_core, &CoreClient::gnssStabilityChanged, this, &BackendManager::onCoreGnssStabilityChanged);
    connect(m_core, &CoreClient::accessGranted, this, &BackendManager::onAccessGranted);
    connect(m_core, &CoreClient::accessDenied, this, &BackendManager::onAccessDenied);

    // Sync initial state from Core
    bool initialStable = m_core->getGnssStability();
    if (m_isGnssStable != initialStable) {
        m_isGnssStable = initialStable;
        emit gnssStabilityChanged(m_isGnssStable);
    }

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
    // Disabled: CoreClient no longer supports manual requests.
    // Updates are pushed automatically via signals.
}

QString BackendManager::landmark1() const { return m_landmark1; }
int BackendManager::distance1() const { return m_dist1; }
QString BackendManager::landmark2() const { return m_landmark2; }
int BackendManager::distance2() const { return m_dist2; }
QString BackendManager::landmark3() const { return m_landmark3; }
int BackendManager::distance3() const { return m_dist3; }
int BackendManager::speed() const { return m_speed; }
QString BackendManager::operationModeLabel() const 
{
    switch(m_opMode) {
        case 1: return "MANUAL";
        case 2: return "AUTO";
        case 0: 
        default: return "IDLE";
    }
}

bool BackendManager::isGnssStable() const
{
    return m_isGnssStable;
}

void BackendManager::onLandmarksUpdated(const QString &l1, int d1, const QString &l2, int d2, const QString &l3, int d3)
{
    qDebug() << "BackendManager: Updating landmarks ->" << l1 << d1;

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

void BackendManager::onSpeedUpdated(int speed)
{
    qDebug() << "BackendManager: Received speed update:" << speed;
    if (m_speed != speed) {
        m_speed = speed;
        emit speedUpdated(m_speed);
    }
}

void BackendManager::onCoreGnssStabilityChanged(bool stable)
{
    if (m_isGnssStable != stable) {
        m_isGnssStable = stable;
        emit gnssStabilityChanged(m_isGnssStable);
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
    emit modeUpdated(newMode);   //this is loopback from ui only to ui. not from backend
}

void BackendManager::setGnssMode(bool simulation)
{
    // 0 = Real, 1 = Simulation
    int mode = simulation ? 1 : 0;
    m_core->setGnssMode(mode);
}

void BackendManager::SetOperationMode(int mode)
{
    // Update local state immediately for UI responsiveness
    if (m_opMode != mode) {
        m_opMode = mode;
        emit operationModeChanged();
    }
    m_core->setOperationMode(mode);
}

QStringList BackendManager::GetAvailableRoutes()
{
    return m_core->getAvailableRoutes();
}

void BackendManager::SelectRoute(const QString &routeName)
{
    m_core->selectRoute(routeName);
}


void BackendManager::ClearRoute()
{
    m_core->clearRoute();
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

bool BackendManager::requestAccess(const QString &capability, const QString &password)
{
    return m_core->requestAccess(capability, password);
}

bool BackendManager::changePassword(const QString &capability, const QString &oldPassword, const QString &newPassword)
{
    return m_core->changePassword(capability, oldPassword, newPassword);
}

void BackendManager::onAccessGranted(const QString &capability)
{
    qInfo() << "BackendManager: Access granted for" << capability;
    emit accessGranted(capability);
}

void BackendManager::onAccessDenied(const QString &capability)
{
    qInfo() << "BackendManager: Access denied for" << capability;
    emit accessDenied(capability);
}
