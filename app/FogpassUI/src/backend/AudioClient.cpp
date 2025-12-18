#include "AudioClient.h"

AudioClient::AudioClient(QObject *parent) : QObject(parent)
{
    qDebug() << "AudioClient: Initialized.";
}

void AudioClient::setVolume(int volume)
{
    qDebug() << "AudioClient: Simulating D-Bus call to set volume to" << volume;
    // In a real app, this makes a D-Bus call.
    // The system service would then emit a signal on success.
}

void AudioClient::requestInitialVolume()
{
    qDebug() << "AudioClient: Requesting initial volume from system service.";
    // In a real app, this makes a D-Bus call to get current state.
}