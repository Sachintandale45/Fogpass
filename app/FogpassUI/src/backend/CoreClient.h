#pragma once

#include <QObject>
#include <QDebug>

class CoreClient : public QObject
{
    Q_OBJECT
public:
    explicit CoreClient(QObject *parent = nullptr);
    void setMode(const QString &mode); // Method to send mode command to system backend
signals:
    void modeChanged(const QString &mode); // Signal emitted when system backend confirms mode change
};