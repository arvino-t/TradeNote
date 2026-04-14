#include "statistics_tab.h"
#include <QScrollArea>
#include <QGridLayout>

MetricCard::MetricCard(const QString& title, const QString& value, QWidget* parent, const QString& color)
    : QFrame(parent) {
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setMinimumSize(180, 100);
    setObjectName("MetricCard");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* title_label = new QLabel(title, this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: #bbbbbb; font-size: 14px;");
    
    value_label = new QLabel(value, this);
    value_label->setAlignment(Qt::AlignCenter);
    QString col = color.isEmpty() ? "#ffffff" : color;
    value_label->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: bold;").arg(col));
    
    layout->addWidget(title_label);
    layout->addWidget(value_label);
}

void MetricCard::updateValue(const QString& value, const QString& color) {
    value_label->setText(value);
    if (!color.isEmpty()) {
        value_label->setStyleSheet(QString("color: %1; font-size: 20px; font-weight: bold;").arg(color));
    }
}

TickerBar::TickerBar(const QString& ticker, double profit, double max_profit, QWidget* parent)
    : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 5);
    
    QLabel* ticker_label = new QLabel(ticker, this);
    ticker_label->setFixedWidth(80);
    
    progress = new QProgressBar(this);
    progress->setTextVisible(true);
    progress->setFormat(QString::number(profit, 'f', 2));
    
    int percent = (max_profit > 0) ? static_cast<int>((std::abs(profit) / max_profit) * 100) : 0;
    progress->setValue(percent);
    
    QString color = (profit >= 0) ? "#4caf50" : "#f44336";
    progress->setStyleSheet(QString("QProgressBar::chunk { background-color: %1; }").arg(color));
    
    layout->addWidget(ticker_label);
    layout->addWidget(progress);
}

StatisticsTab::StatisticsTab(DealsProvider provider, QWidget* parent)
    : QWidget(parent), deals_provider(provider) {
    
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    
    QWidget* content_widget = new QWidget(scroll);
    QVBoxLayout* layout = new QVBoxLayout(content_widget);
    
    QLabel* header = new QLabel("Strategy Statistics", content_widget);
    header->setStyleSheet("font-size: 22px; font-weight: bold; margin-bottom: 10px; color: inherit;");
    layout->addWidget(header);
    
    QGridLayout* metrics_layout = new QGridLayout();
    winrate_card = std::make_unique<MetricCard>("Win Rate", "0.0%", content_widget);
    total_profit_card = std::make_unique<MetricCard>("Total Profit", "$0.00", content_widget);
    profit_factor_card = std::make_unique<MetricCard>("Profit Factor", "0.00", content_widget);
    avg_win_card = std::make_unique<MetricCard>("Avg Win", "$0.00", content_widget, "#4caf50");
    avg_loss_card = std::make_unique<MetricCard>("Avg Loss", "$0.00", content_widget, "#f44336");
    total_deals_card = std::make_unique<MetricCard>("Total Deals", "0", content_widget);
    
    metrics_layout->addWidget(winrate_card.get(), 0, 0);
    metrics_layout->addWidget(total_profit_card.get(), 0, 1);
    metrics_layout->addWidget(profit_factor_card.get(), 0, 2);
    metrics_layout->addWidget(avg_win_card.get(), 1, 0);
    metrics_layout->addWidget(avg_loss_card.get(), 1, 1);
    metrics_layout->addWidget(total_deals_card.get(), 1, 2);
    layout->addLayout(metrics_layout);
    
    QFrame* winrate_group = new QFrame(content_widget);
    winrate_group->setFrameShape(QFrame::StyledPanel);
    winrate_group->setObjectName("StatisticsGroup");
    QVBoxLayout* winrate_vbox = new QVBoxLayout(winrate_group);
    winrate_vbox->addWidget(new QLabel("Win/Loss Distribution", winrate_group));
    
    winrate_bar = std::make_unique<QProgressBar>(winrate_group);
    winrate_bar->setFixedHeight(30);
    winrate_bar->setStyleSheet("QProgressBar::chunk { background-color: #4caf50; }");
    winrate_vbox->addWidget(winrate_bar.get());
    layout->addWidget(winrate_group);
    
    ticker_group = new QFrame(content_widget);
    ticker_group->setFrameShape(QFrame::StyledPanel);
    ticker_group->setObjectName("StatisticsGroup");
    ticker_vbox = new QVBoxLayout(ticker_group);
    ticker_vbox->addWidget(new QLabel("Top Tickers by Profit", ticker_group));
    layout->addWidget(ticker_group);
    
    layout->addStretch();
    scroll->setWidget(content_widget);
    main_layout->addWidget(scroll);
    
    refreshData();
}

void StatisticsTab::refreshData() {
    auto deals_with_meta = deals_provider();
    if (deals_with_meta.isEmpty()) {
        setEmptyState();
        return;
    }
    
    QList<Deal> deals;
    for (const auto& pair : deals_with_meta) deals.append(pair.first);
    
    int total_deals = deals.size();
    QList<Deal> wins, losses;
    for (const auto& d : deals) {
        if (d.profit > 0) wins.append(d);
        else if (d.profit < 0) losses.append(d);
    }
    
    double winrate = (total_deals > 0) ? (static_cast<double>(wins.size()) / total_deals * 100.0) : 0.0;
    double total_profit = 0;
    for (const auto& d : deals) total_profit += d.profit;
    
    double total_win_amt = 0;
    for (const auto& d : wins) total_win_amt += d.profit;
    double total_loss_amt = 0;
    for (const auto& d : losses) total_loss_amt += std::abs(d.profit);
    
    double profit_factor = (total_loss_amt > 0) ? (total_win_amt / total_loss_amt) : (total_win_amt > 0 ? 1e18 : 0);
    double avg_win = wins.isEmpty() ? 0 : (total_win_amt / wins.size());
    double avg_loss = losses.isEmpty() ? 0 : (total_loss_amt / losses.size());
    
    winrate_card->updateValue(QString::number(winrate, 'f', 1) + "%");
    
    QString currency = deals.isEmpty() ? "$" : deals[0].currency;
    QString profit_color = (total_profit >= 0) ? "#4caf50" : "#f44336";
    QString profit_str = currency.startsWith("$") ? QString("%1%2").arg(currency).arg(total_profit, 0, 'f', 2) : QString("%1%2").arg(total_profit, 0, 'f', 2).arg(currency);
    total_profit_card->updateValue(profit_str, profit_color);
    
    profit_factor_card->updateValue(profit_factor >= 1e17 ? "∞" : QString::number(profit_factor, 'f', 2));
    avg_win_card->updateValue(currency.startsWith("$") ? QString("%1%2").arg(currency).arg(avg_win, 0, 'f', 2) : QString("%1%2").arg(avg_win, 0, 'f', 2).arg(currency));
    avg_loss_card->updateValue("-" + (currency.startsWith("$") ? QString("%1%2").arg(currency).arg(avg_loss, 0, 'f', 2) : QString("%1%2").arg(avg_loss, 0, 'f', 2).arg(currency)));
    total_deals_card->updateValue(QString::number(total_deals));
    
    winrate_bar->setValue(static_cast<int>(winrate));
    winrate_bar->setFormat(QString("Wins: %1 | Losses: %2 (%3%)").arg(wins.size()).arg(losses.size()).arg(winrate, 0, 'f', 1));
    
    updateTickerStats(deals);
}

void StatisticsTab::updateTickerStats(const QList<Deal>& deals) {
    while (ticker_vbox->count() > 1) {
        auto item = ticker_vbox->takeAt(1);
        if (item->widget()) delete item->widget();
    }
    
    QMap<QString, double> ticker_profits;
    for (const auto& d : deals) ticker_profits[d.ticker] += d.profit;
    
    auto keys = ticker_profits.keys();
    std::sort(keys.begin(), keys.end(), [&](const QString& a, const QString& b) {
        return ticker_profits[a] > ticker_profits[b];
    });
    
    int display_count = std::min(10, static_cast<int>(keys.size()));
    if (display_count == 0) {
        ticker_vbox->addWidget(new QLabel("No data available", ticker_group));
        return;
    }
    
    double max_abs_profit = 0;
    for (auto it = ticker_profits.begin(); it != ticker_profits.end(); ++it) max_abs_profit = std::max(max_abs_profit, std::abs(it.value()));
    if (max_abs_profit == 0) max_abs_profit = 1;
    
    for (int i = 0; i < display_count; ++i) {
        ticker_vbox->addWidget(new TickerBar(keys[i], ticker_profits[keys[i]], max_abs_profit, ticker_group));
    }
}

void StatisticsTab::setEmptyState() {
    winrate_card->updateValue("0.0%");
    total_profit_card->updateValue("$0.00", "#ffffff");
    profit_factor_card->updateValue("0.00");
    avg_win_card->updateValue("$0.00");
    avg_loss_card->updateValue("$0.00");
    total_deals_card->updateValue("0");
    winrate_bar->setValue(0);
    winrate_bar->setFormat("No Data");
    
    while (ticker_vbox->count() > 1) {
        auto item = ticker_vbox->takeAt(1);
        if (item->widget()) delete item->widget();
    }
    ticker_vbox->addWidget(new QLabel("No data available", ticker_group));
}
