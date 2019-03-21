
// Qt
#include <QDebug>
#include <QFileDialog>
#include <QMutexLocker>

// Application
#include "CCodeAnalyzerView.h"
#include "CUtils.h"
#include "ui_CCodeAnalyzerView.h"

//-------------------------------------------------------------------------------------------------

#define DIRTY_MARKER        " *"

//-------------------------------------------------------------------------------------------------

CErrorListModel::CErrorListModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

//-------------------------------------------------------------------------------------------------

CErrorListModel::~CErrorListModel()
{
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::setErrorList(const QVector<QMLAnalyzerError>& errors)
{
    beginResetModel();
    m_vErrors = errors;
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

QHash<int, QByteArray> CErrorListModel::roleNames() const
{
    QHash<int, QByteArray> hRoles;

    hRoles[File] = "File";
    hRoles[Line] = "Line";
    hRoles[Column] = "Column";
    hRoles[Text] = "Text";

    return hRoles;
}

//-------------------------------------------------------------------------------------------------

QVariant CErrorListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (orientation == Qt::Horizontal)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (section)
                {
                    case 0: return "File";
                    case 1: return "Line";
                    case 2: return "Col";
                    case 3: return "Message";
                }

                break;
            }
        }
    }
    else
    {
        return QVariant(section);
    }

    return QVariant();
}

//-------------------------------------------------------------------------------------------------

QVariant CErrorListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_vErrors.count())
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (index.column())
            {
                case 0:
                {
                    QFileInfo tInfo(m_vErrors[index.row()].fileName());
                    return tInfo.fileName();
                }

                case 1: return m_vErrors[index.row()].position().y() + 1;
                case 2: return m_vErrors[index.row()].position().x() + 1;
                case 3: return m_vErrors[index.row()].text();
            }

            break;
        }

        case File:
        {
            return m_vErrors[index.row()].fileName();
        }

        case Line:
        {
            return m_vErrors[index.row()].position().y() + 1;
        }

        case Column:
        {
            return m_vErrors[index.row()].position().x() + 1;
        }

        case Text:
        {
            return m_vErrors[index.row()].text();
        }
    }

    return QVariant();
}

//-------------------------------------------------------------------------------------------------

int CErrorListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return m_vErrors.count();
}

//-------------------------------------------------------------------------------------------------

int CErrorListModel::columnCount(const QModelIndex& parentQModelIndex) const
{
    Q_UNUSED(parentQModelIndex);

    return 4;
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::clear()
{
    beginResetModel();
    m_vErrors.clear();
    endResetModel();
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::addError(QMLAnalyzerError error)
{
    beginInsertRows(QModelIndex(), m_vErrors.count(), m_vErrors.count());
    insertRow(m_vErrors.count());
    m_vErrors << error;
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

QMap<QString, QVector<QMLAnalyzerError> > CErrorListModel::errorMap() const
{
    QMap<QString, QVector<QMLAnalyzerError> > mReturnValue;

    foreach (const QMLAnalyzerError& error, m_vErrors)
    {
        mReturnValue[error.text()] << error;
    }

    return mReturnValue;
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::lineAdded(QString sFileName, int position)
{
    for (int index = 0; index < m_vErrors.count(); index++)
    {
        if (m_vErrors[index].fileName() == sFileName)
        {
            if (m_vErrors[index].position().y() >= position)
            {
                QPoint pNewPoint(m_vErrors[index].position());
                pNewPoint.setY(pNewPoint.y() + 1);
                m_vErrors[index].setPosition(pNewPoint);
            }
        }
    }

    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, 0), roleNames().keys().toVector());
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::lineRemoved(QString sFileName, int position)
{
    for (int index = 0; index < m_vErrors.count(); index++)
    {
        if (m_vErrors[index].fileName() == sFileName)
        {
            if (m_vErrors[index].position().y() > position)
            {
                QPoint pNewPoint(m_vErrors[index].position());
                pNewPoint.setY(pNewPoint.y() - 1);
                m_vErrors[index].setPosition(pNewPoint);
            }
        }
    }

    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, 0), roleNames().keys().toVector());
}

//-------------------------------------------------------------------------------------------------

void CErrorListModel::revertPositions()
{
    for (int index = 0; index < m_vErrors.count(); index++)
    {
        m_vErrors[index].revertToOriginalPosition();
    }
}

//-------------------------------------------------------------------------------------------------

CCodeAnalyzerView::CCodeAnalyzerView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CCodeAnalyzerView)
    , m_pAnalyzer(new QMLAnalyzer())
    , m_mErrors(this)
    , m_pCompleter(new QCompleter(this))
{
    ui->setupUi(this);
    ui->ruleFileName->setText("CodingRules.xml");
    ui->formatFileName->setText("Format.xml");
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);

    m_pCompleter->setModel(new QDirModel(m_pCompleter));
    ui->directoryName->setCompleter(m_pCompleter);
    ui->ruleFileName->setCompleter(m_pCompleter);
    ui->formatFileName->setCompleter(m_pCompleter);

    m_tErrorsProxy.setSourceModel(&m_mErrors);
    ui->errors->setModel(&m_tErrorsProxy);

    connect(ui->browseDirectory, SIGNAL(clicked(bool)), this, SLOT(onBrowseClicked(bool)));
    connect(ui->browseRuleFile, SIGNAL(clicked(bool)), this, SLOT(onBrowseRuleFileClicked(bool)));
    connect(ui->browseRuleFile, SIGNAL(clicked(bool)), this, SLOT(onBrowseFormatFileClicked(bool)));
    connect(ui->runButton, SIGNAL(clicked(bool)), this, SLOT(onRunClicked(bool)));
    connect(ui->exportReport, SIGNAL(clicked(bool)), this, SLOT(onExportReportClicked(bool)));
    connect(ui->errors, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));
    connect(ui->errors, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onItemDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(newData(QMLAnalyzerError)), this, SLOT(onNewData(QMLAnalyzerError)));
    connect(ui->textEdit, SIGNAL(dirtyChanged(bool)), this, SLOT(onDirtyChanged(bool)));
    connect(ui->textEdit, SIGNAL(saveCurrentDocument()), this, SLOT(onSaveCurrentDocument()));
    connect(ui->textEdit, SIGNAL(lineAdded(int)), this, SLOT(onLineAdded(int)));
    connect(ui->textEdit, SIGNAL(lineRemoved(int)), this, SLOT(onLineRemoved(int)));

    connect(m_pAnalyzer, SIGNAL(finished()), this, SLOT(onAnalyzeFinished()));
    connect(m_pAnalyzer, SIGNAL(parsingStarted(QString)), this, SLOT(onParsingStarted(QString)));
    connect(m_pAnalyzer, SIGNAL(parsingFinished(QString)), this, SLOT(onParsingFinished(QString)));
    connect(m_pAnalyzer, SIGNAL(importParsingStarted(QString)), this, SLOT(onImportParsingStarted(QString)));
    connect(m_pAnalyzer, SIGNAL(analyzeError(QMLAnalyzerError)), this, SLOT(onAnalyzeError(QMLAnalyzerError)));
    connect(m_pAnalyzer, SIGNAL(finished()), this, SLOT(onAnalyzerFinished()));

    CDumpable::m_bJavaStyle = true;
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::resizeEvent(QResizeEvent* event)
{
    double w = ui->errors->width() - 10;
    double column0Width = w * 0.3;
    double column1Width = w * 0.05;
    double column2Width = w * 0.05;
    double column3Width = w * 0.6;

    ui->errors->setColumnWidth(0, column0Width);
    ui->errors->setColumnWidth(1, column1Width);
    ui->errors->setColumnWidth(2, column2Width);
    ui->errors->setColumnWidth(3, column3Width);

    QWidget::resizeEvent(event);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onBrowseClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sCurrentFolder = ui->directoryName->text();
    QString sDirectory = QFileDialog::getExistingDirectory(this, tr("Select directory"), sCurrentFolder, QFileDialog::ShowDirsOnly);

    if (sDirectory.isEmpty() == false)
    {
        ui->directoryName->setText(sDirectory);
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onBrowseRuleFileClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sRuleFileName = QFileDialog::getOpenFileName(this, tr("Select rule file"), "", "*.xml");

    if (sRuleFileName.isEmpty() == false)
    {
        ui->ruleFileName->setText(sRuleFileName);
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onBrowseFormatFileClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sFormatFileName = QFileDialog::getOpenFileName(this, tr("Select format file"), "", "*.xml");

    if (sFormatFileName.isEmpty() == false)
    {
        ui->formatFileName->setText(sFormatFileName);
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onRunClicked(bool bValue)
{
    Q_UNUSED(bValue);

    if (m_pAnalyzer->isRunning() == false)
    {
        ui->runButton->setText("Stop");

        m_mErrors.clear();

        QString sFolder = CUtils::unixPath(ui->directoryName->text());

        ui->directoryName->setText(sFolder);

        m_pAnalyzer->clear();
        m_pAnalyzer->setIncludeImports(false);
        m_pAnalyzer->setIncludeSubFolders(ui->includeSubFolders->checkState() == Qt::Checked);
        m_pAnalyzer->setRewriteFiles(ui->rewriteFiles->checkState() == Qt::Checked);
        m_pAnalyzer->setRemoveUnreferencedSymbols(ui->removeUnreferencedSymbols->checkState() == Qt::Checked);
        m_pAnalyzer->setFolder(sFolder);
        m_pAnalyzer->threadedAnalyze(
                    CXMLNode::load(ui->ruleFileName->text()),
                    CXMLNode::load(ui->formatFileName->text())
                    );
    }
    else
    {
        ui->runButton->setText("Run");

        m_pAnalyzer->stopThreadedAnalyze();
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onExportReportClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sReportFileName = QFileDialog::getSaveFileName(this, tr("Select report file"), "", "*.txt");

    if (sReportFileName.isEmpty() == false)
    {
        QFile fReport(sReportFileName);

        if (fReport.open(QFile::WriteOnly))
        {
            QMap<QString, QVector<QMLAnalyzerError> > mErrors = m_mErrors.errorMap();

            foreach (QString sKey, mErrors.keys())
            {
                fReport.write("\n" + sKey.toLatin1() + "\n");

                for (int index = 0; index < mErrors[sKey].count(); index++)
                {
                    QString sData1 = mErrors[sKey][index].fileName();
                    QString sData2 = QString::number(mErrors[sKey][index].position().y() + 1);
                    QString sData3 = QString::number(mErrors[sKey][index].position().x() + 1);

                    QString sText = QString("%1 (%2, %3)").arg(sData1).arg(sData2).arg(sData3);

                    fReport.write(sText.toLatin1() + "\n");
                }
            }

            fReport.close();
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onAnalyzeFinished()
{
    ui->runButton->setText("Run");
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onParsingStarted(QString sFileName)
{
    ui->statusText->setText(QString("Analyzing %1").arg(sFileName));
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onParsingFinished(QString sFileName)
{
    Q_UNUSED(sFileName);

    ui->statusText->setText("Done");
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onImportParsingStarted(QString sFileName)
{
    Q_UNUSED(sFileName);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onAnalyzeError(QMLAnalyzerError error)
{
    emit newData(error);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onAnalyzerFinished()
{
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onItemClicked(QModelIndex index)
{
    QModelIndex realIndex = m_tErrorsProxy.mapToSource(index);

    if (realIndex.isValid())
    {
        QString sFileName = m_mErrors.data(realIndex, CErrorListModel::File).toString();

        if (sFileName != m_sEditedFileName)
        {
            QFile fFile(sFileName);

            if (fFile.open(QFile::ReadOnly))
            {
                m_mErrors.revertPositions();

                m_sEditedFileName = sFileName;

                QString sText = fFile.readAll();
                fFile.close();

                ui->textEditTitle->setText(sFileName);
                ui->textEdit->reset();
                ui->textEdit->setPlainText(sText);
            }
        }

        int pX = m_mErrors.data(realIndex, CErrorListModel::Column).toInt() - 1;
        int pY = m_mErrors.data(realIndex, CErrorListModel::Line).toInt() - 1;

        QTextCursor tmpCursor = ui->textEdit->textCursor();
        tmpCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, pY);
        tmpCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, pX);
        tmpCursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor, 1);

        ui->textEdit->setTextCursor(tmpCursor);
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onItemDoubleClicked(QModelIndex index)
{
    QModelIndex realIndex = m_tErrorsProxy.mapToSource(index);

    if (realIndex.isValid())
    {
        QString sFileName = m_mErrors.data(realIndex, CErrorListModel::File).toString();

        if (sFileName.isEmpty() == false)
        {
            emit requestDOMOpen(sFileName);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onNewData(QMLAnalyzerError error)
{
    m_mErrors.addError(error);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onDirtyChanged(bool bDirty)
{
    QString sCurrentFileName = ui->textEditTitle->text();

    if (bDirty)
    {
        if (sCurrentFileName.endsWith(DIRTY_MARKER) == false)
        {
            sCurrentFileName += DIRTY_MARKER;
        }
    }
    else
    {
        sCurrentFileName.replace(DIRTY_MARKER, "");
    }

    ui->textEditTitle->setText(sCurrentFileName);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onSaveCurrentDocument()
{
    QTextDocument* pDocument = ui->textEdit->document();

    if (pDocument != nullptr)
    {
        QString sCurrentText = pDocument->toPlainText();
        QString sCurrentFileName = ui->textEditTitle->text();
        sCurrentFileName.replace(DIRTY_MARKER, "");

        QFile outputFile(sCurrentFileName);

        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream outStream(&outputFile);
            outStream << sCurrentText;
            outputFile.close();

            ui->textEdit->setDirty(false);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onLineAdded(int position)
{
    m_mErrors.lineAdded(m_sEditedFileName, position);
}

//-------------------------------------------------------------------------------------------------

void CCodeAnalyzerView::onLineRemoved(int position)
{
    m_mErrors.lineRemoved(m_sEditedFileName, position);
}
