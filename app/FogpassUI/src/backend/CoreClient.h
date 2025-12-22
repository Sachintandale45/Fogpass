#pragma once

#include <QObject>
#include <QDBusInterface>
#include <QStringList>
#include <QDebug>

class CoreClient : public QObject
{
    Q_OBJECT
public:
    explicit CoreClient(QObject *parent = nullptr);

    void setWeatherMode(bool foggy);
    void requestLandmarkLocations();

signals:
    void landmarkLocationsChanged(const QStringList &locations);
    void landmarksUpdated(const QString &l1, int d1, const QString &l2, int d2, const QString &l3, int d3);

private:
    QDBusInterface *m_iface;
};