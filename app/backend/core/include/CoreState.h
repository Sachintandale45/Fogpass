#pragma once

#include <QObject>
#include <QMutex>
#include <QStringList>

class CoreState : public QObject
{
    Q_OBJECT

public:
    explicit CoreState(QObject *parent = nullptr);

    void setFogMode(bool foggy);
    bool fogMode() const;
    QStringList landmarkLocations() const;

public slots:
    void setLandmarkLocations(const QStringList &locations);

signals:
    void fogModeChanged(bool foggy);
    void landmarkLocationsChanged(const QStringList &locations);

private:
    mutable QMutex m_mutex;
    bool m_foggy = false;
    QStringList m_landmarkLocations;
};