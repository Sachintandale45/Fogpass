#pragma once

#include <QObject>
#include <QTimer>

class BackendManager : public QObject
{
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);
public slots:
    void start();
signals:
    void batteryLevelUpdated(int level);
private slots:
    void onTimeout();
private:
    QTimer m_timer;
    int m_batteryLevel;
};
