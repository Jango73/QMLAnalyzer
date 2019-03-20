
// Qt
#include <QDebug>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>

// Application
#include "CFolderView.h"
#include "CUtils.h"
#include "ui_CFolderView.h"

//-------------------------------------------------------------------------------------------------

CFileSystemModel::CFileSystemModel(CFolderView* view)
    : m_pView(view)
{
    connect(&m_tProcessor, SIGNAL(processingFile(const QString&)), this, SLOT(onProcessingFile(const QString&)));
    connect(&m_tProcessor, SIGNAL(finished()), this, SLOT(onFinished()));
}

//-------------------------------------------------------------------------------------------------

Qt::ItemFlags CFileSystemModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index);

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

//-------------------------------------------------------------------------------------------------

bool CFileSystemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if (action == Qt::MoveAction)
    {
        // Get the list of files to move
        QStringList sFiles = data->text().split("\n", QString::SkipEmptyParts);

        QMap<QString, QString> mMovedFiles;

        // Iterate through each file to move
        foreach(QString sFile, sFiles)
        {
            // Get source and target file names
            QString sSourceFile = CUtils::unixPath(QUrl(sFile).toLocalFile());
            QString sTargetFile = CUtils::unixPath(filePath(parent));

            // Get the target directory
            QDir tDirectory1(sTargetFile);

            if (tDirectory1.exists() == false)
            {
                sTargetFile = CUtils::unixPath(filePath(parent.parent()));
            }

            QDir tDirectory2(sTargetFile);

            if (tDirectory2.exists())
            {
                QFileInfo fSourceFile(sSourceFile);

                // Compose target file name
                sTargetFile = sTargetFile + "/" + fSourceFile.fileName();

                // Add file names to map
                mMovedFiles[sSourceFile] = sTargetFile;
            }
        }

        // Call the import processor
        m_tProcessor.processFiles(m_pView->gitBashExecutable(), m_pView->baseDirectory(), mMovedFiles);

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------

void CFileSystemModel::onProcessingFile(const QString& sFileName)
{
    QString sText = QString("Processing %1...").arg(sFileName);
    m_pView->log(sText);
}

//-------------------------------------------------------------------------------------------------

void CFileSystemModel::onFinished()
{
    m_pView->log("Done");
}

//-------------------------------------------------------------------------------------------------

CFolderView::CFolderView(QWidget* parent)
    : QWidget(parent)
    , m_pUI(new Ui::CFolderView)
    , m_pLeftModel(new CFileSystemModel(this))
    , m_pRightModel(new CFileSystemModel(this))
{
    m_pUI->setupUi(this);

    connect(m_pUI->browseButton, SIGNAL(clicked(bool)), this, SLOT(onBrowseClicked(bool)));
    connect(m_pUI->baseDirectoryName, SIGNAL(textChanged(QString)), this, SLOT(onBaseDirectoryChanged(QString)));

    m_pLeftModel->setRootPath("");
    m_pRightModel->setRootPath("");

    QString sPath = QCoreApplication::applicationDirPath();

    m_pUI->baseDirectoryName->setText(sPath);
    m_pUI->leftTree->setModel(m_pLeftModel);
    m_pUI->rightTree->setModel(m_pRightModel);

    setLeftFolder(sPath);
    setRightFolder(sPath);
}

//-------------------------------------------------------------------------------------------------

QString CFolderView::baseDirectory() const
{
    return CUtils::unixPath(m_pUI->baseDirectoryName->text());
}

//-------------------------------------------------------------------------------------------------

QString CFolderView::gitBashExecutable() const
{
    // return m_pUI->useGitMove->isChecked() ? m_pController->gitPath() + "/" + m_pController->gitBashExe() : "";
    return "";
}

//-------------------------------------------------------------------------------------------------

void CFolderView::setLeftFolder(const QString& sFolder)
{
    const QModelIndex index = m_pLeftModel->index(QDir::cleanPath(sFolder));

    if (index.isValid())
    {
        m_pUI->leftTree->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

//-------------------------------------------------------------------------------------------------

void CFolderView::setRightFolder(const QString& sFolder)
{
    const QModelIndex index = m_pRightModel->index(QDir::cleanPath(sFolder));

    if (index.isValid())
    {
        m_pUI->rightTree->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

//-------------------------------------------------------------------------------------------------

void CFolderView::log(const QString& sText)
{
    m_pUI->output->appendPlainText(sText);
}

//-------------------------------------------------------------------------------------------------

void CFolderView::onBrowseClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sCurrentFolder = m_pUI->baseDirectoryName->text();
    QString sDirectory = QFileDialog::getExistingDirectory(this, tr("Select directory"), sCurrentFolder, QFileDialog::ShowDirsOnly);

    if (sDirectory.isEmpty() == false)
    {
        m_pUI->baseDirectoryName->setText(sDirectory);
    }
}

//-------------------------------------------------------------------------------------------------

void CFolderView::onBaseDirectoryChanged(QString sText)
{
    setLeftFolder(sText);
    setRightFolder(sText);
}

//-------------------------------------------------------------------------------------------------

void CFolderView::resizeEvent(QResizeEvent* event)
{
    double w, column0Width, column1Width, column2Width, column3Width;

    w = m_pUI->leftTree->width() - 10;
    column0Width = w * 0.5;
    column1Width = w * 0.1666;
    column2Width = w * 0.1666;
    column3Width = w * 0.1666;

    m_pUI->leftTree->setColumnWidth(0, column0Width);
    m_pUI->leftTree->setColumnWidth(1, column1Width);
    m_pUI->leftTree->setColumnWidth(2, column2Width);
    m_pUI->leftTree->setColumnWidth(3, column3Width);

    w = m_pUI->rightTree->width() - 10;
    column0Width = w * 0.5;
    column1Width = w * 0.1666;
    column2Width = w * 0.1666;
    column3Width = w * 0.1666;

    m_pUI->rightTree->setColumnWidth(0, column0Width);
    m_pUI->rightTree->setColumnWidth(1, column1Width);
    m_pUI->rightTree->setColumnWidth(2, column2Width);
    m_pUI->rightTree->setColumnWidth(3, column3Width);

    QWidget::resizeEvent(event);
}
