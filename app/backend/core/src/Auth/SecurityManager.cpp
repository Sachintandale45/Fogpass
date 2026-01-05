#include "SecurityManager.h"

#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace {
    const QString CONFIG_FILE_PATH = "/etc/fogpass/security.conf";
}

SecurityManager::SecurityManager(QObject *parent)
    : QObject(parent)
{
    loadConfig();
}

void SecurityManager::loadConfig()
{
    QFile file(CONFIG_FILE_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[SecurityManager] Config file missing or unreadable:" << CONFIG_FILE_PATH;
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

bool SecurityManager::setPassword(const QString &capability, const QString &password)
{
    Q_UNUSED(capability); // Currently only "ADMIN" is supported

    QByteArray newHash = hashPassword(password);

    // Write the new hash to the configuration file
    QFile file(CONFIG_FILE_PATH);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qCritical() << "[SecurityManager] Failed to write to config file:" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << "admin_password_hash=" << newHash.toHex() << Qt::endl;
    
    m_adminPasswordHash = newHash;
    return true;
}

QByteArray SecurityManager::hashPassword(const QString &password) const
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
}