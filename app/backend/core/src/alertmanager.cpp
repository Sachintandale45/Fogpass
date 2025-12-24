#include "alertmanager.h"
#include "CoreState.h"

#include <QDebug>

AlertManager::AlertManager(CoreState *coreState, QObject *parent)
    : QObject(parent),
      m_coreState(coreState)
{
    Q_ASSERT(m_coreState);
}

/* ===================== GNSS ALERT ===================== */

void AlertManager::onGnssStabilityChanged(bool stable)
{
    if (!stable && !m_gnssUnstableActive) {
        qDebug() << "[AlertManager] GNSS UNSTABLE";
        m_coreState->raiseAlert("GNSS_UNSTABLE");
        m_gnssUnstableActive = true;
    }

    if (stable && m_gnssUnstableActive) {
        qDebug() << "[AlertManager] GNSS STABLE";
        m_coreState->clearAlert("GNSS_UNSTABLE");
        m_gnssUnstableActive = false;
    }
}

/* ===================== LANDMARK PRE-WARNING ===================== */

void AlertManager::onNextLandmarksUpdated(
    const QString &name1, int dist1,
    const QString &name2, int dist2,
    const QString &name3, int dist3
)
{
    Q_UNUSED(name2)
    Q_UNUSED(dist2)
    Q_UNUSED(name3)
    Q_UNUSED(dist3)

    // Ignore invalid distance
    if (dist1 <= 0)
        return;

    // ---- Raise pre-warning ----
    if (dist1 <= m_landmarkPrewarnDistance && !m_landmarkPrewarnActive) {
        qDebug() << "[AlertManager] Landmark pre-warning:" << name1
                 << "distance:" << dist1 << "m";

        m_coreState->raiseAlert("LANDMARK_PREWARN");
        m_landmarkPrewarnActive = true;
    }

    // ---- Clear after landmark passed ----
    if (dist1 > m_landmarkPrewarnDistance + 100 && m_landmarkPrewarnActive) {
        qDebug() << "[AlertManager] Landmark pre-warning cleared";

        m_coreState->clearAlert("LANDMARK_PREWARN");
        m_landmarkPrewarnActive = false;
    }
}
