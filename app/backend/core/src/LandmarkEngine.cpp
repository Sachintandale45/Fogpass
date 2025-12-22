#include "LandmarkEngine.h"
#include <QDebug>

LandmarkEngine::LandmarkEngine(QObject *parent) : QObject(parent)
{
    m_updateTimer.setInterval(1000); // Update every second
    connect(&m_updateTimer, &QTimer::timeout, this, &LandmarkEngine::onUpdateTimerTimeout);
}

void LandmarkEngine::start()
{
    qDebug() << "[ENGINE] Landmark engine started.";
    m_updateTimer.start();
}

void LandmarkEngine::onUpdateTimerTimeout()
{
    // Simulate a route with landmarks and their positions in meters
    static const QList<QPair<QString, int>> route = {
        {"Start Gate", 0},
        {"Checkpoint Alpha", 1500},
        {"Water Station", 3200},
        {"Hilltop View", 5000},
        {"Medical Tent", 7500},
        {"Final Turn", 9800},
        {"Finish Line", 10000}
    };

    // Simulate the vehicle moving at 25 m/s (90 km/h)
    m_routePosition += 25;
    if (m_routePosition > 10000) {
        m_routePosition = 0; // Loop back to the start
    }

    // Find the next 3 landmarks
    QStringList names;
    QList<int> distances;
    for (const auto &landmark : route) {
        if (landmark.second > m_routePosition) {
            names.append(landmark.first);
            distances.append(landmark.second - m_routePosition);
            if (names.count() == 3) break;
        }
    }
    while (names.count() < 3) { names.append("N/A"); distances.append(0); }

    emit nextLandmarksUpdated(names[0], distances[0], names[1], distances[1], names[2], distances[2]);
}