#include "CoreDBusAdaptor.h"
#include "CoreState.h"
#include <iostream>
#include <QDebug>
#include "LandmarkEngine.h"

CoreDBusAdaptor::CoreDBusAdaptor(CoreState *state, LandmarkEngine *engine, QObject *parent)
    : QObject(parent), m_state(state)
{
    // Connect signals from the application's state/logic objects to this adaptor's D-Bus signals.
    connect(m_state, &CoreState::landmarkLocationsChanged, this, &CoreDBusAdaptor::LandmarkLocationsChanged);
    connect(engine, &LandmarkEngine::nextLandmarksUpdated, this, &CoreDBusAdaptor::NextLandmarksUpdated);
}

void CoreDBusAdaptor::SetWeatherMode(bool foggy)
{
    std::cout << "[CORE] D-Bus call received: SetWeatherMode("
              << (foggy ? "true" : "false") << ")" << std::endl;

    m_state->setFogMode(foggy);
}

QStringList CoreDBusAdaptor::GetLandmarkLocations()
{
    qDebug() << "[CORE] D-Bus call received: GetLandmarkLocations";
    return m_state->landmarkLocations();
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