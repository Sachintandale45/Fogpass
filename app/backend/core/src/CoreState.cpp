#include "CoreState.h"
#include <QDebug>

CoreState::CoreState(QObject *parent)
    : QObject(parent)
{
}

/* ===================== Fog mode ===================== */

void CoreState::setFogMode(bool foggy)
{
    QMutexLocker locker(&m_mutex);

    if (m_foggy == foggy)
        return;

    m_foggy = foggy;
    qInfo() << "[CoreState] Fog mode changed to:" << (m_foggy ? "ENABLED" : "DISABLED");
    emit fogModeChanged(m_foggy);
}

bool CoreState::fogMode() const
{
    QMutexLocker locker(&m_mutex);
    return m_foggy;
}

/* ===================== Speed ===================== */

void CoreState::setSpeed(int speed)
{
    QMutexLocker locker(&m_mutex);
    // Disable deduplication to ensure UI receives the value even if it connects late
    // if (m_speed == speed) return;

    m_speed = speed;
    qDebug() << "[CoreState] Emitting speedChanged:" << m_speed << "(D-Bus should receive this)";
    emit speedChanged(m_speed);
}

int CoreState::speed() const
{
    QMutexLocker locker(&m_mutex);
    return m_speed;
}

/* ===================== Landmarks ===================== */

void CoreState::updateNextLandmarks(
    const QString &name1, int dist1,
    const QString &name2, int dist2,
    const QString &name3, int dist3
)
{
    QMutexLocker locker(&m_mutex);

    m_l1Name = name1;
    m_l1Dist = dist1;
    m_l2Name = name2;
    m_l2Dist = dist2;
    m_l3Name = name3;
    m_l3Dist = dist3;

    emit nextLandmarksUpdated(
        name1, dist1,
        name2, dist2,
        name3, dist3
    );
}

/* ===================== Alerts ===================== */

void CoreState::raiseAlert(const QString &alertId)
{
    QMutexLocker locker(&m_mutex);

    if (m_activeAlerts.contains(alertId))
        return;

    m_activeAlerts.insert(alertId);
    qDebug() << "[CoreState] Alert raised:" << alertId;

    emit alertRaised(alertId);
}

void CoreState::clearAlert(const QString &alertId)
{
    QMutexLocker locker(&m_mutex);

    if (!m_activeAlerts.contains(alertId))
        return;

    m_activeAlerts.remove(alertId);
    qDebug() << "[CoreState] Alert cleared:" << alertId;

    emit alertCleared(alertId);
}

bool CoreState::isAlertActive(const QString &alertId) const
{
    QMutexLocker locker(&m_mutex);
    return m_activeAlerts.contains(alertId);
}
