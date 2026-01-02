#pragma once

#include <QObject>
#include <QTimer>

class AudioClient;
class PowerClient;
class CoreClient;

class BackendManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString landmark1 READ landmark1 NOTIFY landmarksChanged)
    Q_PROPERTY(int distance1 READ distance1 NOTIFY landmarksChanged)
    Q_PROPERTY(QString landmark2 READ landmark2 NOTIFY landmarksChanged)
    Q_PROPERTY(int distance2 READ distance2 NOTIFY landmarksChanged)
    Q_PROPERTY(QString landmark3 READ landmark3 NOTIFY landmarksChanged)
    Q_PROPERTY(int distance3 READ distance3 NOTIFY landmarksChanged)
    Q_PROPERTY(int speed READ speed NOTIFY speedUpdated)
    Q_PROPERTY(QString operationModeLabel READ operationModeLabel NOTIFY operationModeChanged)
    Q_PROPERTY(bool isGnssStable READ isGnssStable NOTIFY gnssStabilityChanged)

public:
    explicit BackendManager(QObject *parent = nullptr);

    // ===== UI -> Backend commands =====
    Q_INVOKABLE void setVolume(int volume);
    Q_INVOKABLE void setWeatherMode(bool foggy);
    Q_INVOKABLE void requestLandmarks();
    Q_INVOKABLE void setGnssMode(bool simulation);
    Q_INVOKABLE void SetOperationMode(int mode);
    Q_INVOKABLE QStringList GetAvailableRoutes();
    Q_INVOKABLE void SelectRoute(const QString &routeName);
    Q_INVOKABLE void ClearRoute();

    QString landmark1() const;
    int distance1() const;
    QString landmark2() const;
    int distance2() const;
    QString landmark3() const;
    int distance3() const;
    int speed() const;
    QString operationModeLabel() const;
    bool isGnssStable() const;

public slots:
    void start();

signals:
    // ===== Backend -> UI updates =====
    void volumeUpdated(int volume);
    void batteryLevelUpdated(int level);
    void modeUpdated(const QString &mode);
    void landmarkLocationsUpdated(const QStringList &locations);
    void landmarksChanged();
    void speedUpdated(int speed);
    void operationModeChanged();
    void gnssStabilityChanged(bool stable);

private slots:
    void onTimeout();
    void onLandmarksUpdated(const QString &l1, int d1, const QString &l2, int d2, const QString &l3, int d3);
    void onSpeedUpdated(int speed);
    void onCoreGnssStabilityChanged(bool stable);

private:
    QTimer m_timer;
    int m_batteryLevel;
    AudioClient *m_audio;
    PowerClient *m_power;
    CoreClient *m_core;

    QString m_landmark1, m_landmark2, m_landmark3;
    int m_dist1 = 0;
    int m_dist2 = 0;
    int m_dist3 = 0;
    int m_speed = 0;
    int m_opMode = 0; // 0=Idle, 1=Manual, 2=Auto
    bool m_isGnssStable = false;
};
