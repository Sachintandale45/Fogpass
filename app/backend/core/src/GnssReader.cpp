#include "GnssReader.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <system_error>

GnssReader::GnssReader() : m_running(false)
{

}

GnssReader::~GnssReader()
{
    stop();
}

bool GnssReader::start(const std::string &portName, int baudRate)
{
    if (m_running) {
        stop();
    }

    m_fd = open(portName.c_str(), O_RDONLY | O_NOCTTY);
    if (m_fd < 0) {
        std::cerr << "[GNSS] Failed to open " << portName << ": " << std::system_category().message(errno) << std::endl;
        return false;
    }

    termios tty;
    if (tcgetattr(m_fd, &tty) != 0) {
        std::cerr << "[GNSS] Failed to get termios attributes: " << std::system_category().message(errno) << std::endl;
        close(m_fd);
        m_fd = -1;
        return false;
    }

    speed_t realBaud;
    switch(baudRate) {
        case 9600:   realBaud = B9600;   break;
        case 19200:  realBaud = B19200;  break;
        case 38400:  realBaud = B38400;  break;
        case 57600:  realBaud = B57600;  break;
        case 115200: realBaud = B115200; break;
        default:
            std::cerr << "[GNSS] Unsupported baud rate: " << baudRate << std::endl;
            close(m_fd);
            m_fd = -1;
            return false;
    }

    cfsetospeed(&tty, realBaud);
    cfsetispeed(&tty, realBaud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0; // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_cc[VMIN]  = 0; // read doesn't block
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(m_fd, TCSANOW, &tty) != 0) {
        std::cerr << "[GNSS] Failed to set termios attributes: " << std::system_category().message(errno) << std::endl;
        close(m_fd);
        m_fd = -1;
        return false;
    }

    m_running = true;
    m_readThread = std::thread(&GnssReader::readLoop, this);

    std::cout << "[GNSS] Connected to " << portName << " at " << baudRate << " baud." << std::endl;
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

void GnssReader::readLoop()
{
    std::string buffer;
    char read_buf[256];

    while (m_running) {
        ssize_t len = read(m_fd, read_buf, sizeof(read_buf));
        if (len > 0) {
            buffer.append(read_buf, len);

            size_t newline_pos;
            while ((newline_pos = buffer.find('\n')) != std::string::npos) {
                std::string sentence = buffer.substr(0, newline_pos);
                buffer.erase(0, newline_pos + 1);

                // Trim whitespace
                sentence.erase(0, sentence.find_first_not_of(" \t\n\r"));
                sentence.erase(sentence.find_last_not_of(" \t\n\r") + 1);

                if (!sentence.empty()) {
                    parseNmeaSentence(sentence);
                }
            }
        }
    }
}

void GnssReader::parseNmeaSentence(const std::string &sentence)
{
    // We look for $GNRMC (GNSS Recommended Minimum Navigation Information)
    // Format: $GNRMC,hhmmss.ss,A,llll.ll,a,yyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
    
    if (sentence.empty() || sentence[0] != '$') return;
m_latitude
    std::vector<std::string> parts;
    std::string current_part;
    for (char c : sentence) {
        if (c == ',') {
            parts.push_back(current_part);
            current_part.clear();
        } else {
            current_part += c;
        }
    }
    parts.push_back(current_part);

    if (parts.size() < 10) return;

    const std::string &type = parts[0];

    // Accept GNRMC (Multi-GNSS) or GPRMC (GPS only)
    if (type.length() > 3 && type.substr(type.length() - 3) == "RMC") {
        const std::string &status = parts[2];
        if (status == "A") { // A = Active/Valid Data
            std::lock_guard<std::mutex> lock(m_dataMutex);
            m_latitude = parseCoordinate(parts[3], parts[4]);
            m_longitude = parseCoordinate(parts[5], parts[6]);
            try {
                double speedKnots = std::stod(parts[7]);
                m_speedKmh = speedKnots * 1.852;
            } catch (const std::invalid_argument&) {
                // Ignore if speed is not a valid double
            }
        }
    }
}

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

double GnssReader::parseCoordinate(const std::string &val, const std::string &dir)
{
    if (val.empty()) return 0.0;

    // NMEA format is DDDMM.MMMMM (Degrees + Minutes)
    // We need to convert this to Decimal Degrees
    size_t dotIndex = val.find('.');
    if (dotIndex == std::string::npos) return 0.0;

    // The degrees are the digits before the last 2 integer digits of the minutes
    if (dotIndex < 2) return 0.0;
    size_t degreesLen = dotIndex - 2;

    double degrees = 0.0;
    double minutes = 0.0;
    try {
        degrees = std::stod(val.substr(0, degreesLen));
        minutes = std::stod(val.substr(degreesLen));
    } catch (const std::invalid_argument&) {
        return 0.0; // Failed to parse
    }

    double decimal = degrees + (minutes / 60.0);

    if (dir == "S" || dir == "W") {
        decimal = -decimal;
    }

    return decimal;
}