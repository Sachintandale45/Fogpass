#pragma once

#include <QObject>
#include <QMutex>

class GnssReader;

/**
 * @brief Locator
 *
 * Responsible for providing current position (latitude, longitude, speed).
 * Uses GNSS when stable, falls back to IMU-based estimation when GNSS is unstable.
 *
 * This class does NOT own threads and does NOT talk to D-Bus.
 */
class Locator : public QObject
{
    Q_OBJECT

public:
    struct Position {
        double latitude  = 0.0;
        double longitude = 0.0;
        double speedKmh  = 0.0;
    };

    explicit Locator(GnssReader* gnssReader, QObject* parent = nullptr);

    /**
     * @brief Returns the latest computed position.
     * Thread-safe snapshot.
     */
    Position position() const;

    /**
     * @brief Returns whether GNSS is currently stable.
     */
    bool isGnssStable() const;

public slots:
    /**
     * @brief Main update entry point.
     * Called periodically (e.g. by timer or controlling thread).
     */
    void update();

private:
    void updateFromGnss();
    void updateFromImu();   // placeholder for future IMU logic

private:
    GnssReader* m_gnss;     // NOT owned
    mutable QMutex m_mutex;

    Position m_currentPosition;
    bool m_gnssStable = true;
};
