
// Application
#include "CSpacerItem.h"

//-------------------------------------------------------------------------------------------------

// Constructor
CSpacerItem::CSpacerItem(int iWidth, QWidget *parent)
    : QLabel(parent)
    , m_iWidth(iWidth)
{
    setFixedHeight(1);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    if (iWidth <= 0) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    else setFixedWidth(iWidth);
}
