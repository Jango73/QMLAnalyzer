
#pragma once

// Qt
#include <QLabel>

//-------------------------------------------------------------------------------------------------

class CSpacerItem : public QLabel
{
public:
    // Constructor:
    CSpacerItem(int width=0, QWidget *parent=0);

private:
    // Width:
    int m_iWidth;
};
