#include "GnssReader.h"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <system_error>

// ------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------

GnssReader::GnssReader(QObject *parent)
    : IGnssSource(parent)
{
}

GnssReader::~GnssReader()
{
    stop();
}

// ------------------------------------------------------------
// IGnssSource start / stop
// ------------------------------------------------------------

bool GnssReader::start()
{
    // Use stored configuration
    if (m_portName.empty()) {
        std::cerr << "[GNSS] start() failed: port not configured\n";
        return false;
    }
    return start(m_portName, m_baudRate);
}

// ------------------------------------------------------------
// Real GNSS start / stop
// ------------------------------------------------------------

bool GnssReader::start(const std::string &portName, int baudRate)
{
    if (m_running) {
        stop();
    }

    m_portName = portName;
    m_baudRate = baudRate;

    m_fd = open(portName.c_str(), O_RDONLY | O_NOCTTY);
    if (m_fd < 0) {
        std::cerr << "[GNSS] Failed to open " << portName
                  << ": " << std::system_category().message(errno)
                  << std::endl;
        return false;
    }

    termios tty{};
    if (tcgetattr(m_fd, &tty) != 0) {
        std::cerr << "[GNSS] tcgetattr failed: "
                  << std::system_category().message(errno)
                  << std::endl;
        close(m_fd);
        m_fd = -1;
        return false;
    }

    speed_t realBaud;
    switch (baudRate) {
        case 9600:   realBaud = B9600; break;
        case 19200:  realBaud = B19200; break;
        case 38400:  realBaud = B38400; break;
        case 57600:  realBaud = B57600; break;
        case 115200: realBaud = B115200; break;
        default:
            std::cerr << "[GNSS] Unsupported baud rate: "
                      << baudRate << std::endl;
            close(m_fd);
            m_fd = -1;
            return false;
    }

    cfsetospeed(&tty, realBaud);
    cfsetispeed(&tty, realBaud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(m_fd, TCSANOW, &tty) != 0) {
        std::cerr << "[GNSS] tcsetattr failed: "
                  << std::system_category().message(errno)
                  << std::endl;
        close(m_fd);
        m_fd = -1;
        return false;
    }

    m_running = true;
    m_readThread = std::thread(&GnssReader::readLoop, this);

    std::cout << "[GNSS] Connected to " << portName
              << " @ " << baudRate << " baud" << std::endl;

    return true;
}

void GnssReader::stop()
{
    m_running = false;

    if (m_readThread.joinable()) {
        m_readThread.join();
    }

    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
        std::cout << "[GNSS] Serial port closed." << std::endl;
    }
}

// ------------------------------------------------------------
// Worker thread
// ------------------------------------------------------------

void GnssReader::readLoop()
{
    std::string buffer;
    char readBuf[256];

    while (m_running) {
        ssize_t len = read(m_fd, readBuf, sizeof(readBuf));
        if (len > 0) {
            buffer.append(readBuf, len);

            size_t pos;
            while ((pos = buffer.find('\n')) != std::string::npos) {
                std::string sentence = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);

                sentence.erase(0, sentence.find_first_not_of(" \r\n\t"));
                sentence.erase(sentence.find_last_not_of(" \r\n\t") + 1);

                if (!sentence.empty()) {
                    parseNmeaSentence(sentence);
                }
            }
        }
    }
}

// ------------------------------------------------------------
// NMEA parsing
// ------------------------------------------------------------

void GnssReader::parseNmeaSentence(const std::string &sentence)
{
    if (sentence.empty() || sentence[0] != '$')
        return;

    std::vector<std::string> parts;
    std::string token;

    for (char c : sentence) {
        if (c == ',') {
            parts.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    parts.push_back(token);

    if (parts.size() < 10)
        return;

    const std::string &type = parts[0];

    if (type.size() >= 3 &&
        type.substr(type.size() - 3) == "RMC") {

        const std::string &status = parts[2];
        bool newStable = (status == "A");

        std::lock_guard<std::mutex> lock(m_dataMutex);

        if (newStable != m_gnssStable) {
            m_gnssStable = newStable;
            emit gnssStabilityChanged(m_gnssStable);
        }

        if (m_gnssStable) {
            m_latitude  = parseCoordinate(parts[3], parts[4]);
            m_longitude = parseCoordinate(parts[5], parts[6]);

            try {
                double speedKnots = std::stod(parts[7]);
                m_speedKmh = speedKnots * 1.852;
            } catch (...) {}

            emit positionUpdated(m_latitude,
                                 m_longitude,
                                 m_speedKmh);
        }
    }
}

// ------------------------------------------------------------
// Thread-safe getters
// ------------------------------------------------------------

double GnssReader::latitude() const
{
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return m_latitude;
}

double GnssReader::longitude() const
{
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return m_longitude;
}

double GnssReader::speedKmh() const
{
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return m_speedKmh;
}

bool GnssReader::isGnssStable() const
{
    std::lock_guard<std::mutex> lock(m_dataMutex);
    return m_gnssStable;
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

double GnssReader::parseCoordinate(const std::string &val,
                                   const std::string &dir)
{
    if (val.empty())
        return 0.0;

    size_t dot = val.find('.');
    if (dot == std::string::npos || dot < 2)
        return 0.0;

    try {
        double degrees = std::stod(val.substr(0, dot - 2));
        double minutes = std::stod(val.substr(dot - 2));
        double decimal = degrees + (minutes / 60.0);

        if (dir == "S" || dir == "W")
            decimal = -decimal;

        return decimal;
    } catch (...) {
        return 0.0;
    }
}
