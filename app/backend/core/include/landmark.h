#pragma once

#include <QObject>
#include <QTimer>
#include <QString>

class Locator;
class CoreState;

class LandmarkEngine : public QObject
{
    Q_OBJECT

public:
    explicit LandmarkEngine(Locator *locator,
                            CoreState *coreState,
                            QObject *parent = nullptr);

    // Load route / landmark data (CSV, JSON, etc.)
    bool loadRouteFile(const QString &filePath);

    // Start / stop processing
    void start();
    void stop();

private slots:
    void process();   // runs periodically

private:
    struct Landmark {
        QString name;
        double latitude;
        double longitude;
    };

    // Helpers (logic to be implemented later)
    void computeNextLandmarks(double curLat, double curLon);

    // Dependencies
    Locator   *m_locator;
    CoreState *m_coreState;

    // Timer for periodic updates
    QTimer m_timer;

    // Route data
    QVector<Landmark> m_routeLandmarks;

    // Cached next landmarks
    Landmark m_next1;
    Landmark m_next2;
    Landmark m_next3;
};
