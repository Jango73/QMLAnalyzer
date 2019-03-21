
// Qt
#include <QToolBar>
#include <QSplitter>
#include <QLabel>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCheckBox>
#include <QProcess>
#include <QDebug>
#include <QDesktopServices>
#include <QDateTime>

// Application
#include "ui_MainWindow.h"
#include "CMainWindow.h"
#include "CRecentFileMenu.h"
#include "CStatusWidget.h"

//-------------------------------------------------------------------------------------------------
// Constants

#define ARG_NAME_SOMETHING              "--something"       // Launches the app with panel view only

#define SPACER_ITEM_MIN_WIDTH           8

//-------------------------------------------------------------------------------------------------
// Reserved for error message output

static CMainWindow* s_pMainWindow = nullptr;

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (s_pMainWindow != nullptr)
    {
        s_pMainWindow->handleOutput(type, context, msg);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    \class CMainWindow
    \inmodule QMLAnalyzer
    \brief A main window for QMLAnalyzer.
*/

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CMainWindow. \br\br
    \a pApplication is the owning application.
    \a lArguments is a list of command line arguments.
    \a parent is the owner widget of this object.
*/
CMainWindow::CMainWindow(QApplication* pApplication, QStringList lArguments, QWidget *parent)
    : QMainWindow(parent)
    , m_pUI(new Ui::MainWindow)
    , m_pApplication(pApplication)
    , m_pRecentFileMenu(nullptr)
    , m_pRecentWorkspaceMenu(nullptr)
    , m_sCurrentFile("")
    , m_pUserStatusLabel(nullptr)
{
    s_pMainWindow = this;
    m_lArguments = lArguments;

    // Setup ui
    m_pUI->setupUi(this);

    // Create actions
    createActions();

    // Create recent files menu
    createRecentFilesMenus();

    // Setup QML status widget
    setupQMLStatus();

    // Add workspace label
    m_pWorkspaceNameLabel = new QLabel;
    m_pWorkspaceNameLabel->setObjectName("workspaceLabel");
    statusBar()->addPermanentWidget(m_pWorkspaceNameLabel);

    // Add user status
    m_pUserStatusLabel = new QLabel;
    m_pUserStatusLabel->setObjectName("userStatusLabel");
    statusBar()->insertWidget(0, m_pUserStatusLabel);

    m_lLogBlacklist << "ScrollView";
    m_lLogBlacklist << "TextArea";
    m_lLogBlacklist << "PropertyDelegate";
    m_lLogBlacklist << "QQuickItem::stackAfter";
    m_lLogBlacklist << "QQuickItem::stackBefore";
    m_lLogBlacklist << " of null";
    m_lLogBlacklist << "iCCP";
    m_lLogBlacklist << "TypeError";

    // Process command line arguments
    processCommandLineArguments();

    // Signals
    connect(m_pUI->analyzer, SIGNAL(requestDOMOpen(QString)), this, SLOT(onRequestDOMOpen(QString)));

    // Theme
    setTheme("Theme_Default");
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CMainWindow.
*/
CMainWindow::~CMainWindow()
{
    s_pMainWindow = nullptr;
    delete m_pUI;
}

//-------------------------------------------------------------------------------------------------

/*!
    Sets up the QML status control.
*/
void CMainWindow::setupQMLStatus()
{
    qInstallMessageHandler(myMessageHandler);
}

//-------------------------------------------------------------------------------------------------

/*!
    Handles an output message from Qt. \br\br
    \a type is the type of message. \br
    \a context provides contextual information. \br
    \a msg is the text to output.
*/
void CMainWindow::handleOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);

    foreach (QString sText, m_lLogBlacklist)
    {
        if (msg.contains(sText))
            return;
    }

    QString sType;

    switch (type)
    {
    case QtDebugMsg:
        sType = "Debug";
        break;
    case QtInfoMsg:
        sType = "Info";
        break;
    case QtWarningMsg:
        sType = "Warning";
        break;
    case QtCriticalMsg:
        sType = "Critical";
        break;
    case QtFatalMsg:
        sType = "Fatal";
        break;
    }

    QDateTime tTime = QDateTime::currentDateTime();
    QString sTime = QString("%1:%2:%3 %4:%5:%6")
            .arg(tTime.date().year())
            .arg(tTime.date().month())
            .arg(tTime.date().day())
            .arg(tTime.time().hour())
            .arg(tTime.time().minute())
            .arg(tTime.time().second())
            ;

    QString sFinalString = QString("%1 - %2 : %3").arg(sTime).arg(sType).arg(msg);

    if (sFinalString.endsWith("\n"))
    {
        sFinalString = sFinalString.mid(0, sFinalString.length() - 1);
    }

    m_pUI->QMLStatus->appendPlainText(sFinalString);
}

//-------------------------------------------------------------------------------------------------

/*!
    Processes command line arguments
*/
void CMainWindow::processCommandLineArguments()
{
//    if (m_lArguments.contains(ARG_NAME_WORKSPACE))
//    {
//        int index = m_lArguments.indexOf(ARG_NAME_WORKSPACE) + 1;

//        if (m_lArguments.count() > index)
//        {
//            QString sDirectoryName = m_lArguments[index];

//            doSetWorkspace(sDirectoryName);
//        }
//    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the action buttons in the main toolbar.
*/
void CMainWindow::createActions()
{
    connect(m_pUI->exitAction, &QAction::triggered, this, &CMainWindow::onExit);
}

//-------------------------------------------------------------------------------------------------

/*!
    Creates the recent files menu.
*/
void CMainWindow::createRecentFilesMenus()
{
    // Recent projects menu:
//    m_pRecentFileMenu = new CRecentFileMenu(tr("Recent Projects"), "Recent Projects", this);
//    QAction *pRecentFileMenuAction = m_pUI->menuFile->addMenu(m_pRecentFileMenu);
//    m_pUI->menuFile->insertAction(m_pUI->newProjectAction, pRecentFileMenuAction);
//    connect(m_pRecentFileMenu, &CRecentFileMenu::openRecentFile, this, &CMainWindow::onOpenRecentFile);
}

//-------------------------------------------------------------------------------------------------

/*!
    Exit.
*/
void CMainWindow::onExit()
{
    close();
}

//-------------------------------------------------------------------------------------------------

void CMainWindow::onRequestDOMOpen(QString sFileName)
{
    m_pUI->domView->openFile(sFileName);
    m_pUI->primaryTab->setCurrentIndex(1);
}

//-------------------------------------------------------------------------------------------------

void CMainWindow::setTheme(const QString& sTheme)
{
    if (m_pApplication != nullptr)
    {
        QString sStyle = QString(":/style/%1.css").arg(sTheme);

        // Set style sheet
        QFile file(sStyle);

        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream stream(&file);
            QString sStyle = stream.readAll();

            m_pApplication->setStyleSheet(sStyle);
            file.close();
        }
    }
}

//-------------------------------------------------------------------------------------------------
