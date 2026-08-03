#include "SearchableComboBox.h"
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QLineEdit>

SearchableComboBox::SearchableComboBox(QWidget *parent) : QComboBox(parent) {
    m_source = new QStandardItemModel(this);
    m_proxy  = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_source);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    setModel(m_proxy);
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    view()->setTextElideMode(Qt::ElideMiddle);

    lineEdit()->setClearButtonEnabled(false);

    connect(lineEdit(), &QLineEdit::textEdited, this, [this](const QString &text) {
        m_typing = true;
        applyFilter(text);
        if (!view()->isVisible()) QComboBox::showPopup();
    });

    connect(this, &QComboBox::activated, this, &SearchableComboBox::commitRow);
}

void SearchableComboBox::addEntry(const QString &text, const QVariant &data) {
    auto *item = new QStandardItem(text);
    item->setData(data.isValid() ? data : QVariant(text), Qt::UserRole);
    item->setEditable(false);
    m_source->appendRow(item);
}

void SearchableComboBox::clearEntries() {
    m_source->clear();
    m_selectedText.clear();
    m_selectedData.clear();
    setEditText(QString());
}

int SearchableComboBox::entryCount() const { return m_source->rowCount(); }

void SearchableComboBox::applyFilter(const QString &needle) {
    m_proxy->setFilterFixedString(needle);
}

void SearchableComboBox::commitRow(int proxyRow) {
    if (proxyRow < 0 || proxyRow >= m_proxy->rowCount()) return;
    const QModelIndex idx = m_proxy->index(proxyRow, 0);
    m_selectedText = idx.data(Qt::DisplayRole).toString();
    m_selectedData = idx.data(Qt::UserRole);
    m_typing = false;
    applyFilter(QString());
    restoreSelectionText();
    emit selectionChanged(m_selectedText, m_selectedData);
}

void SearchableComboBox::restoreSelectionText() {
    // Keep the visible index in sync with the (unfiltered) selection.
    const QSignalBlocker blocker(this);
    for (int row = 0; row < m_proxy->rowCount(); ++row) {
        if (m_proxy->index(row, 0).data(Qt::DisplayRole).toString() == m_selectedText) {
            setCurrentIndex(row);
            break;
        }
    }
    setEditText(m_selectedText);
}

bool SearchableComboBox::selectByData(const QVariant &data) {
    for (int row = 0; row < m_source->rowCount(); ++row) {
        if (m_source->item(row)->data(Qt::UserRole) == data) {
            m_selectedText = m_source->item(row)->text();
            m_selectedData = data;
            applyFilter(QString());
            restoreSelectionText();
            emit selectionChanged(m_selectedText, m_selectedData);
            return true;
        }
    }
    return false;
}

bool SearchableComboBox::selectByText(const QString &text, Qt::MatchFlags flags) {
    const auto hits = m_source->match(m_source->index(0, 0), Qt::DisplayRole, text, 1,
                                      flags | Qt::MatchCaseSensitive);
    if (hits.isEmpty()) return false;
    m_selectedText = hits.first().data(Qt::DisplayRole).toString();
    m_selectedData = hits.first().data(Qt::UserRole);
    applyFilter(QString());
    restoreSelectionText();
    emit selectionChanged(m_selectedText, m_selectedData);
    return true;
}

void SearchableComboBox::selectFirst() {
    if (m_source->rowCount() == 0) return;
    m_selectedText = m_source->item(0)->text();
    m_selectedData = m_source->item(0)->data(Qt::UserRole);
    applyFilter(QString());
    restoreSelectionText();
    emit selectionChanged(m_selectedText, m_selectedData);
}

void SearchableComboBox::showPopup() {
    // Opening the list always shows everything; typing narrows it down again.
    if (!m_typing) applyFilter(QString());
    QComboBox::showPopup();
}

void SearchableComboBox::hidePopup() {
    QComboBox::hidePopup();
    if (m_typing) {
        m_typing = false;
        applyFilter(QString());
        restoreSelectionText();
    }
}

void SearchableComboBox::focusOutEvent(QFocusEvent *e) {
    QComboBox::focusOutEvent(e);
    m_typing = false;
    applyFilter(QString());
    restoreSelectionText();
}

void SearchableComboBox::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        m_typing = false;
        applyFilter(QString());
        restoreSelectionText();
        e->accept();
        return;
    }
    if ((e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) && m_typing) {
        if (m_proxy->rowCount() > 0) commitRow(view()->currentIndex().isValid()
                                                   ? view()->currentIndex().row() : 0);
        e->accept();
        return;
    }
    QComboBox::keyPressEvent(e);
}
