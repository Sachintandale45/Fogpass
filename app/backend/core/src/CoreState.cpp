#include "CoreState.h"
#include <QMutexLocker>

CoreState::CoreState(QObject *parent) : QObject(parent)
{
}

void CoreState::setFogMode(bool foggy)
{
    QMutexLocker locker(&m_mutex);
    if (m_foggy != foggy) {
        m_foggy = foggy;
        locker.unlock(); // Unlock before emitting signal to avoid deadlocks if a slot tries to lock again
        emit fogModeChanged(foggy);
    }
}

bool CoreState::fogMode() const
{
    QMutexLocker locker(&m_mutex);
    return m_foggy;
}