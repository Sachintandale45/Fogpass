#pragma once

#include <QObject>
#include <QStringList>
#include <QDBusArgument>
#include <QDBusMetaType>

class CoreState; // Forward declaration
class LandmarkEngine; // Forward declaration

class CoreDBusAdaptor : public QObject
{
    Q_OBJECT

    // Expose this class to D-Bus with the interface name com.fogpass.Core
    Q_CLASSINFO("D-Bus Interface", "com.fogpass.Core")

public:
    explicit CoreDBusAdaptor(CoreState *state, LandmarkEngine *engine, QObject *parent = nullptr);

signals:
    void LandmarkLocationsChanged(const QStringList &locations);
    void NextLandmarksUpdated(const QString &name1, int dist1, const QString &name2, int dist2, const QString &name3, int dist3);

public slots:
    // This slot will be callable over D-Bus as "SetWeatherMode"
    void SetWeatherMode(bool foggy);
    QStringList GetLandmarkLocations();
    void SetLandmarkLocations(const QStringList &locations);

private:
    CoreState *m_state;
};