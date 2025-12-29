#pragma once

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>

class Locator;
class CoreState;

class LandmarkEngine : public QObject
{
    Q_OBJECT

public:
    enum OperationMode {
        ModeIdle = 0,
        ModeManual = 1,
        ModeAuto = 2
    };
    Q_ENUM(OperationMode)

public:
    explicit LandmarkEngine(Locator *locator,
                            CoreState *coreState,
                            QObject *parent = nullptr);

    // Route handling
    void setOperationMode(OperationMode mode);
    QStringList getAvailableRoutes() const;
    bool selectRoute(const QString &routeName);
    QString getSelectedRouteName() const;

    bool loadRouteFile(const QString &filePath);
    void clearRoute();

    // Control
    void start();
    void stop();

signals:
    // Announce that a route has been chosen by the user.
    void routeSelected(const QString &routeName);

private slots:
    void process();   // periodic update (1 Hz)

private:
    // ===============================
    // Internal data structures
    // ===============================
    struct Landmark {
        int     index = -1;
        QString code;          // DEE, CURV, PM, etc.
        QString name;          // Full display string
        double  latitude = 0.0;
        double  longitude = 0.0;
    };

    struct NextLandmark {
        QString name;
        int distanceMeters = -1;
    };

    // ===============================
    // Core logic helpers
    // ===============================
    bool parseCsvLine(const QString &line, int index, Landmark &out);
    double distanceMeters(double lat1, double lon1,
                           double lat2, double lon2) const;

    int findClosestLandmarkIndex(double curLat, double curLon) const;
    void computeNextLandmarks(double curLat, double curLon);
    void triggerAlerts();

    // ===============================
    // Dependencies (injected)
    // ===============================
    Locator   *m_locator;
    CoreState *m_coreState;

    // ===============================
    // Runtime state
    // ===============================
    QTimer m_timer;
    static constexpr int PREWARN_DISTANCE_METERS = 500;

    QVector<Landmark> m_route;     // full route
    OperationMode m_operationMode = ModeIdle;
    bool m_routeSelected = false;
    QString m_selectedRouteName;
    int m_lastClosestIndex = -1;

    NextLandmark m_next[3];        // next 3 landmarks
};
