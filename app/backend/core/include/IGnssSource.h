#pragma once

#include <QObject>

/*
 * IGnssSource
 *
 * Pure interface for GNSS data sources.
 *
 * Implementations:
 *  - RealGnssReader      (UART / real hardware)
 *  - SimulatedGnssReader (CSV / simulation)
 *
 * Locator and higher layers MUST depend only on this interface.
 */

class IGnssSource : public QObject
{
    Q_OBJECT

public:
    explicit IGnssSource(QObject *parent = nullptr)
        : QObject(parent) {}

    virtual ~IGnssSource() = default;

    // ---------------- GNSS lifecycle ----------------
    virtual bool start() = 0;
    virtual void stop() = 0;

    // ---------------- GNSS data ----------------
    virtual double latitude()  const = 0;
    virtual double longitude() const = 0;
    virtual double speedKmh()  const = 0;

    // ---------------- GNSS health ----------------
    virtual bool isGnssStable() const = 0;

signals:
    /*
     * Emitted when GNSS stability changes.
     * true  = stable (valid fix)
     * false = unstable / lost fix
     */
    void gnssStabilityChanged(bool stable);

    /*
     * Emitted when new GNSS data is available.
     * Useful for reactive designs (optional usage).
     */
    void positionUpdated(double latitude,
                         double longitude,
                         double speedKmh);
};
