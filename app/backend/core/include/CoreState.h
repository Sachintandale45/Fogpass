#pragma once

#include <QObject>
#include <QMutex>
#include <QSet>
#include <QString>

class CoreState : public QObject
{
    Q_OBJECT

public:
    explicit CoreState(QObject *parent = nullptr);

    /* ===================== Fog mode ===================== */
    void setFogMode(bool foggy);
    bool fogMode() const;

    /* ===================== Landmarks ===================== */
    void updateNextLandmarks(
        const QString &name1, int dist1,
        const QString &name2, int dist2,
        const QString &name3, int dist3
    );

    /* ===================== Alerts ===================== */
    void raiseAlert(const QString &alertId);
    void clearAlert(const QString &alertId);
    bool isAlertActive(const QString &alertId) const;

signals:
    /* Fog */
    void fogModeChanged(bool foggy);

    /* Landmarks */
    void nextLandmarksUpdated(
        QString name1, int dist1,
        QString name2, int dist2,
        QString name3, int dist3
    );

    /* Alerts */
    void alertRaised(const QString &alertId);
    void alertCleared(const QString &alertId);

private:
    mutable QMutex m_mutex;

    /* Fog */
    bool m_foggy = false;

    /* Landmarks */
    QString m_l1Name;
    int     m_l1Dist = -1;
    QString m_l2Name;
    int     m_l2Dist = -1;
    QString m_l3Name;
    int     m_l3Dist = -1;

    /* Alerts */
    QSet<QString> m_activeAlerts;
};
