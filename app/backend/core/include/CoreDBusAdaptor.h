#pragma once

#include <QObject>
#include <QtDBus/QtDBus>
#include <QDebug>

class CoreState;

class CoreDbusAdaptor : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.fogpass.Core")

public:
    explicit CoreDbusAdaptor(CoreState *coreState,
                             QObject *parent = nullptr);

signals:
    // -------- D-Bus signals (Core → UI) --------

    // Alerts
    void AlertRaised(const QString &alertId);

    // Landmark updates
    void NextLandmarksUpdated(const QString &name1, int dist1,
                              const QString &name2, int dist2,
                              const QString &name3, int dist3);
    
    // Internal signal to notify main.cpp to switch modes
    void gnssModeChangeRequested(int mode);
    void weatherModeChangeRequested(bool foggy);

public slots:
    // D-Bus method: SetGnssMode(int mode) -> 0=Real, 1=Simulation
    void SetGnssMode(int mode) { 
        qDebug() << "CoreDBusAdaptor: SetGnssMode called with" << mode;
        emit gnssModeChangeRequested(mode); 
    }

    // D-Bus method: SetWeatherMode(bool foggy)
    void SetWeatherMode(bool foggy) {
        qDebug() << "CoreDBusAdaptor: SetWeatherMode called with" << foggy;
        emit weatherModeChangeRequested(foggy);
    }

private slots:
    // -------- Internal slots (CoreState → Adaptor) --------
    void onAlertRaised(const QString &alertId);

    void onNextLandmarksUpdated(const QString &name1, int dist1,
                                const QString &name2, int dist2,
                                const QString &name3, int dist3);

private:
    CoreState *m_coreState;
};
