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
    // The modeUpdated signal from CoreClient is not yet implemented as per previous steps

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
