#pragma once

#include <QObject>

class CoreState;

class AlertManager : public QObject
{
    Q_OBJECT
public:
    explicit AlertManager(CoreState *coreState, QObject *parent = nullptr);

public slots:
    // GNSS
    void onGnssStabilityChanged(bool stable);

    // Landmark distance update (from CoreState)
    void onNextLandmarksUpdated(
        const QString &name1, int dist1,
        const QString &name2, int dist2,
        const QString &name3, int dist3
    );

private:
    CoreState *m_coreState;

    // Internal latches
    bool m_gnssUnstableActive = false;
    bool m_landmarkPrewarnActive = false;

    // Configurable parameter (later from config file)
    int m_landmarkPrewarnDistance = 500;   // meters
};
