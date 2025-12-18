#pragma once

#include <QObject>

class CoreState; // Forward declaration

class CoreDBusAdaptor : public QObject
{
    Q_OBJECT

    // Expose this class to D-Bus with the interface name com.fogpass.Core
    Q_CLASSINFO("D-Bus Interface", "com.fogpass.Core")

public:
    explicit CoreDBusAdaptor(CoreState *state, QObject *parent = nullptr);

public slots:
    // This slot will be callable over D-Bus as "SetWeatherMode"
    void SetWeatherMode(bool foggy);

private:
    CoreState *m_state;
};