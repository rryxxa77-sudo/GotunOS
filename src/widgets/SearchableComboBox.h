#pragma once
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QString>
#include <QVariant>

// A combo box you can type into directly: the field itself is the search box,
// so there is no extra bordered search widget above the list. Typing filters
// the drop-down live; leaving the field restores the last valid selection.
class SearchableComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit SearchableComboBox(QWidget *parent = nullptr);

    void addEntry(const QString &text, const QVariant &data = QVariant());
    void clearEntries();
    int  entryCount() const;

    bool selectByData(const QVariant &data);
    bool selectByText(const QString &text, Qt::MatchFlags flags = Qt::MatchStartsWith);
    void selectFirst();

    QString  selectedText() const { return m_selectedText; }
    QVariant selectedData() const { return m_selectedData; }
    bool     hasSelection() const { return !m_selectedText.isEmpty(); }

signals:
    void selectionChanged(const QString &text, const QVariant &data);

protected:
    void showPopup() override;
    void hidePopup() override;
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void applyFilter(const QString &needle);
    void commitRow(int proxyRow);
    void restoreSelectionText();

    QStandardItemModel    *m_source = nullptr;
    QSortFilterProxyModel *m_proxy  = nullptr;
    QString  m_selectedText;
    QVariant m_selectedData;
    bool     m_typing = false;
};
