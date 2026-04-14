#ifndef JOURNAL_TAB_H
#define JOURNAL_TAB_H

#include <memory>
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <functional>
#include "models/strategy.h"
#include "models/journal_entry.h"

class JournalTab : public QWidget {
    Q_OBJECT
public:
    using JournalProvider = std::function<QList<JournalEntry>()>;
    using JournalAdder = std::function<void(const JournalEntry&)>;
    using JournalDeleter = std::function<void(int)>;

    explicit JournalTab(JournalProvider provider, JournalAdder adder, JournalDeleter deleter, QWidget* parent = nullptr);

    void refreshData();

private slots:
    void onAddEntry();
    void onDeleteEntry();

private:
    void setupUi();
    QString priorityToString(Priority p) const;
    QColor priorityToColor(Priority p) const;

    JournalProvider journal_provider;
    JournalAdder journal_adder;
    JournalDeleter journal_deleter;

    std::unique_ptr<QTableWidget> table;
    std::unique_ptr<QLineEdit> entry_edit;
    std::unique_ptr<QComboBox> priority_combo;
    std::unique_ptr<QPushButton> add_button;
    std::unique_ptr<QPushButton> delete_button;
};

#endif // JOURNAL_TAB_H
