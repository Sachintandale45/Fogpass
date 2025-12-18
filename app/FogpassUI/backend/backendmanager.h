#pragma once

#include <QObject>
#include <QTimer>

class BackendManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
public:
    explicit BackendManager(QObject *parent = nullptr);
    int batteryLevel() const;
public slots:
    void start();
signals:
    void batteryLevelChanged();
private slots:
    void onTimeout();
private:
    QTimer m_timer;
    int m_batteryLevel;
};
