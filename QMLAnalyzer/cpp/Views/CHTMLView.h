/*******************************************************************************
* P. C. A.
*
* Peugeot Citroen Automobiles
*
* This file is the property of PCA. All rights are reserved
* by PCA and this file must not be copied or disclosed
* (in whole or in part) without prior written consent of PCA.
*
* Concept: Raphaël Drap
* Authors: Guillaume Darier, Franck Legall
*
*******************************************************************************/

#pragma once

// Qt
#include <QTextEdit>

//-------------------------------------------------------------------------------------------------

class CHTMLView : public QTextEdit
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    // Constructor
    CHTMLView(QWidget *pParent = nullptr);

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

protected:

    // Load
    void load(const QString &sFilePath);
};
