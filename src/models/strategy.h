#ifndef STRATEGY_H
#define STRATEGY_H

#include <QString>
#include <QList>
#include <QVariantMap>
#include <numeric>
#include "deal.h"
#include "journal_entry.h"
#include "todo_item.h"

class Strategy {
public:
    Strategy() = default;
    Strategy(const QString& name, const QString& type = "Scalping", int deal_limit = 3, 
             const QVariantMap& settings = {}, const QString& default_market = "MOEX Shares");

    void addDeal(const Deal& deal);
    void addJournalEntry(const JournalEntry& entry);
    void removeJournalEntry(int index);
    void addTodoItem(const TodoItem& item);
    void removeTodoItem(int index);
    bool isDealLimitReached() const;
    
    double totalProfit() const;
    double dailyProfit() const;
    double dailyWinrate() const;

    QString name;
    QString strategy_type;
    int deal_limit;
    QList<Deal> deals;
    QList<JournalEntry> journal_entries;
    QList<TodoItem> todo_items;
    QVariantMap settings;
    QString default_market;
};

#endif // STRATEGY_H
