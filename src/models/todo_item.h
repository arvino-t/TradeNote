#ifndef TODO_ITEM_H
#define TODO_ITEM_H

#include <QString>
#include <QVariantMap>

class TodoItem {
public:
    TodoItem() = default;
    TodoItem(const QString& text, bool completed = false)
        : text(text), completed(completed) {}

    QVariantMap toMap() const {
        QVariantMap map;
        map["text"] = text;
        map["completed"] = completed;
        return map;
    }

    static TodoItem fromMap(const QVariantMap& data) {
        return TodoItem(
            data.value("text").toString(),
            data.value("completed").toBool()
        );
    }

    QString text;
    bool completed;
};

#endif // TODO_ITEM_H
