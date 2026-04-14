#include "deals_window.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>

DealsWindow::DealsWindow(DealsProvider provider, QWidget* parent)
    : QWidget(parent), deals_provider(provider) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* filter_layout = new QHBoxLayout();
    filter_layout->addWidget(new QLabel("Filter by:", this));
    
    period_combo = std::make_unique<QComboBox>(this);
    period_combo->addItems({"All", "Day", "Week", "Month", "Year", "Custom Period"});
    connect(period_combo.get(), &QComboBox::currentTextChanged, this, &DealsWindow::onFilterChanged);
    filter_layout->addWidget(period_combo.get());

    start_date = std::make_unique<QDateEdit>(this);
    start_date->setCalendarPopup(true);
    start_date->setDate(QDate::currentDate().addDays(-30));
    start_date->setEnabled(false);
    connect(start_date.get(), &QDateEdit::dateChanged, this, &DealsWindow::onFilterChanged);
    
    end_date = std::make_unique<QDateEdit>(this);
    end_date->setCalendarPopup(true);
    end_date->setDate(QDate::currentDate());
    end_date->setEnabled(false);
    connect(end_date.get(), &QDateEdit::dateChanged, this, &DealsWindow::onFilterChanged);

    filter_layout->addWidget(new QLabel("From:", this));
    filter_layout->addWidget(start_date.get());
    filter_layout->addWidget(new QLabel("To:", this));
    filter_layout->addWidget(end_date.get());
    
    filter_layout->addSpacing(20);
    filter_layout->addWidget(new QLabel("Ticker:", this));
    ticker_filter = std::make_unique<QLineEdit>(this);
    ticker_filter->setPlaceholderText("Search ticker...");
    ticker_filter->setFixedWidth(100);
    connect(ticker_filter.get(), &QLineEdit::textChanged, this, &DealsWindow::onFilterChanged);
    filter_layout->addWidget(ticker_filter.get());

    filter_layout->addSpacing(20);
    filter_layout->addWidget(new QLabel("Profit:", this));
    profit_filter = std::make_unique<QComboBox>(this);
    profit_filter->addItems({"All", "Positive", "Negative"});
    connect(profit_filter.get(), &QComboBox::currentTextChanged, this, &DealsWindow::onFilterChanged);
    filter_layout->addWidget(profit_filter.get());
    
    filter_layout->addStretch();
    layout->addLayout(filter_layout);

    table = std::make_unique<QTableWidget>(this);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"Date", "Ticker", "Strategy", "Profit", "Count", "Open By", "Close By"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table.get());

    updateTable();
}

void DealsWindow::onFilterChanged() const
{
    bool is_custom = period_combo->currentText() == "Custom Period";
    start_date->setEnabled(is_custom);
    end_date->setEnabled(is_custom);
    updateTable();
}

void DealsWindow::refreshData() const
{
    updateTable();
}

void DealsWindow::updateTable() const
{
    table->setRowCount(0);
    QString period = period_combo->currentText();
    QString ticker_text = ticker_filter->text().toLower();
    QString profit_type = profit_filter->currentText();
    QDate today = QDate::currentDate();
    
    auto deals = deals_provider();
    
    QList<std::pair<Deal, Strategy>> filtered_deals;
    for (const auto& pair : deals) {
        const Deal& deal = pair.first;
        QDate deal_date = deal.date;
        if (!deal_date.isValid()) continue;

        bool date_match = false;
        if (period == "All") date_match = true;
        else if (period == "Day") date_match = (deal_date == today);
        else if (period == "Week") date_match = (deal_date >= today.addDays(-today.dayOfWeek() + 1));
        else if (period == "Month") date_match = (deal_date.month() == today.month() && deal_date.year() == today.year());
        else if (period == "Year") date_match = (deal_date.year() == today.year());
        else if (period == "Custom Period") date_match = (deal_date >= start_date->date() && deal_date <= end_date->date());

        if (!date_match) continue;
        if (!ticker_text.isEmpty() && !deal.ticker.toLower().contains(ticker_text)) continue;
        if (profit_type == "Positive" && deal.profit <= 0) continue;
        if (profit_type == "Negative" && deal.profit >= 0) continue;

        filtered_deals.append(pair);
    }

    std::sort(filtered_deals.begin(), filtered_deals.end(), [](const auto& a, const auto& b) {
        return a.first.date > b.first.date;
    });

    for (const auto& pair : filtered_deals) {
        const Deal& deal = pair.first;
        const Strategy& strat = pair.second;
        int row = table->rowCount();
        table->insertRow(row);
        
        table->setItem(row, 0, new QTableWidgetItem(deal.date.toString("yyyy-MM-dd")));
        table->setItem(row, 1, new QTableWidgetItem(deal.ticker));
        table->setItem(row, 2, new QTableWidgetItem(strat.name));
        
        QString profit_str = deal.currency.startsWith("$") ? 
            QString("%1%2").arg(deal.currency).arg(deal.profit, 0, 'f', 2) :
            QString("%1%2").arg(deal.profit, 0, 'f', 2).arg(deal.currency);
        
        QTableWidgetItem* profit_item = new QTableWidgetItem(profit_str);
        if (deal.profit > 0) profit_item->setForeground(Qt::green);
        else if (deal.profit < 0) profit_item->setForeground(Qt::red);
        table->setItem(row, 3, profit_item);

        table->setItem(row, 4, new QTableWidgetItem(QString::number(deal.count)));
        table->setItem(row, 5, new QTableWidgetItem(deal.open_by));
        table->setItem(row, 6, new QTableWidgetItem(deal.close_by));
    }
}
