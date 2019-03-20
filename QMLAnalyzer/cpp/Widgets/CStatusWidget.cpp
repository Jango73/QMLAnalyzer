
// Qt
#include <QSyntaxHighlighter>
#include <QStringListModel>
#include <QTextCursor>
#include <QMutexLocker>
#include <QDebug>

// Application
#include "CStatusWidget.h"
#include "ui_CStatusWidget.h"

//-------------------------------------------------------------------------------------------------

/*!
    Constructor.
*/
SearchHighlighter::SearchHighlighter(QTextDocument *parent, bool has_cursor_)
    : QSyntaxHighlighter(parent)
    , has_cursor(has_cursor_)
    , _num_occurences(0)
    , _occurence_cursor(0)
    , last_cursor_pos(0)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Set search string to be highlighted.
*/
void SearchHighlighter::setSearchString(const QString &search)
{
    // reset all before highlight loop
    _search_string = search;
    _occurence_cursor = _num_occurences = 0;
    search_string_changed = true;
    rehighlight();
    if (has_cursor)
    {
        if (search_string_changed && _num_occurences > 0)
        {
            // last highlight loop, we didn't mark nothing because the cursor
            // should be positioned before the previous position
            // so now we rest it and redo the loop
            last_cursor_pos = 0;
            setSearchString(search);
        }
        if (search.isEmpty())
            emit totalOccurencesChanged(-1);
        else
            emit totalOccurencesChanged(_num_occurences);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Highlight block.
*/
void SearchHighlighter::highlightBlock(const QString &text)
{
    const int block_position = currentBlock().position();

    // highlighted text background color (search results)
    static const Qt::GlobalColor SEARCHRESULT_BACKCOL = Qt::yellow;

    // highlighted text background color (search results)
    static const Qt::GlobalColor CURSOR_SEARCHRESULT_BACKCOL = Qt::red;

    if (_search_string.isEmpty() || text.isEmpty())
        return;

    QTextCharFormat charFormat;

    const int length = _search_string.length();
    int index = text.indexOf(_search_string, 0, Qt::CaseInsensitive);
    while (index >= 0)
    {
        charFormat.setBackground(SEARCHRESULT_BACKCOL);

        if (has_cursor)
        {
            if (search_string_changed)
            {
                if (block_position + index >= last_cursor_pos)
                {
                    // if search_string just changed, _occurence_cursor has been invalidated
                    // because it is bound to a specific search string
                    // however last_cursor_pos records the position
                    // so now we highlight the first search result we find
                    last_cursor_pos = block_position + index;
                    _occurence_cursor = _num_occurences;
                    charFormat.setBackground(CURSOR_SEARCHRESULT_BACKCOL);
                    search_string_changed = false;

                    emit cursorPosChanged(block_position + index);
                }
            }
            else if (_num_occurences == _occurence_cursor)
            {
                // record position of this occurence
               last_cursor_pos = block_position + index;
                charFormat.setBackground(CURSOR_SEARCHRESULT_BACKCOL);

                emit cursorPosChanged(block_position + index);
            }
        }

        setFormat(index, length, charFormat);

        index = text.indexOf(_search_string, index + length, Qt::CaseInsensitive);
        ++_num_occurences;
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Find previous occurence.
*/
void SearchHighlighter::previousOccurence()
{
    // nothing to do if the string-search returned nothing
    if (!_search_string.isEmpty())
    {
        // cyclic behaviour
        if (_occurence_cursor == 0)
            _occurence_cursor = _num_occurences - 1;
        else
            --_occurence_cursor;
        _num_occurences = 0;

        rehighlight();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Find next occurence.
*/
void SearchHighlighter::nextOccurence()
{
    if (!_search_string.isEmpty())
    {
        // cyclic behaviour
        if (_occurence_cursor == _num_occurences - 1)
            _occurence_cursor = 0;
        else
            ++_occurence_cursor;
        _num_occurences = 0;

        rehighlight();
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Return cursor occurence.
*/
int SearchHighlighter::cursorOccurence() const
{
    return _occurence_cursor;
}

//-------------------------------------------------------------------------------------------------

/*!
    Return total occurences.
*/
int SearchHighlighter::totalOccurences() const
{
    return _num_occurences;
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructor.
*/
CStatusWidget::CStatusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CStatusWidget)
    , m_mMutex(QMutex::Recursive)
    , m_tTimer(this)
{
    ui->setupUi(this);

    // Syntax highlighter:
    m_pSearchHighlighter = new SearchHighlighter(ui->statusWidget->document());

    // Set event filter:
    installEventFilter(this);

    // Listen to text changed:
    connect(ui->searchArea, &QLineEdit::textChanged, this, &CStatusWidget::onSearchTextChanged);

    // Clear log:
    connect(ui->clearLogButton, &QPushButton::clicked, this, &CStatusWidget::onClearLog);

    connect(&m_tTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

    m_tTimer.setInterval(1000);
    m_tTimer.start();
}

//-------------------------------------------------------------------------------------------------

/*!
    Destructor.
*/
CStatusWidget::~CStatusWidget()
{
    delete ui;
}

//-------------------------------------------------------------------------------------------------

/*!
    Append text.
*/
void CStatusWidget::appendPlainText(const QString& sText)
{
    m_lLinesToAdd << sText;
}

//-------------------------------------------------------------------------------------------------

/*!
    Return text under cursor.
*/
QString CStatusWidget::textUnderCursor() const
{
    return ui->statusWidget->textCursor().selectedText();
}

//-------------------------------------------------------------------------------------------------

/*!
    Filter events.
*/
bool CStatusWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(event);

        if (pKeyEvent)
        {
            // Show hide search area:
            if (pKeyEvent->key() == Qt::Key_F && pKeyEvent->modifiers() == Qt::CTRL) {
                ui->searchArea->setFocus();
                ui->searchArea->setText("");
                ui->searchArea->setText(textUnderCursor());
            }
            else
            // Next occurence:
            if (pKeyEvent->key() == Qt::Key_F3)
                m_pSearchHighlighter->nextOccurence();
            else
            // Previous occurence:
            if (pKeyEvent->key() == Qt::Key_F4)
                m_pSearchHighlighter->previousOccurence();
        }
    }

    return QWidget::eventFilter(target, event);
}

//-------------------------------------------------------------------------------------------------

/*!
    Search text changed.
*/
void CStatusWidget::onSearchTextChanged()
{
    m_pSearchHighlighter->setSearchString(ui->searchArea->text());
}

//-------------------------------------------------------------------------------------------------

/*!
    Clear log.
*/
void CStatusWidget::onClearLog()
{
    ui->statusWidget->clear();
}

//-------------------------------------------------------------------------------------------------

void CStatusWidget::onTimer()
{
    if (m_lLinesToAdd.count() > 0)
    {
        QMutexLocker locker(&m_mMutex);

        foreach (QString sText, m_lLinesToAdd)
        {
            if (ui->statusWidget->blockCount() > 500)
            {
                QTextCursor cursor = ui->statusWidget->textCursor();

                cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.removeSelectedText();

                ui->statusWidget->setTextCursor(cursor);
            }

            ui->statusWidget->appendPlainText(sText);
        }

        m_lLinesToAdd.clear();
    }
}
