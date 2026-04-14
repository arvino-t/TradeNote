#include "journal_entry.h"

JournalEntry::JournalEntry(const QString& text, Priority priority, const QDateTime& timestamp)
    : text(text), priority(priority), timestamp(timestamp) {}

QVariantMap JournalEntry::toMap() const {
    QVariantMap map;
    map["text"] = text;
    map["priority"] = static_cast<int>(priority);
    map["timestamp"] = timestamp.toString(Qt::ISODate);
    return map;
}

JournalEntry JournalEntry::fromMap(const QVariantMap& data) {
    return JournalEntry(
        data["text"].toString(),
        static_cast<Priority>(data["priority"].toInt()),
        QDateTime::fromString(data["timestamp"].toString(), Qt::ISODate)
    );
}
