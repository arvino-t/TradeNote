#include "strategy.h"
#include <QDate>
#include <algorithm>

Strategy::Strategy(const QString& name, const QString& type, int deal_limit, 
                   const QVariantMap& settings, const QString& default_market)
    : name(name), strategy_type(type), deal_limit(deal_limit), 
      settings(settings), default_market(default_market) {}

void Strategy::addDeal(const Deal& deal) {
    deals.append(deal);
}

void Strategy::addJournalEntry(const JournalEntry& entry) {
    journal_entries.append(entry);
}

void Strategy::removeJournalEntry(int index) {
    if (index >= 0 && index < journal_entries.size()) {
        journal_entries.removeAt(index);
    }
}

void Strategy::addTodoItem(const TodoItem& item) {
    todo_items.append(item);
}

void Strategy::removeTodoItem(int index) {
    if (index >= 0 && index < todo_items.size()) {
        todo_items.removeAt(index);
    }
}

bool Strategy::isDealLimitReached() const {
    QDate today = QDate::currentDate();
    int dailyDealsCount = std::count_if(deals.begin(), deals.end(), [&today](const Deal& deal) {
        return deal.date == today;
    });
    return dailyDealsCount >= deal_limit;
}

double Strategy::totalProfit() const {
    return std::accumulate(deals.begin(), deals.end(), 0.0, [](double sum, const Deal& deal) {
        return sum + deal.profit;
    });
}

double Strategy::dailyProfit() const {
    QDate today = QDate::currentDate();
    return std::accumulate(deals.begin(), deals.end(), 0.0, [&today](double sum, const Deal& deal) {
        return (deal.date == today) ? sum + deal.profit : sum;
    });
}

double Strategy::dailyWinrate() const {
    QDate today = QDate::currentDate();
    int dailyDealsCount = 0;
    int wins = 0;
    for (const auto& deal : deals) {
        if (deal.date == today) {
            dailyDealsCount++;
            if (deal.profit > 0) {
                wins++;
            }
        }
    }
    if (dailyDealsCount == 0) return 0.0;
    return (static_cast<double>(wins) / dailyDealsCount) * 100.0;
}
