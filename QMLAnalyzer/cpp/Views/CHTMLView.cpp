
// Qt
#include <QFileInfo>
#include <QTextStream>

// Application
#include "CHTMLView.h"
#include "CUtils.h"

//-------------------------------------------------------------------------------------------------

// Constructor:
CHTMLView::CHTMLView(QWidget* pParent)
    : QTextEdit(pParent)
{
}

//-------------------------------------------------------------------------------------------------

// Load:
void CHTMLView::load(const QString &sFilePath)
{
    if (QFileInfo::exists(sFilePath))
    {
        QFile fi(sFilePath);

        if (fi.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&fi);
            QString sContents = in.readAll();
            setHtml(sContents);
            fi.close();
        }
    }
}
