#pragma once

#include <QObject>
#include <QTimer>

class AudioClient;
class PowerClient;
class CoreClient;

class BackendManager : public QObject
{
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);

    // ===== UI -> Backend commands =====
    Q_INVOKABLE void setVolume(int volume);
    Q_INVOKABLE void setWeatherMode(bool foggy);

public slots:
    void start();

signals:
    // ===== Backend -> UI updates =====
    void volumeUpdated(int volume);
    void batteryLevelUpdated(int level);
    void modeUpdated(const QString &mode);

private slots:
    void onTimeout();

private:
    QTimer m_timer;
    int m_batteryLevel;
    AudioClient *m_audio;
    PowerClient *m_power;
    CoreClient *m_core;
};
