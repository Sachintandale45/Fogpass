#include "SecurityManager.h"

#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QDebug>

SecurityManager::SecurityManager(QObject *parent)
    : QObject(parent)
{
    loadConfig();
}

void SecurityManager::loadConfig()
{
    QFile file("/etc/fogpass/security.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[SecurityManager] Config file missing or unreadable: /etc/fogpass/security.conf";
        qWarning() << "[SecurityManager] Access will be DENIED by default.";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        // Expected format: admin_password_hash=<hex_sha256_hash>
        if (line.startsWith("admin_password_hash=")) {
            QString hashHex = line.section('=', 1).trimmed();
            m_adminPasswordHash = QByteArray::fromHex(hashHex.toLatin1());
        }
    }

    if (m_adminPasswordHash.isEmpty()) {
        qWarning() << "[SecurityManager] No admin_password_hash found in config.";
    } else {
        // Log success but NEVER the hash
        qInfo() << "[SecurityManager] Security configuration loaded successfully.";
    }
}

bool SecurityManager::verifyPassword(const QString &capability, const QString &password)
{
    // Future-proof: Support multiple roles/capabilities here
    Q_UNUSED(capability);

    if (m_adminPasswordHash.isEmpty()) {
        qWarning() << "[SecurityManager] Access denied: System not configured.";
        return false;
    }

    QByteArray inputHash = hashPassword(password);
    return (inputHash == m_adminPasswordHash);
}

QByteArray SecurityManager::hashPassword(const QString &password) const
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
}