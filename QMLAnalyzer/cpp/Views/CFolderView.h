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
#include <QWidget>
#include <QFileSystemModel>

// Application
#include "CImportProcessor.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class CMainController;
class CFolderView;

namespace Ui {
    class CFolderView;
}

//-------------------------------------------------------------------------------------------------

class CQMLFileInfo
{
public:

    QString     m_sFullPath;
};

//-------------------------------------------------------------------------------------------------

class CFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CFileSystemModel(CFolderView* view);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    //!
    virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    //!
    void onProcessingFile(const QString& sFileName);

    //!
    void onFinished();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

public:

    CImportProcessor    m_tProcessor;
    CFolderView*        m_pView;
};

//-------------------------------------------------------------------------------------------------

class CFolderView : public QWidget
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    CFolderView(QWidget* parent = nullptr);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QString baseDirectory() const;

    //!
    QString gitBashExecutable() const;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void setLeftFolder(const QString& sFolder);

    //!
    void setRightFolder(const QString& sFolder);

    //!
    void log(const QString& sText);

    //-------------------------------------------------------------------------------------------------
    // Protected control methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    //!
    void onBrowseClicked(bool bValue);

    //!
    void onBaseDirectoryChanged(QString sText);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    Ui::CFolderView*    m_pUI;
    CFileSystemModel*   m_pLeftModel;
    CFileSystemModel*   m_pRightModel;
};
