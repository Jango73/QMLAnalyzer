
// Qt
#include <QDebug>
#include <QFileDialog>

// Foundations
#include "QMLTree/QMLAnalyzer.h"

// Application
#include "CDOMView.h"
#include "ui_CDOMView.h"

//-------------------------------------------------------------------------------------------------

CDOMView::CDOMView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CDOMView)
{
    ui->setupUi(this);

    connect(ui->browseFile, SIGNAL(clicked(bool)), this, SLOT(onBrowseClicked(bool)));

    m_mModel = new DomModel(QDomDocument(), this);

    ui->treeView->setWordWrap(true);
    ui->treeView->setTextElideMode(Qt::ElideMiddle);

    ui->treeView->setModel(m_mModel);
    ui->treeView->expandAll();
}

//-------------------------------------------------------------------------------------------------

void CDOMView::openFile(const QString& sFileName)
{
    if (sFileName.isEmpty() == false)
    {
        ui->fileName->setText(sFileName);
        openFileInternal(sFileName);
    }
}

//-------------------------------------------------------------------------------------------------

void CDOMView::onBrowseClicked(bool bValue)
{
    Q_UNUSED(bValue);

    QString sFileName = QFileDialog::getOpenFileName(this, tr("Select a file"), "", "*.xml *.qml *.js");

    if (sFileName.isEmpty() == false)
    {
        ui->fileName->setText(sFileName);
        openFileInternal(sFileName);
    }
}

//-------------------------------------------------------------------------------------------------

void CDOMView::resizeEvent(QResizeEvent* event)
{
    double w = ui->treeView->width();
    double column0Width = w * 0.4;
    double column1Width = w * 0.4;
    double column2Width = w * 0.2;

    ui->treeView->setColumnWidth(0, column0Width);
    ui->treeView->setColumnWidth(1, column1Width);
    ui->treeView->setColumnWidth(2, column2Width);

    QWidget::resizeEvent(event);
}

//-------------------------------------------------------------------------------------------------

void CDOMView::openFileInternal(const QString& sFileName)
{
    if (sFileName.toLower().endsWith(".xml"))
    {
        setXMLFile(sFileName);
    }
    else if (sFileName.toLower().endsWith(".qml"))
    {
        QString sXMLFile = "Temp.xml";
        QMLTreeContext context;
        context.addFile(sFileName);
        context.setIncludeImports(false);

        if (context.parse() == QMLTreeContext::peSuccess)
        {
            if (context.files().count() > 0 && context.files().first()->parsed())
            {
                context.files().first()->toXMLNode(&context, nullptr).save(sXMLFile);
                setXMLFile(sXMLFile);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CDOMView::setXMLFile(const QString& sFileName)
{
    QFile file(sFileName);

    if (file.open(QIODevice::ReadOnly))
    {
        QDomDocument document;

        if (document.setContent(&file))
        {
            DomModel *newModel = new DomModel(document, this);

            ui->treeView->setModel(newModel);
            ui->treeView->expandAll();

            delete m_mModel;

            m_mModel = newModel;
            m_sXMLPath = sFileName;
        }

        file.close();
    }
}
