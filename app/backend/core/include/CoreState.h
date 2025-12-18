#pragma once

#include <QObject>
#include <QMutex>

class CoreState : public QObject
{
    Q_OBJECT

public:
    explicit CoreState(QObject *parent = nullptr);

    void setFogMode(bool foggy);
    bool fogMode() const;

signals:
    void fogModeChanged(bool foggy);

private:
    mutable QMutex m_mutex;
    bool m_foggy = false;
};