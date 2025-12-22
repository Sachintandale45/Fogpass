#pragma once

#include <QObject>
#include <QTimer>
#include <QStringList>

class LandmarkEngine : public QObject
{
    Q_OBJECT
public:
    explicit LandmarkEngine(QObject *parent = nullptr);
    void start();

signals:
    void nextLandmarksUpdated(const QString &name1, int dist1, const QString &name2, int dist2, const QString &name3, int dist3);

private slots:
    void onUpdateTimerTimeout();

private:
    QTimer m_updateTimer;
    int m_routePosition = 0;
};