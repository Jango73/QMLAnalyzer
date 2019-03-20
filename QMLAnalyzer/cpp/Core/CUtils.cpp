
// Qt
#include <QDirIterator>

// Application
#include "CUtils.h"

//-------------------------------------------------------------------------------------------------

// Constructor:
CUtils::CUtils(QObject *parent) : QObject(parent)
{
}

//-------------------------------------------------------------------------------------------------

// To local file:
QString CUtils::toLocalFile(const QString &input) const
{
    QUrl url(input);
    return url.isLocalFile() ? url.toLocalFile() : input;
}

//-------------------------------------------------------------------------------------------------

// From local file:
QString CUtils::fromLocalFile(const QString &input) const
{
    QUrl url(input);
    return !url.isLocalFile() ? QUrl::fromLocalFile(input).toString() : input;
}

//-------------------------------------------------------------------------------------------------

// Return application directory:
QString CUtils::appDir() const
{
    return QCoreApplication::applicationDirPath();
}

//-------------------------------------------------------------------------------------------------

// Return application directory as QDir:
QDir CUtils::applicationDir()
{
    return QDir(QCoreApplication::applicationDirPath());
}

//-------------------------------------------------------------------------------------------------

// File exists?
bool CUtils::fileExists(const QString &sFilePath) const
{
    QString sLocalFile = toLocalFile(sFilePath);
    QFileInfo fi(sLocalFile);
    return fi.exists() && fi.isFile();
}

//-------------------------------------------------------------------------------------------------

// Get files (No RECURSION):
QVector<QString> CUtils::files(const QDir &srcDir, const QStringList &imageFilters)
{
    QVector<QString> vFiles;
    QDirIterator it(srcDir.absolutePath(), imageFilters, QDir::Files, QDirIterator::NoIteratorFlags);

    while (it.hasNext())
        vFiles << it.next();

    return vFiles;
}

//-------------------------------------------------------------------------------------------------

// Return file base name + extension:
QString CUtils::baseName(const QString &sFullPath) const
{
    QFileInfo fi(sFullPath);
    return fi.baseName() + "." + fi.suffix();
}

//-------------------------------------------------------------------------------------------------

QString CUtils::unixPath(const QString& sPath)
{
    QString sReturnValue(sPath);
    sReturnValue.replace("\\", "/");
    return sReturnValue;
}
