
#pragma once

// Qt
#include <QObject>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include <QVector>

class CUtils : public QObject
{
    Q_OBJECT

public:
    // Constructor:
    CUtils(QObject *parent=0);

    // To local file:
    Q_INVOKABLE QString toLocalFile(const QString &input) const;

    // From local file:
    Q_INVOKABLE QString fromLocalFile(const QString &input) const;

    // Return application directory:
    Q_INVOKABLE QString appDir() const;

    // File exists?
    Q_INVOKABLE bool fileExists(const QString &sFilePath) const;

    // Return file base name + extension:
    Q_INVOKABLE QString baseName(const QString &sFullPath) const;

    // Return application directory as QDir:
    static QDir applicationDir();

    // Get files recursively based on image filters:
    static QVector<QString> files(const QDir &srcDir, const QStringList &imageFilters);

    static QString unixPath(const QString& sPath);
};
