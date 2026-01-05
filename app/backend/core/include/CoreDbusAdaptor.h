#pragma once

#include <QObject>
#include <QtDBus/QtDBus>
#include <QDebug>

class CoreState;
class LandmarkEngine;
class SecurityManager;

class CoreDbusAdaptor : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.fogpass.Core")

public:
    explicit CoreDbusAdaptor(CoreState *coreState,
                             LandmarkEngine *landmarkEngine,
                             SecurityManager *securityManager,
                             QObject *parent = nullptr);

signals:
    // -------- D-Bus signals (Core → UI) --------

    // Alerts
    void AlertRaised(const QString &alertId);

    // Speed
    void SpeedUpdated(int speed);

    // Operation Mode
    void OperationModeUpdated(int mode);

    // Landmark updates
    void NextLandmarksUpdated(const QString &name1, int dist1,
                              const QString &name2, int dist2,
                              const QString &name3, int dist3);

    // Signal to UI: GNSS Stability Changed
    void GnssStabilityChanged(bool stable);
    
    // Security
    void AccessGranted(const QString &capability);
    void AccessDenied(const QString &capability);

    // Internal signal to notify main.cpp to switch modes
    void gnssModeChangeRequested(int mode);
    void weatherModeChangeRequested(bool foggy);

public slots:
    // D-Bus method: SetGnssMode(int mode) -> 0=Real, 1=Simulation
    void SetGnssMode(int mode);

    // D-Bus method: Get current stability state
    bool GetGnssStability() { return m_lastKnownStability; }

    // D-Bus method: SetWeatherMode(bool foggy)
    void SetWeatherMode(bool foggy);

    // New Navigation API
    void SetOperationMode(int mode);
    QStringList GetAvailableRoutes();
    void SelectRoute(const QString &routeName);
    void ClearRoute();

    // Security
    bool requestAccess(const QString &capability, const QString &password);
    bool changePassword(const QString &capability, const QString &oldPassword, const QString &newPassword);

    // Slot to forward signal from LandmarkEngine to D-Bus
    void onGnssStabilityChanged(bool stable) {
        m_lastKnownStability = stable;
        qDebug() << "[CoreDbusAdaptor] Forwarding GnssStabilityChanged signal:" << stable;
        emit GnssStabilityChanged(stable);
    }

private slots:
    // -------- Internal slots (CoreState → Adaptor) --------
    void onAlertRaised(const QString &alertId);

    void onNextLandmarksUpdated(const QString &name1, int dist1,
                                const QString &name2, int dist2,
                                const QString &name3, int dist3);

private:
    CoreState *m_coreState;
    LandmarkEngine *m_landmarkEngine;
    SecurityManager *m_securityManager;
    bool m_lastKnownStability = false;
};
