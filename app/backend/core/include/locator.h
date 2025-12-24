#pragma once

#include <QObject>
#include <QMutex>

#include "IGnssSource.h"

class Locator : public QObject
{
    Q_OBJECT

public:
    struct Position {
        double latitude  {0.0};
        double longitude {0.0};
        double speedKmh  {0.0};
    };

    explicit Locator(IGnssSource *gnssSource,
                     QObject *parent = nullptr);

    // Called periodically (timer / worker thread)
    void update();

    Position position() const;
    bool isGnssStable() const;

private:
    void updateFromGnss();
    void updateFromImu();   // placeholder for future

private:
    IGnssSource *m_gnss {nullptr};

    mutable QMutex m_mutex;
    Position m_currentPosition;
    bool m_gnssStable {false};
};
