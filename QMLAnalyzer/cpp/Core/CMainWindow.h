
#pragma once

// Qt
#include <QMainWindow>
#include <QDir>
#include <QPlainTextEdit>
#include <QProcess>

#include "CDOMView.h"
#include "CCodeAnalyzerView.h"
#include "CFolderView.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class QLabel;
class CRecentFileMenu;
class CSpacerItem;
class CStatusWidget;

namespace Ui {
class MainWindow;
}

//-------------------------------------------------------------------------------------------------

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Enumerators
    //-------------------------------------------------------------------------------------------------

    // Tabs list
    enum EPrimaryTabs { MAIN_VIEW, CODE_ANALYZER_VIEW, DOM_VIEW };

    // Exit operation
    enum EOperation { NO_OPERATION, SAVED, IGNORED, CANCELLED };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    // Constructor
    CMainWindow(QApplication* pApplication, QStringList lArguments, QWidget* parent = 0);

    // Destructor
    virtual ~CMainWindow();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    // Handle output
    void handleOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    //-------------------------------------------------------------------------------------------------
    // Private control methods
    //-------------------------------------------------------------------------------------------------

private:

    // Process command line arguments
    void processCommandLineArguments();

    // Create actions
    void createActions();

    // Create recent files menu
    void createRecentFilesMenus();

    // Set up QML status
    void setupQMLStatus();

    //!
    void setTheme(const QString& sTheme);

    //-------------------------------------------------------------------------------------------------
    // Signals
    //-------------------------------------------------------------------------------------------------

signals:

    // Current file changed
    void currentFileChanged();

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

public slots:

    // Exit
    void onExit();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    // UI
    Ui::MainWindow* m_pUI;

    QApplication* m_pApplication;

    // Recent file menu
    CRecentFileMenu* m_pRecentFileMenu;

    // Recent workspace menu
    CRecentFileMenu* m_pRecentWorkspaceMenu;

    // Current file
    QString m_sCurrentFile;

    // Workspace name label
    QLabel* m_pWorkspaceNameLabel;

    // User status
    QLabel* m_pUserStatusLabel;

    // Arguments
    QStringList m_lArguments;

    // Log blacklist
    QStringList m_lLogBlacklist;

    CCodeAnalyzerView*          m_pCodeAnalyzerView;
    CDOMView*                   m_pDOMView;
    CFolderView*                m_pFolderView;
};
