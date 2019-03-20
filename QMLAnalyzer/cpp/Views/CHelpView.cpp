
// Application
#include "CHelpView.h"
#include "CUtils.h"

//-------------------------------------------------------------------------------------------------

CHelpView::CHelpView(QWidget* pParent)
    : CHTMLView(pParent)
{
    load(":/documentation/PanelDesigner_User_Manual.htm");
}
