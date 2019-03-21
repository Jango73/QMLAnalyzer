
// Application
#include "CHelpView.h"
#include "CUtils.h"

//-------------------------------------------------------------------------------------------------

CHelpView::CHelpView(QWidget* pParent)
    : CHTMLView(pParent)
{
    load(":/misc/help.html");
}
