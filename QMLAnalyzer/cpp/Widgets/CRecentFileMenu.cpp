
// Qt
#include <QFileInfo>
#include <QDebug>

// Application
#include "CRecentFileMenu.h"

//-------------------------------------------------------------------------------------------------

QString CRecentFileMenu::sOrganizationName = "Jango73";
QString CRecentFileMenu::sProductName = "QMLAnalyzer";

// Recent file manager:
CRecentFileMenu::CRecentFileMenu(const QString &sMenuName,
    const QString &sSettingName, QWidget *parent)
    : QMenu(sMenuName, parent)
    , m_sSettingName(sSettingName)
    , m_sCurrentFile("")
    , m_pClearRecentFilesAction(nullptr)
{
    buildMenu();
}

//-------------------------------------------------------------------------------------------------

// Update menu:
void CRecentFileMenu::updateMenu(const QString &sFileName)
{
    // Set current file:
    m_sCurrentFile = sFileName;

    // Retrieve settings:
    QSettings settings(sOrganizationName, sProductName);

    // Retrieve file list:
    QStringList lFiles = settings.value(m_sSettingName).toStringList();
    lFiles.removeAll(sFileName);
    lFiles.prepend(sFileName);

    // Make sure we don't exceed MaxRecentFiles:
    while (lFiles.size() > MaxRecentFiles)
        lFiles.removeLast();

    // Update settings:
    settings.setValue(m_sSettingName, lFiles);

    // Update recent file actions:
    onUpdateRecentFileActions();

    emit currentFileChanged(m_sCurrentFile);
}

//-------------------------------------------------------------------------------------------------

// Update recent files action:
void CRecentFileMenu::onUpdateRecentFileActions()
{
    // Read settings:
    QSettings settings(sOrganizationName, sProductName);

    QStringList lFiles = settings.value(m_sSettingName).toStringList();

    // Make sure files still exist:
    QStringList lExistingFiles;

    for (auto sFile : lFiles)
    {
        QFileInfo fi(sFile);

        if (fi.exists())
            lExistingFiles << sFile;
    }

    int iNumRecentFiles = qMin(lExistingFiles.size(), (int)MaxRecentFiles);

    // Enable/Disable menu:
    setEnabled(iNumRecentFiles > 0);

    // Show clear recent files actions:
    m_pClearRecentFilesAction->setVisible(iNumRecentFiles > 0);

    for (int i = 0; i < iNumRecentFiles; ++i)
    {
        m_pRecentFileActions[i]->setText(lExistingFiles[i]);
        m_pRecentFileActions[i]->setData(lExistingFiles[i]);
        m_pRecentFileActions[i]->setVisible(true);
    }

    for (int j = iNumRecentFiles; j < MaxRecentFiles; ++j)
        m_pRecentFileActions[j]->setVisible(false);
}

//-------------------------------------------------------------------------------------------------

// Build menu:
void CRecentFileMenu::buildMenu()
{
    // Recent file menu:
    connect(this, &QMenu::aboutToShow, this, &CRecentFileMenu::onUpdateRecentFileActions);

    // Manage recent files:
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        m_pRecentFileActions[i] = new QAction(this);
        m_pRecentFileActions[i]->setVisible(false);
        addAction(m_pRecentFileActions[i]);
        connect(m_pRecentFileActions[i], &QAction::triggered, this, &CRecentFileMenu::onOpenRecentFile);
    }

    // Clear action:
    m_pClearRecentFilesAction = new QAction(tr("Clear List"), this);
    m_pClearRecentFilesAction->setVisible(false);
    addAction(m_pClearRecentFilesAction);
    connect(m_pClearRecentFilesAction, &QAction::triggered, this, &CRecentFileMenu::onClearRecentFiles);

    // Set initial state:
    onUpdateRecentFileActions();
}

//-------------------------------------------------------------------------------------------------

// Open recent file:
void CRecentFileMenu::onOpenRecentFile()
{
    // Retrieve sender:
    QAction *pSender = dynamic_cast<QAction *>(sender());

    if (pSender != nullptr)
        emit openRecentFile(pSender);
}

//-------------------------------------------------------------------------------------------------

// Clear recent files:
void CRecentFileMenu::onClearRecentFiles()
{
    QSettings settings(sOrganizationName, sProductName);
    settings.clear();
    onUpdateRecentFileActions();
}
