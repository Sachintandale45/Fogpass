#pragma once

#include <QObject>
#include <QMutex>

#include "IGnssSource.h"

/*
 * GnssManager
 *
 * Responsibility:
 *  - Owns multiple GNSS sources (real + simulated)
 *  - Exposes ONE GNSS interface to the rest of the system
 *  - Switches GNSS source at runtime (UI-controlled)
 *
 * Locator MUST talk only to GnssManager.
 */

class GnssManager : public IGnssSource
{
    Q_OBJECT

public:
    enum class Mode {
        Real,
        Simulation
    };
    Q_ENUM(Mode)

    explicit GnssManager(IGnssSource *realGnss,
                         IGnssSource *simGnss,
                         QObject *parent = nullptr);

    ~GnssManager() override;

    // -------- IGnssSource API --------
    bool start() override;
    void stop() override;

    double latitude()  const override;
    double longitude() const override;
    double speedKmh()  const override;

    bool isGnssStable() const override;

public slots:
    // Called when UI / CoreState requests GNSS mode change
    void setMode(Mode mode);

signals:
    // Forwarded signals (from active GNSS source)
    void gnssModeChanged(GnssManager::Mode mode);

private slots:
    // Internal signal forwarding
    void onSourceStabilityChanged(bool stable);
    void onSourcePositionUpdated(double lat, double lon, double speed);

private:
    IGnssSource *activeSource() const;

private:
    IGnssSource *m_realGnss;
    IGnssSource *m_simGnss;

    Mode m_mode { Mode::Real };

    mutable QMutex m_mutex;
};
