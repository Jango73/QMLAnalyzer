
#pragma once

// Qt
#include <QAbstractTableModel>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QDirModel>
#include <QMutex>
#include <QWidget>

// Foundations
#include "QMLTree/QMLAnalyzer.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

namespace Ui {
    class CCodeAnalyzerView;
}

//-------------------------------------------------------------------------------------------------

class CErrorListModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Enumerators
    //-------------------------------------------------------------------------------------------------

    enum Roles {
        File = Qt::UserRole + 1,
        Line,
        Column,
        Text
    };

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    // Constructor
    explicit CErrorListModel(QObject* parent = nullptr);

    // Destructor
    virtual ~CErrorListModel();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setErrorList(const QVector<QMLAnalyzerError>& errors);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    QHash<int, QByteArray> roleNames() const;

    //!
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

    //!
    virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    //!
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    //!
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    void clear();

    //!
    void addError(QMLAnalyzerError error);

    //!
    QMap<QString, QVector<QMLAnalyzerError> > errorMap() const;

    //!
    void lineAdded(QString sFileName, int position);

    //!
    void lineRemoved(QString sFileName, int position);

    //!
    void revertPositions();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    QVector<QMLAnalyzerError>   m_vErrors;
};

//-------------------------------------------------------------------------------------------------

class CCodeAnalyzerView : public QWidget
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Constructor
    explicit CCodeAnalyzerView(QWidget* parent = 0);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

signals:

    //!
    void newData(QMLAnalyzerError error);

    //!
    void requestDOMOpen(QString sFileName);

    //-------------------------------------------------------------------------------------------------
    // Slots
    //-------------------------------------------------------------------------------------------------

protected slots:

    //!
    void onBrowseClicked(bool bValue);

    //!
    void onBrowseRuleFileClicked(bool bValue);

    //!
    void onBrowseFormatFileClicked(bool bValue);

    //!
    void onRunClicked(bool bValue);

    //!
    void onExportReportClicked(bool bValue);

    //!
    void onAnalyzeFinished();

    //!
    void onParsingStarted(QString sFileName);

    //!
    void onParsingFinished(QString sFileName);

    //!
    void onImportParsingStarted(QString sFileName);

    //!
    void onAnalyzeError(QMLAnalyzerError error);

    //!
    void onAnalyzerFinished();

    //!
    void onItemClicked(QModelIndex index);

    //!
    void onItemDoubleClicked(QModelIndex index);

    //!
    void onNewData(QMLAnalyzerError error);

    //!
    void onDirtyChanged(bool bDirty);

    //!
    void onSaveCurrentDocument();

    //!
    void onLineAdded(int position);

    //!
    void onLineRemoved(int position);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

private:

    Ui::CCodeAnalyzerView*      ui;
    QMLAnalyzer*                m_pAnalyzer;
    CErrorListModel             m_mErrors;
    QSortFilterProxyModel       m_tErrorsProxy;
    QCompleter*                 m_pCompleter;
    QString                     m_sEditedFileName;
};
