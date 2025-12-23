#include "LandmarkEngine.h"
#include "Locator.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

LandmarkEngine::LandmarkEngine(Locator* locator, QObject* parent)
    : QObject(parent),
      m_locator(locator)
{
    Q_ASSERT(m_locator != nullptr);
}

bool LandmarkEngine::loadLandmarkFile(const QString& filePath)
{
    qDebug() << "[Landmark] Loading landmark file:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Landmark] Failed to open file";
        return false;
    }

    QTextStream in(&file);

    // Placeholder: real CSV parsing later
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty())
            continue;

        // TODO: parse CSV properly
        // name,lat,lon
    }

    file.close();
    return true;
}

void LandmarkEngine::update()
{
    // Called periodically (timer or thread loop)
    computeDistances();
}

void LandmarkEngine::computeDistances()
{
    // Placeholder logic
    m_nextLandmarks.clear();

    auto pos = m_locator->position();

    Q_UNUSED(pos);

    // TODO:
    // - compute distance to each landmark
    // - sort by distance
    // - keep next 3
}

double LandmarkEngine::computeDistanceMeters(double lat1, double lon1,
                                             double lat2, double lon2)
{
    Q_UNUSED(lat1);
    Q_UNUSED(lon1);
    Q_UNUSED(lat2);
    Q_UNUSED(lon2);

    // TODO: Haversine formula
    return 0.0;
}

QVector<LandmarkEngine::LandmarkStatus>
LandmarkEngine::nextLandmarks(int count) const
{
    if (count >= m_nextLandmarks.size())
        return m_nextLandmarks;

    return m_nextLandmarks.mid(0, count);
}
