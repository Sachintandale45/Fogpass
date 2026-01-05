#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief Manages password verification and access control.
 * 
 * Loads password hashes from /etc/fogpass/security.conf.
 * Does NOT interact with UI or D-Bus directly.
 */
class SecurityManager : public QObject
{
    Q_OBJECT
public:
    explicit SecurityManager(QObject *parent = nullptr);

    bool verifyPassword(const QString &capability, const QString &password);
    bool setPassword(const QString &capability, const QString &password);

private:
    /**
     * @brief Loads the security configuration from disk.
     */
    void loadConfig();

    QByteArray hashPassword(const QString &password) const;

    QByteArray m_adminPasswordHash;
};

#endif // SECURITYMANAGER_H