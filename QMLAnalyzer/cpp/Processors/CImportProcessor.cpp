
// Qt
#include <QDebug>
#include <QProcess>

// Application
#include "CImportProcessor.h"
#include "CUtils.h"

//-------------------------------------------------------------------------------------------------

// #define DEBUG_OUTPUT(a)     qDebug() << a
#define DEBUG_OUTPUT(a)

//-------------------------------------------------------------------------------------------------

CImportProcessor::CImportProcessor()
    : m_iLastImportPosition(0)
{
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::processFiles(const QString& sGitBashExePath, const QString& sBaseDirectory, QMap<QString, QString> mMovedFiles)
{
    if (isRunning() == false)
    {
        m_sGitBashExePath = sGitBashExePath;
        m_sBaseDirectory = sBaseDirectory;
        m_mMovedFiles = mMovedFiles;

        start();
    }
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::run()
{
    // Iterate through each file to move
    foreach (QString sKey, m_mMovedFiles.keys())
    {
        // Get the source and target file names
        QString sSourceFile = sKey;
        QString sTargetFile = m_mMovedFiles[sKey];

        // Tell the world we are processing a file
        emit processingFile(sSourceFile);

        checkMovedFile(sSourceFile, sTargetFile);
        checkReferencingFiles(m_sBaseDirectory, sSourceFile, sTargetFile);

        // Move the file
        if (m_sGitBashExePath.isEmpty())
        {
            if (QFile::copy(sSourceFile, sTargetFile))
            {
                QFile::remove(sSourceFile);
            }
        }
        else
        {
            QDir dMovedFileFrom(m_sBaseDirectory);

            QString sRelativeSourceFile = dMovedFileFrom.relativeFilePath(sSourceFile);
            QString sRelativeTargetFile = dMovedFileFrom.relativeFilePath(sTargetFile);

            QString sCommand = QString("\"%1\" --cd=\"%2\" -c \"git mv %3 %4\"")
                    .arg(m_sGitBashExePath)
                    .arg(m_sBaseDirectory)
                    .arg(sRelativeSourceFile)
                    .arg(sRelativeTargetFile);

            DEBUG_OUTPUT("Launching : " << sCommand);

            QProcess process;
            process.start(sCommand);
            process.waitForFinished();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::checkMovedFile(
        const QString& sMovedFileFrom,
        const QString& sMovedFileTo
        )
{
    // Read the QML content of the 'from' file, and the import statements
    readImportStatements(sMovedFileFrom, "", false);

    // Mark the file as 'not modified'
    bool bChanged = false;

    foreach (CImportStatement statement, m_vImportStatement)
    {
        // Mark the file as 'modified'
        bChanged = true;

        // Get the absolute directories to the 'from' and 'to' files
        QFileInfo iMovedFileFrom(sMovedFileFrom);
        QFileInfo iMovedFileTo(sMovedFileTo);

        QDir dMovedFileFrom = iMovedFileFrom.absoluteDir();
        QDir dMovedFileTo = iMovedFileTo.absoluteDir();

        // Get the absolute path to the file in the import statement
        QString sImportPath = dMovedFileFrom.canonicalPath() + "/" + statement.m_sImportRelativePath;

        // Get the relative path to the file in the import statement, with regard to the 'to' file
        QString sRelativeDirectoryTo = dMovedFileTo.relativeFilePath(sImportPath);

        // Replace the import statement in the QML content
        QString sReplace = QString("import \"%1\"").arg(sRelativeDirectoryTo);
        m_sQMLContent.replace(statement.m_iPosition, statement.m_iLength, sReplace);
    }

    // Save the QML content if modified
    if (bChanged)
    {
        QFile fFile(sMovedFileFrom);
        if (fFile.open(QFile::WriteOnly))
        {
            fFile.write(m_sQMLContent.toLatin1());
            fFile.close();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::checkReferencingFiles(
        const QString& sDirectory,
        const QString& sReferencedFileFrom,
        const QString& sReferencedFileTo
        )
{
    DEBUG_OUTPUT("---------------------------------------------------------------------------");
    DEBUG_OUTPUT("---------------------------------------------------------------------------");
    DEBUG_OUTPUT("checkReferencingFiles() : " << sDirectory << sReferencedFileFrom << sReferencedFileTo);

    // We filter QML files
    QStringList slNameFilter;
    slNameFilter << "*.qml";

    // Get a list of QML files in sDirectory
    QDir dDirectory(sDirectory);

    dDirectory.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files);
    QStringList lFiles = dDirectory.entryList(slNameFilter);

    // Iterate through listed files
    foreach (QString sFile, lFiles)
    {
        // Get the full path to the current file
        QString sFullName = CUtils::unixPath(QString("%1/%2").arg(sDirectory).arg(sFile));

        // If this is not the file we are moving, process it
        if (sFullName != sReferencedFileFrom)
        {
            checkReferencingFile(sFullName, sReferencedFileFrom, sReferencedFileTo);
        }
    }

    // Get all sub-directories in sDirectory
    dDirectory.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList lDirectories = dDirectory.entryList();

    // Iterate through listed directories
    foreach (QString sNewDirectory, lDirectories)
    {
        QString sFullName = CUtils::unixPath(QString("%1/%2").arg(sDirectory).arg(sNewDirectory));

        // Call this method recursively
        checkReferencingFiles(sFullName, sReferencedFileFrom, sReferencedFileTo);
    }
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::checkReferencingFile(
        const QString& sProcessedFileName,
        const QString& sReferencedFileFrom,
        const QString& sReferencedFileTo
        )
{
    DEBUG_OUTPUT("---------------------------------------------------------------------------");
    DEBUG_OUTPUT("checkReferencingFile() : " << sProcessedFileName << sReferencedFileFrom << sReferencedFileTo);

    readImportStatements(sProcessedFileName, sReferencedFileFrom, true);

    QFileInfo tProcessedFileInfo(sProcessedFileName);
    QDir dProcessedDirectory = tProcessedFileInfo.absoluteDir();
    QString sRelativeFilePathFrom = dProcessedDirectory.relativeFilePath(sReferencedFileFrom);
    QString sRelativeDirectoryFrom = sRelativeFilePathFrom.mid(0, sRelativeFilePathFrom.lastIndexOf("/"));
    QString sRelativeFilePathTo = dProcessedDirectory.relativeFilePath(sReferencedFileTo);
    QString sRelativeDirectoryTo = sRelativeFilePathTo.mid(0, sRelativeFilePathTo.lastIndexOf("/"));

    DEBUG_OUTPUT("sRelativeFilePathFrom : " << sRelativeFilePathFrom);
    DEBUG_OUTPUT("sRelativeDirectoryFrom : " << sRelativeDirectoryFrom);
    DEBUG_OUTPUT("sRelativeFilePathTo : " << sRelativeFilePathTo);
    DEBUG_OUTPUT("sRelativeDirectoryTo : " << sRelativeDirectoryTo);

    if (hasImport(sRelativeDirectoryFrom))
    {
        if (sRelativeDirectoryTo.contains("/") == false && sRelativeDirectoryTo.contains("..") == false)
        {
            sRelativeDirectoryTo.clear();
        }

        if (sRelativeDirectoryTo.isEmpty())
        {
            sRelativeDirectoryTo = ".";
        }

        addImportStatement(sRelativeDirectoryTo);
    }

    bool bChanged = processImportStatements();

    if (bChanged)
    {
        QFile fFile(sProcessedFileName);
        if (fFile.open(QFile::WriteOnly))
        {
            fFile.write(m_sQMLContent.toLatin1());
            fFile.close();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::readImportStatements(const QString& sFileName, const QString& sReferencedFileFrom, bool bCheckClassUsage)
{
    DEBUG_OUTPUT("readImportStatements() : " << sFileName << sReferencedFileFrom << bCheckClassUsage);

    m_iLastImportPosition = 0;
    m_sQMLContent.clear();
    m_vImportStatement.clear();
    m_vClassUsage.clear();

    m_sFileName = sFileName;

    QFileInfo tFileInfo(sFileName);
    QString sDirectory = tFileInfo.canonicalPath();

    QFile fFile(sFileName);
    if (fFile.open(QFile::ReadOnly))
    {
        m_sQMLContent = fFile.readAll();
        fFile.close();

        // Get import statements
        QRegExp sImportRegExp("import\\s+\"([0-9a-zA-Z\\.\\/]+)\"");

        int iIndexIn = 0;
        int iLengthIn = 0;

        while (iIndexIn >= 0)
        {
            iIndexIn = sImportRegExp.indexIn(m_sQMLContent, iIndexIn);
            iLengthIn = sImportRegExp.matchedLength();

            if (iIndexIn < 0)
                break;

            CImportStatement statement;

            statement.m_sImportStatement = sImportRegExp.cap(0);
            statement.m_sImportRelativePath = sImportRegExp.cap(1);
            statement.m_iPosition = iIndexIn;
            statement.m_iLength = iLengthIn;

            m_iLastImportPosition = statement.m_iPosition + statement.m_iLength;

            while (true)
            {
                if (m_iLastImportPosition >= m_sQMLContent.count())
                    break;

                if (m_sQMLContent[m_iLastImportPosition] == '\n')
                {
                    m_iLastImportPosition++;
                    break;
                }

                m_iLastImportPosition++;
            }

            DEBUG_OUTPUT("Statement: " << statement.m_sImportStatement << statement.m_sImportRelativePath << statement.m_iPosition);

            m_vImportStatement << statement;

            iIndexIn++;
        }

        // Get class usage
        if (bCheckClassUsage)
        {
            foreach (CImportStatement statement, m_vImportStatement)
            {
                // List all files in import directory
                QDir dImportDirectory(sDirectory + "/" + statement.m_sImportRelativePath);
                QString sImportDirectory = dImportDirectory.canonicalPath();

                QStringList slNameFilter;
                slNameFilter << "*.qml";

                dImportDirectory.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files);
                QStringList lFiles = dImportDirectory.entryList(slNameFilter);

                foreach (QString sFile, lFiles)
                {
                    QFileInfo info(sImportDirectory + "/" + sFile);

                    CClassUsage usage;

                    usage.m_sAbsolutePath = sImportDirectory;
                    usage.m_sRelativePath = statement.m_sImportRelativePath;
                    usage.m_sClassName = info.baseName();
                    usage.m_bUsed = false;

                    // Check if class is used
                    if (info.canonicalFilePath() != sReferencedFileFrom)
                    {
                        if (QRegExp(QString("%1\\s*[{]").arg(usage.m_sClassName)).indexIn(m_sQMLContent) >= 0)
                        {
                            usage.m_bUsed = true;
                            DEBUG_OUTPUT(usage.m_sClassName << "is used");
                        }
                        else
                        {
                            if (QRegExp(QString("%1[.]").arg(usage.m_sClassName)).indexIn(m_sQMLContent) >= 0)
                            {
                                usage.m_bUsed = true;
                                DEBUG_OUTPUT(usage.m_sClassName << "is used");
                            }
                        }
                    }

                    m_vClassUsage << usage;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

bool CImportProcessor::processImportStatements()
{
    bool bChanged = false;

    // Remove unused import statements
    foreach (CImportStatement statement, m_vImportStatement)
    {
        if (statement.m_iPosition != -1)
        {
            if (isImportUsed(statement.m_sImportRelativePath) == false)
            {
                QString sReplace = QString().leftJustified(statement.m_sImportStatement.count(), ' ');
                m_sQMLContent.replace(statement.m_iPosition, statement.m_iLength, sReplace);
                bChanged = true;
            }
        }
    }

    // Insert non existent statements
    foreach (CImportStatement statement, m_vImportStatement)
    {
        if (statement.m_iPosition == -1)
        {
            m_sQMLContent.insert(m_iLastImportPosition, statement.m_sImportStatement + "\r\n");
            bChanged = true;
        }
    }

    m_vImportStatement.clear();

    return bChanged;
}

//-------------------------------------------------------------------------------------------------

void CImportProcessor::addImportStatement(const QString& sRelativeDirectory)
{
    DEBUG_OUTPUT("addImportStatement() : " << sRelativeDirectory);

    if (hasImport(sRelativeDirectory) == false)
    {
        CImportStatement statement;
        statement.m_sImportRelativePath = sRelativeDirectory;
        statement.m_sImportStatement = QString("import \"%1\"").arg(statement.m_sImportRelativePath);

        m_vImportStatement << statement;
    }
}

//-------------------------------------------------------------------------------------------------

bool CImportProcessor::hasImport(const QString& sRelativeDirectory) const
{
    foreach (const CImportStatement& statement, m_vImportStatement)
    {
        if (statement.m_sImportRelativePath == sRelativeDirectory)
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

bool CImportProcessor::isImportUsed(const QString& sRelativeDirectory) const
{
    foreach (const CClassUsage& usage, m_vClassUsage)
    {
        if (usage.m_sRelativePath == sRelativeDirectory && usage.m_bUsed)
        {
            return true;
        }
    }

    return false;
}
