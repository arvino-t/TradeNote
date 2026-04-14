#ifndef TODO_TAB_H
#define TODO_TAB_H

#include <memory>
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <functional>
#include "../models/strategy.h"

class TodoTab : public QWidget {
    Q_OBJECT
public:
    using TodoProvider = std::function<QList<TodoItem>()>;
    using TodoAdder = std::function<void(const TodoItem&)>;
    using TodoUpdater = std::function<void(int, bool)>;
    using TodoDeleter = std::function<void(int)>;

    explicit TodoTab(TodoProvider provider, TodoAdder adder, TodoUpdater updater, TodoDeleter deleter, QWidget* parent = nullptr);

    void refreshData();

private slots:
    void onAddItem();
    void onDeleteItem();
    void onCellChanged(int row, int column);

private:
    void setupUi();

    TodoProvider todo_provider;
    TodoAdder todo_adder;
    TodoUpdater todo_updater;
    TodoDeleter todo_deleter;

    std::unique_ptr<QTableWidget> table;
    std::unique_ptr<QLineEdit> item_edit;
    std::unique_ptr<QPushButton> add_button;
    std::unique_ptr<QPushButton> delete_button;
    bool is_refreshing = false;
};

#endif // TODO_TAB_H
