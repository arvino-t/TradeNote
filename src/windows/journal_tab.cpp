#include "journal_tab.h"
#include "resources/icons.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

JournalTab::JournalTab(JournalProvider provider, JournalAdder adder, JournalDeleter deleter, QWidget* parent)
    : QWidget(parent), journal_provider(provider), journal_adder(adder), journal_deleter(deleter) {
    setupUi();
    refreshData();
}

void JournalTab::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Input Area
    QHBoxLayout* input_layout = new QHBoxLayout();
    entry_edit = std::make_unique<QLineEdit>(this);
    entry_edit->setPlaceholderText("Enter journal entry...");
    
    priority_combo = std::make_unique<QComboBox>(this);
    priority_combo->addItem(get_priority_icon(Priority::Low), "Low");
    priority_combo->addItem(get_priority_icon(Priority::Medium), "Medium");
    priority_combo->addItem(get_priority_icon(Priority::High), "High");
    priority_combo->setCurrentIndex(1); // Medium

    add_button = std::make_unique<QPushButton>("Add Note", this);
    connect(add_button.get(), &QPushButton::clicked, this, &JournalTab::onAddEntry);
    connect(entry_edit.get(), &QLineEdit::returnPressed, this, &JournalTab::onAddEntry);

    input_layout->addWidget(new QLabel("New Entry:", this));
    input_layout->addWidget(entry_edit.get(), 1);
    input_layout->addWidget(new QLabel("Priority:", this));
    input_layout->addWidget(priority_combo.get());
    input_layout->addWidget(add_button.get());

    delete_button = std::make_unique<QPushButton>("Delete Selected", this);
    delete_button->setEnabled(false);
    connect(delete_button.get(), &QPushButton::clicked, this, &JournalTab::onDeleteEntry);
    input_layout->addWidget(delete_button.get());

    layout->addLayout(input_layout);

    // Table
    table = std::make_unique<QTableWidget>(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Timestamp", "Priority", "Text"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    
    connect(table.get(), &QTableWidget::itemSelectionChanged, [this]() {
        delete_button->setEnabled(!table->selectedItems().isEmpty());
    });
    
    layout->addWidget(table.get());
}

void JournalTab::refreshData() {
    table->setRowCount(0);
    delete_button->setEnabled(false);
    auto entries = journal_provider();
    
    // Sort by timestamp descending
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.timestamp > b.timestamp;
    });

    for (const auto& entry : entries) {
        int row = table->rowCount();
        table->insertRow(row);
        
        table->setItem(row, 0, new QTableWidgetItem(entry.timestamp.toString("yyyy-MM-dd HH:mm:ss")));
        
        QTableWidgetItem* p_item = new QTableWidgetItem();
        p_item->setIcon(get_priority_icon(entry.priority));
        p_item->setToolTip(priorityToString(entry.priority));
        table->setItem(row, 1, p_item);
        
        table->setItem(row, 2, new QTableWidgetItem(entry.text));
    }
}

void JournalTab::onAddEntry() {
    QString text = entry_edit->text().trimmed();
    if (text.isEmpty()) return;

    Priority p = static_cast<Priority>(priority_combo->currentIndex());
    journal_adder(JournalEntry(text, p));
    
    entry_edit->clear();
    refreshData();
}

void JournalTab::onDeleteEntry() {
    int row = table->currentRow();
    if (row < 0) return;

    // The data in the table is sorted by timestamp descending
    // We need to find the correct index in the original list
    // Or we can pass the timestamp to the deleter.
    // However, refreshData sorts it.
    
    auto entries = journal_provider();
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.timestamp > b.timestamp;
    });

    if (row < entries.size()) {
        // We need to find the index in the original UNSORTED list (as it is in Strategy)
        // Actually, it's easier to just pass the index from the sorted list if the deleter handles it,
        // but Strategy holds them in a specific order.
        
        // Let's find the original index by comparing timestamp and text
        auto selected_entry = entries[row];
        auto original_entries = journal_provider(); // unsorted
        for (int i = 0; i < original_entries.size(); ++i) {
            if (original_entries[i].timestamp == selected_entry.timestamp && 
                original_entries[i].text == selected_entry.text) {
                journal_deleter(i);
                break;
            }
        }
    }
    
    refreshData();
}

QString JournalTab::priorityToString(Priority p) const {
    switch (p) {
        case Priority::Low: return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High: return "High";
    }
    return "Unknown";
}

QColor JournalTab::priorityToColor(Priority p) const {
    switch (p) {
        case Priority::Low: return Qt::gray;
        case Priority::Medium: return Qt::yellow;
        case Priority::High: return Qt::red;
    }
    return Qt::white;
}
