
#pragma once

// Qt
#include <QFile>
#include <QDir>
#include <QVector>
#include <QMap>
#include <QString>
#include <QThread>

//-------------------------------------------------------------------------------------------------

class CImportProcessor : public QThread
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    CImportProcessor();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void processFiles(const QString& sGitBashExePath, const QString& sBaseDirectory, QMap<QString, QString> mMovedFiles);

    //!
    virtual void run() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    void checkReferencingFiles(const QString& sDirectory, const QString& sReferencedFile, const QString& sNewReferencedFile);

    //!
    void checkMovedFile(const QString& sMovedFileFrom, const QString& sMovedFileTo);

    //!
    void checkReferencingFile(const QString& sProcessedFileName, const QString& sReferencedFileFrom, const QString& sReferencedFileTo);

    //!
    void readImportStatements(const QString& sFileName, const QString &sReferencedFileFrom, bool bCheckClassUsage);

    //!
    bool processImportStatements();

    //!
    void addImportStatement(const QString& sRelativeDirectory);

    //!
    bool hasImport(const QString& sRelativeDirectory) const;

    //!
    bool isImportUsed(const QString& sRelativeDirectory) const;

    //-------------------------------------------------------------------------------------------------
    // Inner classes
    //-------------------------------------------------------------------------------------------------

protected:

    class CImportStatement
    {
    public:

        CImportStatement()
            : m_iPosition(-1)
            , m_iLength(0)
        {
        }

        QString     m_sImportStatement;
        QString     m_sImportRelativePath;
        int         m_iPosition;
        int         m_iLength;
    };

    class CClassUsage
    {
    public:

        CClassUsage()
            : m_bUsed(false)
        {
        }

        QString     m_sAbsolutePath;
        QString     m_sRelativePath;
        QString     m_sClassName;
        bool        m_bUsed;
    };

    //-------------------------------------------------------------------------------------------------
    // Signals
    //-------------------------------------------------------------------------------------------------

signals:

    //!
    void processingFile(const QString& sFileName);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString                     m_sGitBashExePath;
    QString                     m_sBaseDirectory;
    QMap<QString, QString>      m_mMovedFiles;
    QString                     m_sFileName;
    QString                     m_sQMLContent;
    QVector<CImportStatement>   m_vImportStatement;
    QVector<CClassUsage>        m_vClassUsage;
    int                         m_iLastImportPosition;
};
