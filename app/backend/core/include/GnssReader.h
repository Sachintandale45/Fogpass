#pragma once

#include <QObject>
#include <thread>
#include <mutex>
#include <string>

/*
 * GnssReader
 *
 * Responsibilities:
 *  - Open UART device (/dev/ttySx)
 *  - Read NMEA data in background thread
 *  - Parse RMC sentences
 *  - Maintain GNSS position & speed
 *  - Track GNSS stability
 *  - Emit signal when stability changes
 *
 * DOES NOT:
 *  - Talk to CoreState
 *  - Raise alerts
 *  - Use D-Bus
 */
class GnssReader : public QObject
{
    Q_OBJECT

public:
    explicit GnssReader(QObject *parent = nullptr);
    ~GnssReader();

    bool start(const std::string &portName, int baudRate);
    void stop();

    // Thread-safe getters
    double latitude() const;
    double longitude() const;
    double speedKmh() const;
    bool isGnssStable() const;

signals:
    // Emitted ONLY when GNSS stability changes
    void gnssStabilityChanged(bool stable);

private:
    // Worker thread
    void readLoop();

    // NMEA parsing
    void parseNmeaSentence(const std::string &sentence);
    double parseCoordinate(const std::string &val,
                           const std::string &dir);

private:
    // UART
    int m_fd{-1};

    // Thread control
    std::thread m_readThread;
    bool m_running{false};

    // GNSS data (protected)
    mutable std::mutex m_dataMutex;
    double m_latitude{0.0};
    double m_longitude{0.0};
    double m_speedKmh{0.0};

    // GNSS state
    bool m_gnssStable{false};
};
