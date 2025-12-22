#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class GnssReader
{
public:
    GnssReader();
    ~GnssReader();

    // Main API to start reading from the module
    // portName: e.g., "/dev/ttyACM0"
    // baudRate: u-blox default is usually 9600 or 38400
    bool start(const std::string &portName, int baudRate = 9600);
    void stop();

    double latitude() const;
    double longitude() const;
    double speedKmh() const;

private:
    void readLoop();
    void parseNmeaSentence(const std::string &sentence);
    double parseCoordinate(const std::string &val, const std::string &dir);

    int m_fd = -1;
    std::thread m_readThread;
    std::atomic<bool> m_running;

    mutable std::mutex m_dataMutex;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    double m_speedKmh = 0.0;
};