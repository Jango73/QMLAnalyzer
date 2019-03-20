
#pragma once

// Qt
#include <QObject>
#include <QSettings>
#include <QMenu>

//-------------------------------------------------------------------------------------------------
// Constants

#define APPLICATION_TITLE   "Panel Designer"

//-------------------------------------------------------------------------------------------------

class CRecentFileMenu : public QMenu
{
    Q_OBJECT

public:
    enum {MaxRecentFiles = 5};

    // Constructor:
    CRecentFileMenu(const QString &sMenuName, const QString &sSettingName, QWidget *parent=0);

    // Update menu:
    void updateMenu(const QString &sFileName);

private:
    // Build menu:
    void buildMenu();

private:
    // Setting name:
    QString m_sSettingName;

    // Current file:
    QString m_sCurrentFile;

    // Recent file actions:
    QAction *m_pRecentFileActions[MaxRecentFiles];

    // Clear recent files action:
    QAction *m_pClearRecentFilesAction;

    // Organization name:
    static QString sOrganizationName;

    // Product name:
    static QString sProductName;

public slots:
    // Update recent files action:
    void onUpdateRecentFileActions();

    // Open recent file:
    void onOpenRecentFile();

    // Clear recent files:
    void onClearRecentFiles();

signals:
    // Current file changed:
    void currentFileChanged(const QString &sCurrentFile);

    // Open recent file:
    void openRecentFile(QAction *pAction);
};
