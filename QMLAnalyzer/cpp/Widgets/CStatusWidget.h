
#pragma once

// Qt
#include <QObject>
#include <QWidget>
#include <QSyntaxHighlighter>
#include <QMutex>
#include <QTimer>

//-------------------------------------------------------------------------------------------------
// Forward declarations

namespace Ui {
class CStatusWidget;
}
class QStringListModel;

//-------------------------------------------------------------------------------------------------

// Syntax highlighter for search results
class SearchHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    // The current search string.
    QString _search_string;

    // Indicate if highlighter maintains an occurence cursor.
    bool has_cursor;

    // Count number N of occurences of search string in document.
    int _num_occurences;

    // Occurence cursor [0, N-1].
    int _occurence_cursor;

    // Last occurence cursor position.
    int last_cursor_pos;

    // Indicates that search string has just been changed.
    bool search_string_changed;

signals:
    // Cursor position changed.
    void cursorPosChanged(int pos);

    // Total occurences change.
    void totalOccurencesChanged(int total_occurences);

public:
    // Constructor.
    SearchHighlighter(QTextDocument *parent, bool has_cursor_ = true);

    // Set search string.
    void setSearchString(const QString &search);

    // Highlight block.
    void highlightBlock(const QString &text);

    // Find previous occurence.
    void previousOccurence();

    // Find next occurence.
    void nextOccurence();

    // Return cursor occurence.
    int cursorOccurence() const;

    // Return total occurences.
    int totalOccurences() const;
};

//-------------------------------------------------------------------------------------------------

class CStatusWidget : public QWidget
{
    Q_OBJECT

public:
    // Constructor:
    explicit CStatusWidget(QWidget *parent = 0);

    // Destructor:
    ~CStatusWidget();

    // Append plain text:
    void appendPlainText(const QString& sText);

protected:
    // Event filter:
    bool eventFilter(QObject* target, QEvent* event);

private:
    // Return text under cursor:
    QString textUnderCursor() const;

public slots:
    // Search text changed:
    void onSearchTextChanged();

    // Clear log:
    void onClearLog();

    void onTimer();

protected:

    Ui::CStatusWidget*  ui;
    SearchHighlighter*  m_pSearchHighlighter;
    QMutex              m_mMutex;
    QTimer              m_tTimer;
    QStringList         m_lLinesToAdd;
};
