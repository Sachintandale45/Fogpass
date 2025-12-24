#pragma once

#include <QObject>
#include <QDebug>

class AudioClient : public QObject
{
    Q_OBJECT
public:
    explicit AudioClient(QObject *parent = nullptr);
    void setVolume(int volume);
    void requestInitialVolume();
signals:
    void volumeChanged(int volume);
};