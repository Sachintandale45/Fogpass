#include "CoreDBusAdaptor.h"
#include "CoreState.h"
#include <iostream>
#include <QDebug>

CoreDBusAdaptor::CoreDBusAdaptor(CoreState *state, QObject *parent)
    : QObject(parent), m_state(state)
{
    // Here you would connect signals from CoreState to D-Bus signals
    // For example:
    // connect(m_state, &CoreState::fogModeChanged, this, &CoreDBusAdaptor::FogModeChanged);
    connect(m_state, &CoreState::landmarkLocationsChanged, this, &CoreDBusAdaptor::LandmarkLocationsChanged);
}

void CoreDBusAdaptor::SetWeatherMode(bool foggy)
{
    std::cout << "[CORE] D-Bus call received: SetWeatherMode("
              << (foggy ? "true" : "false") << ")" << std::endl;

    m_state->setFogMode(foggy);
}

void CoreDBusAdaptor::SetLandmarkLocations(const QStringList &locations)
{
    qDebug() << "[CORE] D-Bus call received: SetLandmarkLocations";
    m_state->setLandmarkLocations(locations);
}

QDBusArgument &operator<<(QDBusArgument &argument, const QStringList &list)
{
    argument.beginArray();
    for (const QString &str : list) argument << str;
    argument.endArray();
    return argument;
}