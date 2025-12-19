#pragma once

#include <QObject>
#include <QStringList>
#include <QDBusArgument>
#include <QDBusMetaType>

class CoreState; // Forward declaration

class CoreDBusAdaptor : public QObject
{
    Q_OBJECT

    // Expose this class to D-Bus with the interface name com.fogpass.Core
    Q_CLASSINFO("D-Bus Interface", "com.fogpass.Core")

public:
    explicit CoreDBusAdaptor(CoreState *state, QObject *parent = nullptr);

signals:
    void LandmarkLocationsChanged(const QStringList &locations);

public slots:
    // This slot will be callable over D-Bus as "SetWeatherMode"
    void SetWeatherMode(bool foggy);
    void SetLandmarkLocations(const QStringList &locations);

private:
    CoreState *m_state;
};