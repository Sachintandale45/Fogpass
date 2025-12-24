#pragma once

#include <QObject>
#include <thread>
#include <mutex>
#include <string>

#include "IGnssSource.h"

/*
 * GnssReader
 *
 * REAL GNSS implementation using UART + NMEA
 * Implements IGnssSource
 */

class GnssReader : public IGnssSource
{
    Q_OBJECT

public:
    explicit GnssReader(QObject *parent = nullptr);
    ~GnssReader() override;

    // -------- IGnssSource API --------
    bool start() override;     // uses stored port/baud
    void stop() override;

    double latitude()  const override;
    double longitude() const override;
    double speedKmh()  const override;
    bool isGnssStable() const override;

    // -------- Real GNSS specific API --------
    // Configure the serial port before starting
    void configure(const std::string &portName, int baudRate);

private:
    void readLoop();
    void parseNmeaSentence(const std::string &sentence);
    double parseCoordinate(const std::string &val,
                           const std::string &dir);

private:
    // UART
    int m_fd {-1};
    std::thread m_readThread;
    bool m_running {false};

    // Config (for IGnssSource::start())
    std::string m_portName;
    int m_baudRate {115200};

    // GNSS data
    mutable std::mutex m_dataMutex;
    double m_latitude  {0.0};
    double m_longitude {0.0};
    double m_speedKmh  {0.0};
    bool   m_gnssStable {false};
};
