#ifndef JOURNAL_ENTRY_H
#define JOURNAL_ENTRY_H

#include <QString>
#include <QDateTime>
#include <QVariantMap>

enum class Priority {
    Low,
    Medium,
    High
};

class JournalEntry {
public:
    JournalEntry() = default;
    JournalEntry(const QString& text, Priority priority = Priority::Medium, const QDateTime& timestamp = QDateTime::currentDateTime());

    QVariantMap toMap() const;
    static JournalEntry fromMap(const QVariantMap& data);

    QString text;
    Priority priority;
    QDateTime timestamp;
};

#endif // JOURNAL_ENTRY_H
