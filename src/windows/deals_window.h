#ifndef DEALS_WINDOW_H
#define DEALS_WINDOW_H

#include <memory>
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <functional>
#include "models/deal.h"
#include "models/strategy.h"

class DealsWindow : public QWidget {
    Q_OBJECT
public:
    using DealsProvider = std::function<QList<std::pair<Deal, Strategy>>()>;
    explicit DealsWindow(DealsProvider provider, QWidget* parent = nullptr);

    void refreshData() const;

private slots:
    void onFilterChanged() const;

private:
    void updateTable() const;

    DealsProvider deals_provider;
    std::unique_ptr<QComboBox> period_combo;
    std::unique_ptr<QDateEdit> start_date;
    std::unique_ptr<QDateEdit> end_date;
    std::unique_ptr<QLineEdit> ticker_filter;
    std::unique_ptr<QComboBox> profit_filter;
    std::unique_ptr<QTableWidget> table;
};

#endif // DEALS_WINDOW_H
