#ifndef STATISTICS_TAB_H
#define STATISTICS_TAB_H

#include <memory>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <functional>
#include "models/deal.h"
#include "models/strategy.h"

class MetricCard : public QFrame {
    Q_OBJECT
public:
    MetricCard(const QString& title, const QString& value, QWidget* parent = nullptr, const QString& color = "");
    void updateValue(const QString& value, const QString& color = "");

private:
    QLabel* value_label;
};

class TickerBar : public QWidget {
    Q_OBJECT
public:
    TickerBar(const QString& ticker, double profit, double max_profit, QWidget* parent = nullptr);

private:
    QProgressBar* progress;
};

class StatisticsTab : public QWidget {
    Q_OBJECT
public:
    using DealsProvider = std::function<QList<std::pair<Deal, Strategy>>()>;
    explicit StatisticsTab(DealsProvider provider, QWidget* parent = nullptr);

    void refreshData();

private:
    void setEmptyState();
    void updateTickerStats(const QList<Deal>& deals);

    DealsProvider deals_provider;
    std::unique_ptr<MetricCard> winrate_card;
    std::unique_ptr<MetricCard> total_profit_card;
    std::unique_ptr<MetricCard> profit_factor_card;
    std::unique_ptr<MetricCard> avg_win_card;
    std::unique_ptr<MetricCard> avg_loss_card;
    std::unique_ptr<MetricCard> total_deals_card;
    std::unique_ptr<QProgressBar> winrate_bar;
    QVBoxLayout* ticker_vbox;
    QFrame* ticker_group;
};

#endif // STATISTICS_TAB_H
