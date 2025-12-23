#pragma once

#include <QString>

class Landmark
{
public:
    Landmark();
    Landmark(const QString &name, double latitude, double longitude);

    QString name() const;
    double latitude() const;
    double longitude() const;
    bool isValid() const;

private:
    QString m_name;
    double m_latitude;
    double m_longitude;
};