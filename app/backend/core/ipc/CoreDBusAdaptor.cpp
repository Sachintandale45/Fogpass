#include "CoreDBusAdaptor.h"
#include "CoreState.h"
#include <iostream>

CoreDBusAdaptor::CoreDBusAdaptor(CoreState *state, QObject *parent)
    : QObject(parent), m_state(state)
{
    // Here you would connect signals from CoreState to D-Bus signals
    // For example:
    // connect(m_state, &CoreState::fogModeChanged, this, &CoreDBusAdaptor::FogModeChanged);
}

void CoreDBusAdaptor::SetWeatherMode(bool foggy)
{
    std::cout << "[CORE] D-Bus call received: SetWeatherMode("
              << (foggy ? "true" : "false") << ")" << std::endl;

    m_state->setFogMode(foggy);
}