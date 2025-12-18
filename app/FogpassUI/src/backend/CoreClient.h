#pragma once

#include <QObject>
#include <QDBusInterface> // New: For D-Bus communication

class CoreClient : public QObject
{
    Q_OBJECT
public:
    explicit CoreClient(QObject *parent = nullptr);

    // UI → Core command
    void setWeatherMode(bool foggy); // Renamed and changed to bool

private:
    QDBusInterface *m_iface; // New: D-Bus interface member
};