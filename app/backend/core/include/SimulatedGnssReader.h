#pragma once

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMutex>
#include <QString>

#include "IGnssSource.h"

/*
 * SimulatedGnssReader
 *
 * Generates fake GNSS data from a CSV file.
 * Used when GNSS simulation mode is enabled.
 *
 * CSV format (example):
 *   latitude,longitude,speed_kmh
 *   18.5204,73.8567,40
 *   18.5206,73.8570,40
 *   18.5208,73.8574,40
 */

class SimulatedGnssReader : public IGnssSource
{
    Q_OBJECT

public:
    explicit SimulatedGnssReader(QObject *parent = nullptr);
    ~SimulatedGnssReader() override;

    struct GnssPoint {
        double latitude;
        double longitude;
        double speedKmh;
    };

    // Load simulation data file
    bool loadCsvFile(const QString &filePath);

    // -------- IGnssSource API --------
    bool start() override;
    void stop() override;

    double latitude()  const override;
    double longitude() const override;
    double speedKmh()  const override;
    bool isGnssStable() const override;


public slots:
    void onRouteSelected(const QString &routeName);

private slots:
    void onTimerTick();

private:
    void generateTrackPoints(const QVector<GnssPoint>& waypoints);
    static constexpr int INTERPOLATION_STEPS = 10;

    QVector<GnssPoint> m_points;
    int m_currentIndex {0};

    mutable QMutex m_mutex;
    QTimer m_timer;

    double m_latitude {0.0};
    double m_longitude {0.0};
    double m_speedKmh {0.0};

    bool m_running {false};
};
