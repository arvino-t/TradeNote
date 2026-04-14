#include "todo_tab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QCheckBox>

TodoTab::TodoTab(TodoProvider provider, TodoAdder adder, TodoUpdater updater, TodoDeleter deleter, QWidget* parent)
    : QWidget(parent), todo_provider(provider), todo_adder(adder), todo_updater(updater), todo_deleter(deleter) {
    setupUi();
    refreshData();
}

void TodoTab::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Input Area
    QHBoxLayout* input_layout = new QHBoxLayout();
    item_edit = std::make_unique<QLineEdit>(this);
    item_edit->setPlaceholderText("Enter TODO task...");
    
    add_button = std::make_unique<QPushButton>("Add Task", this);
    connect(add_button.get(), &QPushButton::clicked, this, &TodoTab::onAddItem);
    connect(item_edit.get(), &QLineEdit::returnPressed, this, &TodoTab::onAddItem);

    input_layout->addWidget(new QLabel("New Task:", this));
    input_layout->addWidget(item_edit.get(), 1);
    input_layout->addWidget(add_button.get());

    delete_button = std::make_unique<QPushButton>("Delete Selected", this);
    delete_button->setEnabled(false);
    connect(delete_button.get(), &QPushButton::clicked, this, &TodoTab::onDeleteItem);
    input_layout->addWidget(delete_button.get());

    layout->addLayout(input_layout);

    // Table
    table = std::make_unique<QTableWidget>(this);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Done", "Task"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    
    connect(table.get(), &QTableWidget::cellChanged, this, &TodoTab::onCellChanged);
    connect(table.get(), &QTableWidget::itemSelectionChanged, [this]() {
        delete_button->setEnabled(!table->selectedItems().isEmpty());
    });
    
    layout->addWidget(table.get());
}

void TodoTab::refreshData() {
    is_refreshing = true;
    table->setRowCount(0);
    delete_button->setEnabled(false);
    auto items = todo_provider();
    
    for (const auto& item : items) {
        int row = table->rowCount();
        table->insertRow(row);
        
        QTableWidgetItem* checkItem = new QTableWidgetItem();
        checkItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);
        table->setItem(row, 0, checkItem);
        
        QTableWidgetItem* textItem = new QTableWidgetItem(item.text);
        if (item.completed) {
            QFont font = textItem->font();
            font.setStrikeOut(true);
            textItem->setFont(font);
            textItem->setForeground(Qt::gray);
        }
        table->setItem(row, 1, textItem);
    }
    is_refreshing = false;
}

void TodoTab::onAddItem() {
    QString text = item_edit->text().trimmed();
    if (text.isEmpty()) return;

    todo_adder(TodoItem(text));
    
    item_edit->clear();
    refreshData();
}

void TodoTab::onDeleteItem() {
    int row = table->currentRow();
    if (row < 0) return;

    todo_deleter(row);
    refreshData();
}

void TodoTab::onCellChanged(int row, int column) {
    if (is_refreshing) return;
    if (column == 0) {
        bool completed = (table->item(row, column)->checkState() == Qt::Checked);
        todo_updater(row, completed);
        refreshData();
    }
}
